/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_FUNCTION_HPP
#define ADOBE_FUNCTION_HPP

#include <algorithm>
#include <exception>

#include <tr1/functional>
#include <tr1/type_traits>

#include <boost/bind.hpp>

#include <adobe/cstdint.hpp>
#include <adobe/string.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

class marshaled_exception : public std::exception {
 public:
    explicit marshaled_exception(string_t message) : message_m(move(message)) { }
    ~marshaled_exception() throw() { }
    const char* what() const throw() { return message_m.c_str(); }
 private:
    string_t message_m;
};

const char* current_exception_what() {
    const char* result = "unknown_exception";
    
    try { throw; }
    catch (const std::exception& error) {
        if (error.what()) result = error.what();
    } catch (...) { }
    
    return result;
}

struct void_;
typedef void_* null_ptr_t;

/**************************************************************************************************/

namespace implementation {

template <typename T>
class function_base {
 public:
    // [3.7.2.2] function modifiers
    void swap(T& x) { std::swap(self()->object_m, x.object_m); }
 
    // [3.7.2.3] function capacity
    operator bool() const { return self()->object_m; }
    
    // [3.7.2.5] function target access
    /* Not provided:
        const std::type_info& target_type() const;
        template <typename T> T* target();
        template <typename T> const T* target() const;
    */
    
    friend inline void swap(T& x, T& y) { x.swap(y); }
 private: 
    operator int() const; // for safe bool cast
    
    T* self() { return static_cast<T*>(this); }
    const T* self() const { return static_cast<const T*>(this); }
};

template <typename Concept, typename Apply>
struct vtable {
    void (*destroy)(Concept*);
    Concept* (*copy)(const Concept*, string_t& message);
    Apply apply;
};
    
template <typename Concept, typename V>
struct concept_base_t {
    explicit concept_base_t(const V* x) : vtable_m(x) { }
    
    Concept* copy() const {
        string_t message;
        Concept* result = vtable_m->copy(self(), message);
        if (message) throw marshaled_exception(move(message));
        return result;
    }
    
    void destroy() { vtable_m->destroy(self()); }
    
    Concept* self() { return static_cast<Concept*>(this); }
    const Concept* self() const { return static_cast<const Concept*>(this); }
    
    const V* vtable_m;
};

template <typename Model,
          typename Concept>
struct model_base : Concept {
    template <typename T>
    explicit model_base(T x) : Concept(x) { }
    
    static Model* self(Concept* x) { return static_cast<Model*>(x); }
    static const Model* self(const Concept* x) { return static_cast<const Model*>(x); }
    
    static void destroy(Concept* x) { delete self(x); }
    
    static Concept* copy(const Concept* x, string_t& message) {
        Concept* result = 0;
        
        try {
            result = new Model(*self(x));
        } catch (...) {
            message = current_exception_what();
        }
        
        return result;
    }
};

template <typename F, typename T>
typename F::concept_t* make_model(const T& x) {
    return new typename F::template model<T>(x);
}

template <typename F, typename T>
typename F::concept_t* create_model(const T& x, std::tr1::true_type) {
    return x ? make_model<F>(boost::bind<typename F::result_type>(x)) : 0;
}

template <typename F, typename T>
typename F::concept_t* create_model(const T& x, std::tr1::false_type) {
    return make_model<F>(boost::bind<typename F::result_type>(x));
}

template <typename F, typename T>
typename F::concept_t* create_model(const T& x) {
    return create_model<F>(x, std::tr1::is_pointer<T>());
}

} // namespace implementation

/**************************************************************************************************/

namespace version_1 {

/**************************************************************************************************/

/*!
    \requires \c R must have a non-throwing default constructor. \c R and all argument types
    must be abi safe. Any arguments passed by value must have a non-throwing copy constructor.
*/

template <typename F> class function;

/**************************************************************************************************/

// [3.7.2.7] null pointer comparisons
template <typename F> bool operator==(const function<F>& x, null_ptr_t) { return !x; }
template <typename F> bool operator==(null_ptr_t, const function<F>& x) { return !x; }
template <typename F> bool operator!=(const function<F>& x, null_ptr_t) { return x; }
template <typename F> bool operator!=(null_ptr_t, const function<F>& x) { return x; }

/**************************************************************************************************/

template <typename R>
class function<R ()> : public implementation::function_base<function<R ()> >
{
 public:
    typedef R result_type;
 
    // [3.7.2.1] construction/copy/destroy
    function() : object_m(0) { }
    function(null_ptr_t) : object_m(0) { }
    function(const function& x) : object_m(x.object_m ? x.object_m->copy() : 0) { }
    template <typename T> function(T x) : object_m(implementation::create_model<function>(x)) { }
    
    function& operator=(function x) { swap(*this, x); return *this; }
    function& operator=(null_ptr_t) { if (object_m) object_m->destroy(); object_m = 0; }
    template <typename T> function& operator=(T x) { return *this = function(x); }
    // template<class F> function& operator=(reference_wrapper<F>);
    
    ~function() { if (object_m) object_m->destroy(); }
    
    // [3.7.2.4] function invocation
    R operator()() const {
        if (!object_m) throw std::tr1::bad_function_call();
        return object_m->apply();
    }
    
    struct concept_t;  // implementation

  private:
    friend class implementation::function_base<function<R ()> >;

    typedef implementation::vtable<concept_t, R (*)(concept_t*, string_t&)> vtable_type;
    
    /*
    REVISIT (sparent) : This section is an implementation detail - made public until I can figure
    out how to make template friends.
    */
  
  public:
  
    struct concept_t : implementation::concept_base_t<concept_t, vtable_type> {
        typedef implementation::concept_base_t<concept_t, vtable_type> base_type;
                
        explicit concept_t(const vtable_type* x) : base_type(x) { }
        R apply() {
            return apply(typename std::tr1::is_void<R>());
        }
        
        void apply(std::tr1::true_type) {
            string_t message;
            this->vtable_m->apply(this, message);
            if (message) throw marshaled_exception(move(message));
        }
        
        R apply(std::tr1::false_type) {
            string_t message;
            R result = this->vtable_m->apply(this, message);
            if (message) throw marshaled_exception(move(message));
            return result;
        }
    };

    template <typename T>
    struct model : implementation::model_base<model<T>, concept_t> {
        typedef implementation::model_base<model<T>, concept_t> base_type;
        
        explicit model(T x) : base_type(&vtable_s), function_m(x) { }
                
        static R apply(concept_t* x, string_t& message) {
            return apply(x, message, typename std::tr1::is_void<R>());
        }
        
        static void apply(concept_t* x, string_t& message, std::tr1::true_type) {
            try {
                static_cast<model*>(x)->function_m();
            } catch (...) {
                message = current_exception_what();
            }
        }
        
        static R apply(concept_t* x, string_t& message, std::tr1::false_type) {
            R result;
            try {
                result = static_cast<model*>(x)->function_m();
            } catch (...) {
                message = current_exception_what();
            }
            return result;
        }
        
        static const vtable_type vtable_s;
        T function_m;
    };
    
 private:
    concept_t* object_m;
};

template <typename R>
template <typename T>
const typename function<R ()>::vtable_type function<R ()>::model<T>::vtable_s = {
    &model::destroy,
    &model::copy,
    &model::apply
};

/**************************************************************************************************/

} // namespace version_1
using namespace version_1;
} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
