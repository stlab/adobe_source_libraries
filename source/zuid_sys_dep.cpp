/*
    Copyright 2005-2007 Adobe Systems Incorporated and others
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <boost/version.hpp>

#include <adobe/implementation/zuid_sys_dep.hpp>
#include <adobe/config.hpp>
#include <adobe/once.hpp>
#include <adobe/thread_id.hpp>

#include <cstring>

#if ADOBE_PLATFORM_WIN
    #include <windows.h>
#endif

#if defined(BOOST_MSVC)
    #pragma warning ( push )
    #pragma warning ( disable : 4244 ) // conversion from type to type, possible loss of data
    #pragma warning ( disable : 4996 ) // c runtime library deprecated function warning
#endif

#include <boost/date_time/posix_time/posix_time_types.hpp>

#if defined(BOOST_MSVC)
    #pragma warning ( pop )
#endif

#if defined(BOOST_HAS_THREADS)
    #include <boost/thread/xtime.hpp>
#endif

#if defined(BOOST_HAS_GETTIMEOFDAY)
    #include <sys/time.h>
#endif

/*************************************************************************************************/

/*
    Relevant copyright information is provided below and may not be removed from this file. 
*/

/*************************************************************************************************/

/*
    Copyright (c) 1990 - 1993, 1996 Open Software Foundation, Inc.
    Copyright (c) 1989 by Hewlett-Packard Company, Palo Alto, Ca. &
    Digital Equipment Corporation, Maynard, Mass.
    Copyright (c) 1998 Microsoft.

    To anyone who acknowledges that this file is provided "AS IS" without any 
    express or implied warranty: permission to use, copy, modify, and 
    distribute this file for any purpose is hereby granted without fee, 
    provided that the above copyright notices and this notice appears in all 
    source code copies, and that none of the names of Open Software Foundation, 
    Inc., Hewlett-Packard Company, or Digital Equipment Corporation be used in 
    advertising or publicity pertaining to distribution of the software without 
    specific, written prior permission. Neither Open Software Foundation, Inc., 
    Hewlett-Packard Company, Microsoft, nor Digital Equipment Corporation makes 
    any representations about the suitability of this software for any purpose.
*/

/*************************************************************************************************/

ADOBE_ONCE_DECLARATION(zuid_sys_dep_once)

/*************************************************************************************************/

namespace {

/*************************************************************************************************/

/* data type for UUID generator persistent state */
struct uuid_state_t
{
    adobe::uuid_time_t  ts_m;   /* saved timestamp */
    adobe::uuid_node_t  node_m; /* saved node ID */
    boost::uint16_t     cs_m;   /* saved clock sequence */
};

/*************************************************************************************************/

ADOBE_THREAD_LOCAL_STORAGE(uuid_state_t, zuid_uuid_state)
ADOBE_THREAD_LOCAL_STORAGE_1(bool, zuid_uuid_state_inited, false)

ADOBE_THREAD_LOCAL_STORAGE(adobe::uuid_node_t, zuid_ieee_node)
ADOBE_THREAD_LOCAL_STORAGE_1(bool, zuid_ieee_node_inited, false)

/*************************************************************************************************/

void init_zuid_sys_dep_once()
{
    ADOBE_THREAD_LOCAL_STORAGE_INITIALIZE(zuid_uuid_state);
    ADOBE_THREAD_LOCAL_STORAGE_INITIALIZE(zuid_uuid_state_inited);
    ADOBE_THREAD_LOCAL_STORAGE_INITIALIZE(zuid_ieee_node);
    ADOBE_THREAD_LOCAL_STORAGE_INITIALIZE(zuid_ieee_node_inited);
}

/*************************************************************************************************/

} // namespace

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

