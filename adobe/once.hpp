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


#define ADOBE_THREAD_SAFE

/**************************************************************************************************/

#define ADOBE_THREAD_LOCAL_STORAGE_1(type, signature, ctor_p1)                                     \
    type& adobe_thread_local_storage_##signature##_access() {                                      \
        thread_local type holder{ctor_p1};                                                         \
        return holder;                                                                             \
    }

#define ADOBE_THREAD_LOCAL_STORAGE(type, signature)                                                \
    type& adobe_thread_local_storage_##signature##_access() {                                      \
        thread_local type holder;                                                                  \
        return holder;                                                                             \
    }

#define ADOBE_THREAD_LOCAL_STORAGE_INITIALIZE(signature)

#define ADOBE_THREAD_LOCAL_STORAGE_ACCESS(signature)                                               \
    adobe_thread_local_storage_##signature##_access()

/**************************************************************************************************/

#endif // ADOBE_ONCE_HPP

/**************************************************************************************************/
