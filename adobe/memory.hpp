/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_MEMORY_HPP
#define ADOBE_MEMORY_HPP

#include <adobe/config.hpp>

#include <cassert>
#include <functional>
#include <memory>

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_const.hpp>

#include <adobe/conversion.hpp>
#include <adobe/functional.hpp>
#include <adobe/memory_fwd.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

/*!
\defgroup memory Memory
\ingroup utility

Augments the functionality of \<memory\>
@{
*/


template <typename T>
struct empty_ptr;

template <typename T>
struct empty_ptr<T*> : std::unary_function<T*, bool> {
    bool operator()(const T* x) const throw() { return x == NULL; }
};


template <typename T>
struct empty_ptr<T (*)[]> : std::unary_function<T*, bool> {
    bool operator()(const T* x) const throw() { return x == NULL; }
};

/*
    REVISIT (sparent) : This is a hack - the new delete_ptr is not a template
    auto_resource is too complicated with the traits classes -
    provide a delete op as a tempalte argument?
*/

template <typename T>
struct delete_ptr_trait;

template <typename T>
struct delete_ptr_trait<T*> : std::unary_function<T*, void> {
    void operator()(const T* x) const { delete x; }
};

template <typename T>
struct delete_ptr_trait<T (*)[]> : std::unary_function<T*, void> {
    void operator()(const T* x) const { delete[] x; }
};

//@}

/*************************************************************************************************/

template <typename X, class Traits>
class auto_resource;

template <typename ptrT>
struct ptr_traits;

//!\ingroup memory
template <typename T>
struct ptr_traits<T (*)[]> {
    typedef T element_type;
    typedef T* pointer_type;
    typedef const T* const_pointer_type;

    template <class U>
    struct rebind {
        typedef adobe::ptr_traits<U> other;
    };
    enum {
        is_array = true
    };

    static void delete_ptr(pointer_type x) throw() { delete_ptr_trait<T(*)[]>()(x); }
    static bool empty_ptr(const_pointer_type x) throw() { return adobe::empty_ptr<T(*)[]>()(x); }
};

/*!
    The ptr_traits class provide basic information and operations associated with a pointer
    type.
*/

/*! \ingroup memory
   \brief This section defines requirements on classes representing <i>pointer traits</i>. The
    template class <code>ptr_traits\< ptrT \></code> is defined along with several specializations.

    A <i>pointer</i> may be any type used to refer to another, possibly not visible, type.

    In the following table, <code>X</code> denotes a Traits class defining types and functions for
    the pointer type PtrT. The type of the item refered to is T. T may be <code>void</code> or an
    incomplete type. The argument <code>p</code> is of type PtrT.

    <table>
        <tr>
            <td><b>expression</b></td>
            <td><b>return type</b></td>
            <td><b>notes</b></td>
            <td><b>complexity</b></td>
        </tr>
        <tr>
            <td><code>X::element_type</code></td>
            <td><code>T</code></td>
            <td>the type of the item refered to</td>
            <td>compile time</td>
        </tr>
        <tr>
            <td><code>X::pointer_type</code></td>
            <td><code>PtrT</code></td>
            <td>if opaque, may be any type used to refer to T</td>
            <td>compile time</td>
        </tr>
        <tr>
            <td><code>X::const_pointer_type</code></td>
            <td>implementation defined</td>
            <td>type corresponding to PtrT refering to a const T</td>
            <td>compile time</td>
        </tr>
        <tr>
            <td><code>X::is_array</code></td>
            <td><code>bool</code></td>
            <td>true iff T is an array; type may also be convertable to bool</td>
            <td>compile time</td>
        </tr>
        <tr>
            <td><code>X::delete_ptr(p)</code></td>
            <td><code>void</code></td>
            <td>destructs and deallocates item refered to by p; if p is empty, delete_ptr() has no
   effect</td>
            <td>implementation defined</td>
        </tr>
        <tr>
            <td><code>X::empty_ptr(p)</code></td>
            <td><code>bool</code></td>
            <td>result is <code>true</code> if <code>p</code> refers to nothing (corresponds to
                <code>NULL</code>); <code>false</code> otherwise</td>
            <td>constant</td>
        </tr>
    </table>
*/
template <typename T>
struct ptr_traits<T*> {
    typedef T element_type;
    typedef T* pointer_type;
    typedef const pointer_type const_pointer_type;

    template <class U>
    struct rebind {
        typedef adobe::ptr_traits<U> other;
    };
    enum {
        is_array = false
    };

    static void delete_ptr(pointer_type x) throw() { adobe::delete_ptr_trait<T*>()(x); }
    static bool empty_ptr(const_pointer_type x) throw() { return adobe::empty_ptr<T*>()(x); }
};

/*************************************************************************************************/

#ifndef NO_DOCUMENTATION

/*
    REVISIT (sparent) : This could use boost::static_assert but it doesn't seem worth adding a
    boost dependency just for this case.
*/

