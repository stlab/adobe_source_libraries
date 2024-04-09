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
#include <optional>

#endif

/**************************************************************************************************/

/*!

This library implements portions of the Microsoft PPL task cancelation API:

cancellation_token_source <http://msdn.microsoft.com/en-us/library/hh749985.aspx>
    - with the exception of create_linked_source()
cancellation_token <http://msdn.microsoft.com/en-us/library/hh749975.aspx>
cancellation_token_registration <http://msdn.microsoft.com/en-us/library/hh750057.aspx>
task_canceled <http://msdn.microsoft.com/en-us/library/hh750110.aspx>
cancel_current_task <http://msdn.microsoft.com/en-us/library/hh749945.aspx>

For detailed documentation on using this library see:
<http://msdn.microsoft.com/en-us/library/dd984117.aspx>

*/

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

#if defined(_MSC_VER)

using concurrency::cancel_current_task;
using concurrency::cancellation_token;
using concurrency::cancellation_token_registration;
using concurrency::cancellation_token_source;
using concurrency::task_canceled;

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
            Avoid heap operations in the lock by inserting the element in a temporary list then
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

[[noreturn]] inline void cancel_current_task() { throw task_canceled(); }

/**************************************************************************************************/

#endif

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
