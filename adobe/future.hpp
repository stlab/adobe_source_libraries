/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_FUTURE_HPP
#define ADOBE_FUTURE_HPP

#include <future>
#include <list>
#include <memory>
#include <type_traits>
#include <utility>

#include <adobe/cassert.hpp>
#include <adobe/type_traits.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

namespace details {

struct any_packaged_task_ {

    any_packaged_task_() = default;

    template <typename T>
    any_packaged_task_(T&& task) : object_(new model_<T>(std::forward<T>(task))) {}

    any_packaged_task_(any_packaged_task_&& x) noexcept = default;
    any_packaged_task_& operator=(any_packaged_task_&& x) noexcept = default;

    void operator()() const {
        ADOBE_ASSERT(object_);
        object_->call();
    }

    struct concept_t {
        virtual ~concept_t() {}
        virtual void call() = 0;
    };

    template <typename T>
    struct model_ : concept_t {
        model_(T&& task) : task_(std::move(task)) {}
        void call() override { task_(); }

        T task_;
    };

    std::unique_ptr<concept_t> object_;
};

void async_(any_packaged_task_&&);
void async_(const std::chrono::steady_clock::time_point&, any_packaged_task_&&);

} // namespace details

/**************************************************************************************************/

template <class F, class... Args>
auto async(F&& f, Args&&... args) -> std::future<typename adobe::invoke_result_t<F, Args...>> {
    using namespace std;

    using result_t = adobe::invoke_result_t<F, Args...>;
    using packaged_t = packaged_task<result_t()>;

    auto p = packaged_t(forward<F>(f), forward<Args>(args)...);
    auto result = p.get_future();

    details::async_(move(p));

    return result;
}

/**************************************************************************************************/

template <class Duration, class F, class... Args>
auto async(const std::chrono::time_point<std::chrono::steady_clock, Duration>& when, F&& f,
           Args&&... args) -> std::future<adobe::invoke_result_t<F, Args...>> {
    using namespace std;
    using namespace chrono;

    using result_t = typename adobe::invoke_result_t<F, Args...>;
    using packaged_t = packaged_task<result_t()>;

    auto p = packaged_t(forward<F>(f), forward<Args>(args)...);
    auto result = p.get_future();

    details::async_(when, move(p));
    return result;
}

/**************************************************************************************************/

// REVISIT (sparent) : This probably is not the correct place for a concurrent queue

template <typename T>
class concurrent_queue {
    using queue_t = std::list<T>;
    using lock_t = std::lock_guard<std::mutex>;

    queue_t q_;
    std::mutex mutex_;

public:
    using value_type = T;

    void push(T x) {
        queue_t tmp;
        tmp.push_back(move(x));
        {
            lock_t lock(mutex_);
            q_.splice(q_.end(), tmp);
        }
    }

    /*
        REVISIT (sparent) : This should return an optional<T>, using reference and bool as
        temporary solution.
    */

    bool pop(T& out) {
        queue_t tmp;
        {
            lock_t lock(mutex_);
            if (!q_.empty())
                tmp.splice(tmp.end(), q_, q_.begin());
        }
        if (tmp.empty())
            return false;
        out = move(tmp.back());
        return true;
    }
};

/**************************************************************************************************/

class shared_task_queue {
    struct task_queue_;

    template <typename P>
    struct move_binder_ {
        move_binder_(std::shared_ptr<task_queue_> q, P&& p) : q_(std::move(q)), p_(std::move(p)) {}

        void operator()() {
            p_();
            continue_(q_);
        }

        std::shared_ptr<task_queue_> q_;
        P p_;
    };

    static auto make_task_queue_() -> std::shared_ptr<task_queue_>;
    void async_(details::any_packaged_task_&&);

    static void continue_(const std::shared_ptr<task_queue_>&);
    static void suspend_(const std::shared_ptr<task_queue_>&);
    static void resume_(const std::shared_ptr<task_queue_>&);

    std::shared_ptr<task_queue_> object_ = make_task_queue_();

public:
    template <class F, class... Args>
    auto async(F&& f, Args&&... args) -> std::future<adobe::invoke_result_t<F, Args...>> {
        using namespace std;

        using result_t = typename adobe::invoke_result_t<F, Args...>;
        using packaged_t = packaged_task<result_t()>;

        auto p = packaged_t(forward<F>(f), forward<Args>(args)...);
        auto result = p.get_future();

        async_(move_binder_<packaged_t>(object_, move(p)));

        return result;
    }

    void suspend() { suspend_(object_); }
    void resume() { resume_(object_); }
};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