namespace implementation {
template <bool x>
struct adobe_static_assert;
template <>
struct adobe_static_assert<true> {};
}

#endif

/*************************************************************************************************/

/* REVISIT (sparent) : auto_resource should become unique_resource. */

/*! \addtogroup memory
  @{
*/
/*! \brief The template class <code>auto_resource\< X, Traits \></code> provides similar
    functionality to <code>unique_ptr</code> for resources for which the pointer is <i>opaque</i>
    refered to by a non-pointer type.

    <b>Example:</b>
    \dontinclude auto_resource_test.cpp
    \skip start_of_example
    \until end_of_example
*/

template <typename X, class Traits = ptr_traits<X>>
class auto_resource {
    struct clear_type {};
    operator int() const;

public:
    typedef Traits traits_type;
    typedef typename traits_type::element_type element_type;
    typedef typename traits_type::pointer_type pointer_type;

    // 20.4.5.1 construct/copy/destroy:
    explicit auto_resource(pointer_type p = 0) throw();

    auto_resource(auto_resource&) throw();
    template <typename Y>
    auto_resource(const auto_resource<Y, typename traits_type::template rebind<Y>::other>&) throw();

    auto_resource& operator=(auto_resource&) throw();
    template <typename Y>
    auto_resource& operator=(
        auto_resource<Y, typename traits_type::template rebind<Y>::other>) throw();

    ~auto_resource() throw();

    // assignment from NULL
    auto_resource& operator=(const clear_type*) throw();

    // 20.4.5.2 members:
    pointer_type get() const throw();
    pointer_type release() throw();
    void reset(pointer_type p = 0) throw();

    // Safe bool conversion (private int conversion prevents unsafe use)
    operator bool() const throw() { return (pointer_m != NULL); }
    bool operator!() const throw();

private:
/*
    20.4.5.3 conversions:

    NOTE (sparent) : As per the recommendations on standard issue 463 the conversion
    operators through auto_ptr_ref have been removed in favor of using this conditional
    enabled trick.

    http://www.open-std.org/jtc1/sc22/wg21/docs/lwg-active.html#463
*/
// REVISIT: VC 2003 internal compiler error workaround. Some misues of auto_resource will go
// undetected under MSVC until fixed.
// REVISIT: Failes with incomplete types with Apple's LLVM 3.0 compiler.
#if 0
#ifndef BOOST_MSVC 
  template <typename Y> struct error_on_const_auto_type;
    template <typename Y> struct error_on_const_auto_type<auto_resource<Y, typename traits_type::template rebind<Y>::other> const>
    { typedef typename auto_resource<Y, typename traits_type::template rebind<Y>::other>::const_auto_type_is_not_allowed type; };
    
    template <class Y>
    auto_resource(Y& rhs, typename error_on_const_auto_type<Y>::type = 0);
#endif
#endif
    pointer_type pointer_m;
};

/*************************************************************************************************/

template <typename X, class Traits>
inline auto_resource<X, Traits>::auto_resource(pointer_type p) throw()
    : pointer_m(p) {}

template <typename X, class Traits>
inline auto_resource<X, Traits>::auto_resource(auto_resource& x) throw()
    : pointer_m(x.release()) {}

template <typename X, class Traits>
template <typename Y>
inline auto_resource<X, Traits>::auto_resource(
    auto_resource<Y, typename traits_type::template rebind<Y>::other> const& x) throw()
    : pointer_m(const_cast<auto_resource<Y, typename traits_type::template rebind<Y>::other>&>(x)
                    .release()) {}

template <typename X, class Traits>
inline auto_resource<X, Traits>& auto_resource<X, Traits>::operator=(auto_resource& x) throw() {
    reset(x.release());
    return *this;
}

template <typename X, class Traits>
template <typename Y>
inline auto_resource<X, Traits>& auto_resource<X, Traits>::
operator=(auto_resource<Y, typename traits_type::template rebind<Y>::other> x) throw() {
    reset(x.release());
    return *this;
}

template <typename X, class Traits>
inline auto_resource<X, Traits>::~auto_resource() throw() {
    traits_type::delete_ptr(pointer_m);
}

/*************************************************************************************************/

template <typename X, class Traits>
inline auto_resource<X, Traits>& auto_resource<X, Traits>::operator=(const clear_type*) throw() {
    reset();
    return *this;
}

/*************************************************************************************************/

template <typename X, class Traits>
inline typename auto_resource<X, Traits>::pointer_type auto_resource<X, Traits>::get() const
    throw() {
    return pointer_m;
}

template <typename X, class Traits>
inline typename auto_resource<X, Traits>::pointer_type auto_resource<X, Traits>::release() throw() {
    pointer_type result(pointer_m);
    pointer_m = NULL;
    return result;
}

template <typename X, class Traits>
inline void auto_resource<X, Traits>::reset(pointer_type p) throw() {
    if (pointer_m != p) {
        traits_type::delete_ptr(pointer_m);
        pointer_m = p;
    }
}

