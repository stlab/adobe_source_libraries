/*
    Copyright 2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_TASK_HPP
#define ADOBE_TASK_HPP

/**************************************************************************************************/

#include <type_traits>
#include <utility>

#if defined(_MSC_VER)

#include <ppltasks.h>

#else

#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mutex>

#endif

/**************************************************************************************************/

/*!

This library implements portions of the Microsoft PPL task cancelation API:

cancellation_token_source <http://msdn.microsoft.com/en-us/library/hh749985.aspx>
    - with the exception of create_linked_source()
cancellation_token <http://msdn.microsoft.com/en-us/library/hh749975.aspx>
cancellation_token_registration <http://msdn.microsoft.com/en-us/library/hh750057.aspx>
task_canceled <http://msdn.microsoft.com/en-us/library/hh750110.aspx>
is_task_cancellation_requested <http://msdn.microsoft.com/en-us/library/hh750070.aspx>
cancel_current_task <http://msdn.microsoft.com/en-us/library/hh749945.aspx>
run_with_cancellation_token <http://msdn.microsoft.com/en-us/library/hh749944.aspx>

For detailed documentation on using this library see:
<http://msdn.microsoft.com/en-us/library/dd984117.aspx>

There are also two additions:
    cancelable_function
    make_cancelable

*/

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

#if defined(_MSC_VER)

using concurrency::cancellation_token_source;
using concurrency::cancellation_token;
using concurrency::cancellation_token_registration;
using concurrency::task_canceled;
using concurrency::is_task_cancellation_requested;
using concurrency::cancel_current_task;
using concurrency::run_with_cancellation_token;

#else

/**************************************************************************************************/

typedef std::function<void()>* cancellation_token_registration;

/**************************************************************************************************/

namespace details {

/**************************************************************************************************/

struct cancel_state {
    typedef std::list<std::function<void()>> list_type;

    cancel_state() : canceled_(false) {}

    void cancel() {
        list_type list;
        {
            std::lock_guard<std::mutex> lock(guard_);
            if (!canceled_) {
                canceled_ = true;
                swap(list, callback_list_);
            }
        }
        for (const auto& f : list)
            f();
    }

    template <typename F> // F models void ()
    cancellation_token_registration register_callback(F f) {
        /*
            Avoid heap oerations in the lock by inserting the element in a temporary list then
            splicing that list into the callback_list_ under the lock.
        */
        list_type list;
        list.push_back(std::move(f));
        cancellation_token_registration result = nullptr;
        bool canceled;
        {
            std::lock_guard<std::mutex> lock(guard_);
            canceled = canceled_;
            if (!canceled) {
                /*
                    We splice the item on the front of the list, under the assumption that
                    if we do deregister, it will likely be in lifo order.
                */
                callback_list_.splice(begin(callback_list_), list);
                result = &callback_list_.back();
            }
        }
        if (canceled)
            list.front()();
        return result;
    }

    void deregister_callback(const cancellation_token_registration& token) {
        /*
            Avoid heap operations or user destructors in the lock by splicing out
            the element to a temporary and allowing it to destruct with the temporary list
            outside the lock.
        */
        list_type list;
        {
            std::lock_guard<std::mutex> lock(guard_);

            auto i = find_if(begin(callback_list_), end(callback_list_),
                             [token](const std::function<void()>& x) { return &x == token; });
            if (i != end(callback_list_))
                list.splice(end(list), callback_list_, i);
        }
    }

    bool is_canceled() const { return canceled_; }

    std::mutex guard_;
    volatile bool canceled_; // Write once, read many
    list_type callback_list_;
};

/**************************************************************************************************/

} // namespace details

/**************************************************************************************************/

class cancellation_token_source;

class cancellation_token {
public:
    template <typename F> // F models void ()
    cancellation_token_registration register_callback(F f) const {
        return state_ ? state_->register_callback(std::move(f)) : nullptr;
    }

    void deregister_callback(const cancellation_token_registration& token) const {
        if (state_)
            state_->deregister_callback(token);
    }

    bool is_cancelable() const { return static_cast<bool>(state_); }


    bool is_canceled() const { return state_ ? state_->is_canceled() : false; }

    static cancellation_token none() { return cancellation_token(); }

    friend inline bool operator==(const cancellation_token& x, const cancellation_token& y) {
        return x.state_ == y.state_;
    }

private:
    friend class cancellation_token_source;
    cancellation_token() {}
    explicit cancellation_token(std::shared_ptr<details::cancel_state> s) : state_(std::move(s)) {}

    std::shared_ptr<details::cancel_state> state_;
};

inline bool operator!=(const cancellation_token& x, const cancellation_token& y) {
    return !(x == y);
}

/**************************************************************************************************/