namespace {

/*************************************************************************************************/

adobe::md5_t::digest_t get_generic_random_info()
{
    struct randomness
    {
        randomness() :
            thread_id_m(thread_id())
#if defined(BOOST_HAS_UNISTD_H)
            , pid_m(getpid()),
            uid_m(getuid()),
            gid_m(getgid())
#endif
        {
#if defined(BOOST_HAS_THREADS)
            boost::xtime_get(&time_m, boost::TIME_UTC);
#endif
#if defined(BOOST_HAS_UNISTD_H)
            gethostname(hostname_m, 256);
#endif
#if defined(BOOST_HAS_GETTIMEOFDAY)
            gettimeofday(&timeval_m, static_cast<struct timezone*>(0));
#endif
        }

        thread_id_t     thread_id_m;
#if defined(BOOST_HAS_THREADS)
        boost::xtime    time_m;
#endif
#if defined(BOOST_HAS_UNISTD_H)
        pid_t           pid_m;
        uid_t           uid_m;
        gid_t           gid_m;
        char            hostname_m[257];
#endif
#if defined(BOOST_HAS_GETTIMEOFDAY)
        timeval         timeval_m;
#endif
    };

    randomness r;

    return adobe::md5(&r, sizeof(r));
}

/*************************************************************************************************/

#if ADOBE_PLATFORM_WIN

/*************************************************************************************************/

adobe::md5_t::digest_t get_random_info()
{
    struct randomness
    {
        randomness() :
            generic_random_m(get_generic_random_info()),
            tc(::GetTickCount()),
            l(MAX_COMPUTERNAME_LENGTH + 1),
            processID(::GetCurrentProcessId())
        {
            GlobalMemoryStatus(&m);
            GetSystemInfo(&s);
            GetSystemTimeAsFileTime(&t);
            QueryPerformanceCounter(&pc);
            GetComputerNameA(hostname, &l);
        }

        adobe::md5_t::digest_t  generic_random_m;
        MEMORYSTATUS            m;  /* memory usage stats */
        SYSTEM_INFO             s;  /* random system stats */
        FILETIME                t;  /* 100ns resolution (nominally) time of day */
        LARGE_INTEGER           pc; /* high resolution performance counter */
        DWORD                   tc; /* milliseconds since last boot */
        DWORD                   l;
        DWORD                   processID;
        char                    hostname[MAX_COMPUTERNAME_LENGTH + 1];
    };

    randomness r;

    return adobe::md5(&r, sizeof(r));
}

/*************************************************************************************************/

#elif defined(BOOST_HAS_THREADS) || defined(BOOST_HAS_UNISTD_H) || defined(BOOST_HAS_GETTIMEOFDAY)

/*************************************************************************************************/

adobe::md5_t::digest_t get_random_info()
{
    return get_generic_random_info();
}

/*************************************************************************************************/

#endif

/*************************************************************************************************/

void random_address(uuid_node_t* node)
{
    adobe::md5_t::digest_t seed(get_random_info());

    seed[0] |= 0x80; // Mark this as multicast

    std::memcpy(node, &seed[0], sizeof(uuid_node_t));
}

/*************************************************************************************************/

} // namespace

/*************************************************************************************************/

void get_system_time(uuid_time_t* uuid_time)
{
    // REVISIT (fbrereto) : universal_time() is only available on the second_clock.
    //                      It would be nice if it was one on the microsec_clock. 

    boost::posix_time::ptime then(boost::gregorian::date(1585, boost::date_time::Oct, 15));
#if BOOST_VERSION < 103300
    boost::posix_time::ptime now(boost::date_time::second_clock<boost::gregorian::date, boost::posix_time::ptime>::universal_time());
#else
    boost::posix_time::ptime now(boost::date_time::second_clock<boost::posix_time::ptime>::universal_time());
#endif
    *uuid_time = static_cast<uuid_time_t>((now - then).total_nanoseconds() /
                    boost::posix_time::time_duration::ticks_per_second());
}

/*************************************************************************************************/

void get_ieee_node_identifier(uuid_node_t* node)
{
    ADOBE_ONCE_INSTANCE(zuid_sys_dep_once);
    
    bool&                   inited(ADOBE_THREAD_LOCAL_STORAGE_ACCESS(zuid_ieee_node_inited));
    adobe::uuid_node_t&     saved_node(ADOBE_THREAD_LOCAL_STORAGE_ACCESS(zuid_ieee_node));

    if (!inited) 
    {
        random_address(&saved_node);

        inited = true;
    }

    *node = saved_node;
}

/*************************************************************************************************/

/*
    generate a crypto-quality random number.
    This sample doesn't do that, but it's closer than the last one.
*/

boost::uint64_t true_random()
{
    boost::uint64_t         result;
    adobe::md5_t::digest_t  seed(get_random_info());

    std::memcpy(&result, &seed[0], sizeof(result));

    return result;
}

/*************************************************************************************************/

/* read_state -- read UUID generator state from non-volatile store */
boost::int16_t read_state(boost::uint16_t* clockseq, uuid_time_t* timestamp, uuid_node_t* node)
{
    ADOBE_ONCE_INSTANCE(zuid_sys_dep_once);
    
    bool& state_inited(ADOBE_THREAD_LOCAL_STORAGE_ACCESS(zuid_uuid_state_inited));

    if (!state_inited) return boost::int16_t(0);

    uuid_state_t& state(ADOBE_THREAD_LOCAL_STORAGE_ACCESS(zuid_uuid_state));

    *clockseq   = state.cs_m;
    *timestamp  = state.ts_m;
    *node       = state.node_m;

    return boost::int16_t(1);
}

/* write_state -- save UUID generator state back to non-volatile storage */
void write_state(boost::uint16_t clockseq, uuid_time_t timestamp, uuid_node_t node)
{
    ADOBE_ONCE_INSTANCE(zuid_sys_dep_once);
    
    bool&           state_inited(ADOBE_THREAD_LOCAL_STORAGE_ACCESS(zuid_uuid_state_inited));
    uuid_state_t&   state(ADOBE_THREAD_LOCAL_STORAGE_ACCESS(zuid_uuid_state));

    /* always save state to volatile shared state */
    state.cs_m  = clockseq;
    state.ts_m  = timestamp;
    state.node_m = node;

    state_inited = true;
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

ADOBE_ONCE_DEFINITION(zuid_sys_dep_once, init_zuid_sys_dep_once)

/*************************************************************************************************/