/*************************************************************************************************/

template <typename X, class Traits>
inline bool auto_resource<X, Traits>::operator!() const throw() {
    return !pointer_m;
}

/*************************************************************************************************/

template <typename T> // T models Regular
inline void destroy(T* p) {
    p->~T();
}

template <typename T> // T models Regular
inline void construct(T* p) {
    ::new (static_cast<void*>(p)) T();
}

template <typename T> // T models Regular
inline void construct(T* p, T x) {
    using std::swap;

    construct(p);
    swap(*p, x);

/*
 REVISIT (sparent): RVO is failing with Xcode 4.3.3 with a direct call to placement new.
 using default construct and swap to speed up. May break if using non-default constructible types.
*/

#if 0
    ::new(static_cast<void*>(p)) T(adobe::move(x));
#endif
}

template <typename F> // F models ForwardIterator
inline void destroy(F f, F l) {
    while (f != l) {
        destroy(&*f);
        ++f;
    }
}

/*************************************************************************************************/

/*!
\brief Similar to std::uninitialized_copy but with move semantics.
*/
template <typename I, // I models InputIterator
          typename F>
// F models ForwardIterator
F uninitialized_move(I f, I l, F r) {
    while (f != l) {
        adobe::construct(&*r, std::move(*f));
        ++f;
        ++r;
    }
    return r;
}

/*************************************************************************************************/

namespace version_1 {

//! \addtogroup memory
//! @{

struct new_delete_t {
    void* (*new_)(std::size_t);
    void (*delete_)(void*);
};

extern const new_delete_t local_new_delete_g;

template <>
class capture_allocator<void> {
public:
    void* pointer;
    typedef const void* const_pointer;
    typedef void value_type;
    template <class U>
    struct rebind {
        typedef capture_allocator<U> other;
    };

    friend inline bool operator==(const capture_allocator&, const capture_allocator&) {
        return true;
    }

    friend inline bool operator!=(const capture_allocator&, const capture_allocator&) {
        return false;
    }
};

template <typename T>
class capture_allocator {
public:
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;
    template <typename U>
    struct rebind {
        typedef capture_allocator<U> other;
    };

    capture_allocator() : new_delete_m(&local_new_delete_g) {}
    template <typename U>
    capture_allocator(const capture_allocator<U>& x)
        : new_delete_m(x.new_delete()) {}

    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }
    pointer allocate(size_type n, capture_allocator<void>::const_pointer = 0) {
        if (n > max_size())
            throw std::bad_alloc();
        pointer result = static_cast<pointer>(new_delete_m->new_(n * sizeof(T)));
        if (!result)
            throw std::bad_alloc();
        return result;
    }
    void deallocate(pointer p, size_type) { new_delete_m->delete_(p); }
    size_type max_size() const { return size_type(-1) / sizeof(T); }
    void construct(pointer p, const T& x) { adobe::construct(p, x); }
    void destroy(pointer p) { adobe::destroy(p); }

    friend inline bool operator==(const capture_allocator& x, const capture_allocator& y) {
        return x.new_delete_m == y.new_delete_m;
    }

    friend inline bool operator!=(const capture_allocator& x, const capture_allocator& y) {
        return x.new_delete_m != y.new_delete_m;
    }

    const new_delete_t* new_delete() const { return new_delete_m; }

private:
    const new_delete_t* new_delete_m;
};

//! @} //end addtogroup memory
/*************************************************************************************************/

} // namespace version_1

/*************************************************************************************************/

/*
    Note (sparent) : The aligned storage class is intended to pad out an item of size_t such that
    anything following it is aligned to the max alignement on the machine - in this case, quadword.

    REVISIT (sparent) : This is a different beast than C++11 aligned storage. See if we can
    replace usage with C++11 aligned storage.
*/

//! \addtogroup memory
//! @{
template <typename T>
struct aligned_storage {
    aligned_storage() { construct(&get()); }

    explicit aligned_storage(T x) { construct(&get(), std::move(x)); }

    ~aligned_storage() { destroy(&get()); }

    aligned_storage(const aligned_storage& x) { construct(&get(), x.get()); }
    aligned_storage(aligned_storage&& x) { construct(&get(), std::move(x.get())); }

    aligned_storage& operator=(aligned_storage x) {
        swap(*this, x);
        return *this;
    }

    T& get() { return *static_cast<T*>(storage()); }
    const T& get() const { return *static_cast<const T*>(storage()); }

    friend inline void swap(aligned_storage& x, aligned_storage& y) { swap(x.get(), y.get()); }

private:
    enum {
        word_size = 16
    }; // quad word alignment

    typedef double storage_t
        [((sizeof(T) + (word_size - 1)) / word_size) * (word_size / sizeof(double))];

    void* storage() { return &data_m; }
    const void* storage() const { return &data_m; }
    storage_t data_m;

    BOOST_STATIC_ASSERT(sizeof(T) <= sizeof(storage_t));
};

//! @} //end addtogroup memory

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