namespace details {

/**************************************************************************************************/

struct cancellation_scope;

/*
    REVISIT (sparent) : The thread local implementation should be the default implementation.
    The pthread implementation is for current Mac/iOS platforms. Look at what it would take
    to use boost instead of posix for OS X.

*/

#if defined(_MSC_VER) && (1800 <= _MSC_VER)

template <typename T = void> // placeholder to allow header only library
cancellation_scope*& get_cancellation_scope_() {
    __declspec(thread) static cancellation_scope* result = nullptr;
    return result;
}


inline cancellation_scope* get_cancellation_scope() { return get_cancellation_scope_(); }


inline void set_cancellation_scope(cancellation_scope* scope) { get_cancellation_scope_() = scope; }

#else

template <typename T> // placeholder to allow header only library
pthread_key_t get_cancellation_key() {
    static pthread_key_t cancellation_key = 0;        // aggregate initialized
    int r = pthread_key_create(&cancellation_key, 0); // execute once
    (void)r;                                          // avoid unused arg warning from above
    return cancellation_key;
}

inline cancellation_scope* get_cancellation_scope() {
    return static_cast<cancellation_scope*>(pthread_getspecific(get_cancellation_key<void>()));
}


inline void set_cancellation_scope(cancellation_scope* scope) {
    pthread_setspecific(get_cancellation_key<void>(), scope);
}

#endif

struct cancellation_scope {
    cancellation_scope(cancellation_token token) : token_(std::move(token)) {
        prior_ = get_cancellation_scope();
        set_cancellation_scope(this);
    }

    ~cancellation_scope() { set_cancellation_scope(prior_); }

    cancellation_scope* prior_;
    const cancellation_token& token_;
};

/**************************************************************************************************/

} // namespace details

/**************************************************************************************************/

class cancellation_token_source {
public:
    cancellation_token_source() : state_(std::make_shared<details::cancel_state>()) {}

    void cancel() const { state_->cancel(); }

    cancellation_token get_token() const { return cancellation_token(state_); }

    friend inline bool operator==(const cancellation_token_source& x,
                                  const cancellation_token_source& y) {
        return x.state_ == y.state_;
    }

private:
    std::shared_ptr<details::cancel_state> state_;
};

inline bool operator!=(const cancellation_token_source& x, const cancellation_token_source& y) {
    return !(x == y);
}

/**************************************************************************************************/

class task_canceled : public std::exception {
public:
    task_canceled() noexcept {}
    task_canceled(const char* message) : what_(message) {}
    const char* what() const noexcept { return what_.empty() ? "task_canceled" : what_.c_str(); }

private:
    std::string what_;
};

/**************************************************************************************************/

inline bool is_task_cancellation_requested() {
    const details::cancellation_scope* scope = details::get_cancellation_scope();
    return scope ? scope->token_.is_canceled() : false;
}

[[noreturn]] inline void cancel_current_task() { throw task_canceled(); }

/*!
    To be compatible with PPL, this function does nothing if the token is canceled before the
    function is executed.
*/

template <typename F> // F models void ()
inline void run_with_cancellation_token(const F& f, cancellation_token token) {
    details::cancellation_scope scope(std::move(token));
    if (!is_task_cancellation_requested())
        f();
}

/**************************************************************************************************/

#endif

/**************************************************************************************************/

namespace details {

/*
    REVISIT (sparent) : Simple optional - could use Boost but don't want to add the depenency.
*/

template <typename T>
class optional {
public:
    optional() : initialized_(false) {}

    optional& operator=(T&& x) {
        if (initialized_)
            get() = std::forward<T>(x);
        else
            new (&storage_) T(std::forward<T>(x));
        initialized_ = true;
        return *this;
    }

    T& get() {
        assert(initialized_ && "getting unset optional.");
        return *static_cast<T*>(static_cast<void*>(&storage_));
    }

    explicit operator bool() const { return initialized_; }

    ~optional() {
        if (initialized_)
            get().~T();
    }

private:
    optional(const optional&);
    optional operator=(const optional&);

    typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type storage_;

    bool initialized_;
};

} // namespace details

/**************************************************************************************************/

/*!

    A function object that will execute f with the given cancellation_token in a
    run_with_cancellation_token() context. If cancelation is already requested then
    task_canceled is thrown and f is not executed.

*/

template <typename F, typename SIG>
class cancelable_function;
template <typename F, typename R, typename... Arg>
class cancelable_function<F, R(Arg...)> {
public:
    typedef R result_type;

    cancelable_function(F f, cancellation_token token)
        : function_(std::move(f)), token_(std::move(token)) {}

    R operator()(Arg&&... arg) const {
        details::optional<R> r;

        run_with_cancellation_token([&] { r = function_(std::forward<Arg>(arg)...); }, token_);

        if (!r)
            cancel_current_task();
        return std::move(r.get());
    }

private:
    F function_;
    cancellation_token token_;
};

template <typename F, typename... Arg>
class cancelable_function<F, void(Arg...)> {
public:
    typedef void result_type;

    cancelable_function(F f, cancellation_token token)
        : function_(std::move(f)), token_(std::move(token)) {}

    void operator()(Arg&&... arg) const {
        bool executed = false;

        run_with_cancellation_token([&] {
                                        executed = true;
                                        function_(std::forward<Arg>(arg)...);
                                    },
                                    token_);

        if (!executed)
            cancel_current_task();
    }

private:
    F function_;
    cancellation_token token_;
};

template <typename SIG, typename F> // F models void ()
cancelable_function<F, SIG> make_cancelable(F f, cancellation_token token) {
    return cancelable_function<F, SIG>(f, token);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
