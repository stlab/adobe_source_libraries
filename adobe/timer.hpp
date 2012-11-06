/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_TIMER_HPP
#define ADOBE_TIMER_HPP

/****************************************************************************************************/

/*!
\class timer_t
\ingroup testing

\brief  A class for measuring periods of elapsed time. Can also
        accrue split times for some basic statistical reporting.

Comparisons between two <code>timer_t</code> considers only the last split time.

\model_of
    - \ref concept_regular_type
    - \ref stldoc_LessThanComparable

\rationale
    A class that measures elapsed time can be useful when debugging, optimizing, or comparing
    implementations. <code>timer_t</code> utilizes the most accurate timing API a
    platform has to offer to maximize accuracy, while keeping the public API generic.

\example
\par
Checking the speed between two implementations might look like this:
\par
\code
    timer_t  timer1;
        do_my_func();
    timer1.split();

    timer_t  timer2;
        do_their_func();
    timer2.split();

    if (timer1 == timer2)
        std::cout << "Functions are equally fast" << std::endl;
    else if (timer1 < timer2)
        std::cout << "My function is faster" << std::endl;
    else
        std::cout << "Their function is faster" << std::endl;
\endcode
\par
Most of the time, however, you will want to take several measurements and compare the averages
in order to gain a more accurate understanding of the cost of an implementation:
\code
    timer_t  timer1;
    timer_t  timer2;

    for (std::size_t i(0); i < sample_count_k; ++i)
    {
        timer1.reset();
        do_my_func();
        timer1.accrue();

        timer2.reset();
        do_their_func();
        timer2.accrue();
    }

    double my_avg(timer1.accrued_average());
    double their_avg(timer2.accrued_average());

    if (my_avg == their_avg)
        std::cout << "Functions are equally fast" << std::endl;
    else if (my_avg < their_avg)
        std::cout << "My function is faster" << std::endl;
    else
        std::cout << "Their function is faster" << std::endl;
\endcode
\par
Note that in the above case the two implementations are tested in the same <code>for</code> loop.
This is in attempt to distribute possible overhead from memory locality issues (i.e.,
<code>do_my_func</code> could push memory used by <code>do_their_func</code> out of the cache,
unfairly slowing <code>do_their_func</code> down (or vice versa).
\par
Depending on what you are measuring in your own code, measurement strategies may vary from the
ones used above.
*/

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/algorithm/minmax.hpp>
#include <adobe/algorithm/sort.hpp>
#include <adobe/numeric.hpp>

#include <boost/operators.hpp>

#if ADOBE_PLATFORM_WIN
    #ifndef WINDOWS_LEAN_AND_MEAN
        #define WINDOWS_LEAN_AND_MEAN
        #define ADOBE_UNDEFINE_WINDOWS_LEAN_AND_MEAN 1
    #endif
    #include <windows.h>
    #if ADOBE_UNDEFINE_WINDOWS_LEAN_AND_MEAN
        #undef WINDOWS_LEAN_AND_MEAN
        #undef ADOBE_UNDEFINE_WINDOWS_LEAN_AND_MEAN
    #endif
#elif defined(BOOST_HAS_THREADS)
    #include <boost/thread/xtime.hpp>
#elif defined(BOOST_HAS_GETTIMEOFDAY)
    #include <sys/time.h>
#endif

#include <iostream>
#include <vector>
#include <cassert>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

class timer_t : boost::totally_ordered<timer_t>
{
#if ADOBE_PLATFORM_WIN
    typedef LARGE_INTEGER               value_type;
#elif defined(BOOST_HAS_THREADS)
    typedef boost::xtime                value_type;
#elif defined(BOOST_HAS_GETTIMEOFDAY)
    typedef timeval                     value_type;
#endif

    typedef std::vector<double>         accumulator_type;

public:
    typedef accumulator_type::size_type size_type;

#ifndef ADOBE_NO_DOCUMENTATION

    timer_t()
    {
#if ADOBE_PLATFORM_WIN
        (void)::QueryPerformanceFrequency(&frequency_m);
#endif

        reset();
    }

    timer_t(const timer_t& rhs) :
        epoch_m(rhs.epoch_m),
        split_m(rhs.split_m),
        time_set_m(rhs.time_set_m)
#if ADOBE_PLATFORM_WIN
        , frequency_m(rhs.frequency_m)
#endif
    { }

