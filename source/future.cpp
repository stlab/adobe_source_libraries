/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/future.hpp>

#include <condition_variable>
#include <list>
#include <mutex>
#include <queue>
#include <vector>

using namespace std;
using namespace adobe::details;
using namespace std::chrono;

/**************************************************************************************************/

namespace {

using list_t = list<any_packaged_task_>;

struct thread_pool {

    using lock_t = unique_lock<mutex>;
    using queue_t = list<any_packaged_task_>;

    thread_pool()
    {
        for (auto count = thread::hardware_concurrency(); count != 0; --count) {
            pool_.push_back(thread([this]{ this->_run(); }));
        }
    }

    ~thread_pool()
    {
    #if 0
        for (auto count = pool_.size(); count != 0; --count) async([]{ return true; });
    #endif
        // REVISIT (sparent) : hang on quit
        for (auto& e : pool_) e.join();
    }

    void _run()
    {
        while(true) {
            queue_t tmp;

            {
            lock_t lock(mutex_);
            while (q_.empty()) condition_.wait(lock);
            tmp.splice(tmp.end(), q_, q_.begin());
            }

            tmp.back()();
        }
    }

    void async(any_packaged_task_&& p)
    {
        queue_t tmp;
        tmp.push_back(move(p));

        {
        lock_t lock(mutex_);
        q_.splice(q_.end(), tmp);
        }

        condition_.notify_one();
    }

    queue_t             q_;
    condition_variable  condition_;
    mutex               mutex_;
    vector<thread>      pool_;
};

struct timed_queue {

    struct greater_first {
        template <typename T>
        bool operator()(const T& x, const T& y) const { return x.first > y.first; }
    };

    using lock_t = unique_lock<mutex>;
    using element_t = pair<steady_clock::time_point, any_packaged_task_>;
    using queue_t = vector<element_t>;

    timed_queue() { schedular_ = thread([this]{ this->_run(); }); }

    ~timed_queue() { schedular_.join(); } // REVISIT (sparent) : never happens

    void _run()
    {
        while(true) {
            any_packaged_task_ task;
            {
            lock_t lock(mutex_);

            while (q_.empty()) condition_.wait(lock);
            while (steady_clock::now() < q_.front().first) {
                auto t = q_.front().first;
                condition_.wait_until(lock, t);
            }
            pop_heap(begin(q_), end(q_), greater_first());
            task = move(q_.back().second);
            q_.pop_back();
            }

            adobe::details::async_(move(task));
        }
    }

    void async(const steady_clock::time_point& when, any_packaged_task_&& p)
    {
        {
        lock_t lock(mutex_);
        q_.push_back(element_t(when, move(p)));
        push_heap(begin(q_), end(q_), greater_first());
        }
        condition_.notify_one();
    }

    queue_t             q_;
    condition_variable  condition_;
    mutex               mutex_;
    thread              schedular_;
};


} // namespace

/**************************************************************************************************/

namespace adobe {
namespace details {

void async_(any_packaged_task_&& p)
{
    /*
        The "standard" behavior with GCD, PPL, and the standard library on Windows
        is to simply lead any tasks that haven't been completed die with the process.
        
        If you don't want this behavior, you hang onto the future and get the result,
        allowing that task to complete, but there is mechanism to wait for all tasks
        (which may never happen, especially with timer driven tasks).
        
        So we simply let our thread pool leak and allow the threads to get killed with the
        process.
    */

    static thread_pool pool_s;

    pool_s.async(move(p));
}

void async_(const steady_clock::time_point& when, any_packaged_task_&& p)
{
    static timed_queue queue_s;
    queue_s.async(when, move(p));
}

} // namespace details

/**************************************************************************************************/

struct shared_task_queue::task_queue_
{
    using queue_t = list<any_packaged_task_>;
    using lock_t = unique_lock<mutex>;

    void _suspend()
    {
        lock_t lock(mutex_);
        suspend_ = true;
    }

    void _resume()
    {
        queue_t tmp;

        {
        lock_t lock(mutex_);
        if (!suspend_) return;
        suspend_ = false;
        if (!executing_ && !q_.empty()) {
            tmp.splice(tmp.end(), q_, q_.begin());
            executing_ = true;
        }
        }

        if (!tmp.empty()) details::async_(move(tmp.front()));
    }

    void _continue()
    {
        queue_t tmp;

        {
        lock_t lock(mutex_);
        if (!suspend_ && !q_.empty()) tmp.splice(tmp.end(), q_, q_.begin());
        else executing_ = false;
        }

        if (!tmp.empty()) details::async_(move(tmp.front()));
    }

    void async(any_packaged_task_&& p)
    {
        queue_t tmp;
        tmp.push_back(move(p));

        {
        lock_t lock(mutex_);
        if (!executing_ && !suspend_) { executing_ = true; }
        else q_.splice(q_.end(), tmp);
        }

        if (!tmp.empty()) details::async_(move(tmp.front()));
    }

    queue_t q_;
    bool suspend_ = false;
    bool executing_ = false;
    mutex mutex_;
};

auto shared_task_queue::make_task_queue_() -> std::shared_ptr<task_queue_>
{
    return make_shared<task_queue_>();
}

void shared_task_queue::async_(details::any_packaged_task_&& task)
{
    object_->async(move(task));
}
void shared_task_queue::continue_(const std::shared_ptr<task_queue_>& q)
{
    q->_continue();
}
void shared_task_queue::suspend_(const std::shared_ptr<task_queue_>& q)
{
    q->_suspend();
}
void shared_task_queue::resume_(const std::shared_ptr<task_queue_>& q)
{
    q->_resume();
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
