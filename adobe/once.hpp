/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ONCE_HPP
#define ADOBE_ONCE_HPP

/**************************************************************************************************/

/*

Moving the code to C++11 thread safe statics, using this #define to mark where a static initializer
is assumed to be thread safe. If support is added for platforms where this is not true, we'll have
to do something here.

*/


#define ADOBE_STD_THREAD_LOCAL

/**************************************************************************************************/

#ifdef ADOBE_STD_THREAD_LOCAL

#define ADOBE_THREAD_LOCAL_STORAGE_1(type, signature, ctor_p1)                                     \
    type& adobe_thread_local_storage_##signature##_access() {                                      \
        thread_local type holder {ctor_p1};                                                        \
        return holder;                                                                             \
    }                                                                                              \

#define ADOBE_THREAD_LOCAL_STORAGE(type, signature)                                                \
    type& adobe_thread_local_storage_##signature##_access() {                                      \
        thread_local type holder;                                                                  \
        return holder;                                                                             \
    }                                                                                              \

#define ADOBE_THREAD_LOCAL_STORAGE_INITIALIZE(signature)                                           \


#else

#include <adobe/config.hpp>

#if defined(BOOST_HAS_THREADS)

#include <boost/thread.hpp>

#define ADOBE_THREAD_LOCAL_STORAGE_1(type, signature, ctor_p1)                                     \
    namespace {                                                                                    \
    typedef boost::thread_specific_ptr<type> adobe_thread_local_storage_##signature##_t;           \
    adobe_thread_local_storage_##signature##_t* adobe_thread_local_storage_##signature##_g = 0;    \
    type& adobe_thread_local_storage_##signature##_access();                                       \
    type& adobe_thread_local_storage_##signature##_access() {                                      \
        type* result = adobe_thread_local_storage_##signature##_g->get();                          \
        if (result)                                                                                \
            return *result;                                                                        \
        result = new type(ctor_p1);                                                                \
        adobe_thread_local_storage_##signature##_g->reset(result);                                 \
        return *result;                                                                            \
    }                                                                                              \
    }

#define ADOBE_THREAD_LOCAL_STORAGE(type, signature)                                                \
    namespace {                                                                                    \
    typedef boost::thread_specific_ptr<type> adobe_thread_local_storage_##signature##_t;           \
    adobe_thread_local_storage_##signature##_t* adobe_thread_local_storage_##signature##_g = 0;    \
    type& adobe_thread_local_storage_##signature##_access();                                       \
    type& adobe_thread_local_storage_##signature##_access() {                                      \
        type* result = adobe_thread_local_storage_##signature##_g->get();                          \
        if (result)                                                                                \
            return *result;                                                                        \
        result = new type();                                                                       \
        adobe_thread_local_storage_##signature##_g->reset(result);                                 \
        return *result;                                                                            \
    }                                                                                              \
    }

#define ADOBE_THREAD_LOCAL_STORAGE_INITIALIZE(signature)                                           \
    static adobe_thread_local_storage_##signature##_t adobe_thread_local_storage_##signature##_s;  \
    adobe_thread_local_storage_##signature##_g = &adobe_thread_local_storage_##signature##_s

#else

#define ADOBE_THREAD_LOCAL_STORAGE_1(type, signature, ctor_p1)                                     \
    type& adobe_thread_local_storage_##signature##_access();                                       \
    type& adobe_thread_local_storage_##signature##_access() {                                      \
        static type adobe_thread_local_storage_##signature##_s(ctor_p1);                           \
        return adobe_thread_local_storage_##signature##_s;                                         \
    }

#define ADOBE_THREAD_LOCAL_STORAGE(type, signature)                                                \
    type& adobe_thread_local_storage_##signature##_access();                                       \
    type& adobe_thread_local_storage_##signature##_access() {                                      \
        static type adobe_thread_local_storage_##signature##_s;                                    \
        return adobe_thread_local_storage_##signature##_s;                                         \
    }

#define ADOBE_THREAD_LOCAL_STORAGE_INITIALIZE(signature)

#endif

#endif

#define ADOBE_THREAD_LOCAL_STORAGE_ACCESS(signature)                                               \
    adobe_thread_local_storage_##signature##_access()

/**************************************************************************************************/

#endif // ADOBE_ONCE_HPP

/**************************************************************************************************/