    timer_t& operator = (const timer_t& rhs)
    {
        epoch_m = rhs.epoch_m;
        split_m = rhs.split_m;
        time_set_m = rhs.time_set_m;

#if ADOBE_PLATFORM_WIN
        frequency_m = rhs.frequency_m;
#endif

        return *this;
    }

#endif

    /*!
        Resets the epoch of the timer to now
    */

    inline void reset()
    {
#if ADOBE_PLATFORM_WIN
        (void)::QueryPerformanceCounter(&epoch_m);
#elif defined(BOOST_HAS_THREADS)
        boost::xtime_get(&epoch_m, boost::TIME_UTC);
#elif defined(BOOST_HAS_GETTIMEOFDAY)
        gettimeofday(&epoch_m, static_cast<struct timezone*>(0));
#endif
    }

    /*!
        Resets the split time accumulator
    */

    inline void reset_accumulator()
    { time_set_m.clear(); }

    /*!
        \return
            The difference of time between the epoch and now, in milliseconds
    */

    inline double split()
    {
#if ADOBE_PLATFORM_WIN
        (void)::QueryPerformanceCounter(&split_m);
        return (split_m.QuadPart - epoch_m.QuadPart) / static_cast<double>(frequency_m.QuadPart) * double(1e3);
#elif defined(BOOST_HAS_THREADS)
        boost::xtime_get(&split_m, boost::TIME_UTC);
        return ((split_m.sec - epoch_m.sec) * double(1e3) + (split_m.nsec - epoch_m.nsec) / double(1e6));
#elif defined(BOOST_HAS_GETTIMEOFDAY)
        gettimeofday(&split_m, static_cast<struct timezone*>(0));
        return ((split_m.tv_sec - epoch_m.tv_sec) * double(1e3) + (split_m.tv_usec - epoch_m.tv_usec) / double(1e3));
#else
        return -1;
#endif

    }

    /*!
        Grabs a new split time and stores it in the accumulator.
    */

    inline void accrue()
    { time_set_m.push_back(split()); reset(); }

    /*!
        \return
            The smallest of the split times in the accumulator, in milliseconds
    */

    inline double accrued_min() const
    { return *adobe::min_element(time_set_m); }

    /*!
        \return
            The largest of the split times in the accumulator, in milliseconds
    */

    inline double accrued_max() const
    { return *adobe::max_element(time_set_m); }

    /*!
        \return
            The average of the split times in the accumulator, in milliseconds
    */

    inline double accrued_average() const
    { return empty() ? 0 : accrued_total() / size(); }

    /*!
        \return
            The median of the split times in the accumulator, in milliseconds
    */

    inline double accrued_median() const
    {
        if (empty())
            return 0;

        adobe::sort(time_set_m);

        return (size() % 2 == 1) ?
                time_set_m[time_set_m.size() / 2] :
                (time_set_m[time_set_m.size() / 2] +
                    time_set_m[time_set_m.size() / 2 - 1]) / 2;
    }

    /*!
        \return
            The summation of the split times in the accumulator, in milliseconds
    */

    inline double accrued_total() const
    { return adobe::accumulate(time_set_m, double(0)); }

    /*!
        \return
            The number of split times in the accumulator
    */

    inline size_type size() const
    { return time_set_m.size(); }

    /*!
        \return
            Whether or not there are any split times in the accumulator
    */

    inline bool empty() const
    { return time_set_m.empty(); }

    /*!
        An archaic utility function that takes a new split time and outputs it to a stream

        \param decoration   An identifier to apply to the split time.
        \param s            The stream to which output is written. Defaults to <code>std::cout</code>
    */

    inline void report(const char* decoration, std::ostream& s = std::cout)
    {
        double time(split());

        s << decoration << " took " << time <<  " milliseconds (" << time / 1e3 << " sec)" << std::endl;

        reset();
    }

private:
#ifndef ADOBE_NO_DOCUMENTATION
    friend bool operator == (const timer_t& x, const timer_t& y);
    friend bool operator < (const timer_t& x, const timer_t& y);
#endif

    value_type                  epoch_m;
    value_type                  split_m;
    mutable accumulator_type    time_set_m;
#if ADOBE_PLATFORM_WIN
    value_type                  frequency_m;
#endif
};

/****************************************************************************************************/

#ifndef ADOBE_NO_DOCUMENTATION

inline bool operator == (const timer_t& /*x*/, const timer_t& /*y*/)
{ return true; }

/****************************************************************************************************/

inline bool operator < (const timer_t& /*x*/, const timer_t& /*y*/)
{ return false; }

#endif

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
