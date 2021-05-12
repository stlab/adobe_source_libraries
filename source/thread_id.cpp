/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#if 0

#include <adobe/once.hpp>
#include <adobe/thread_id.hpp>

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

ADOBE_ONCE_DECLARATION(thread_id_once)
ADOBE_THREAD_LOCAL_STORAGE(int, thread_id)

/**************************************************************************************************/

void init_thread_id_once()
{
    ADOBE_THREAD_LOCAL_STORAGE_INITIALIZE(thread_id);
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

thread_id_t thread_id()
{
    ADOBE_ONCE_INSTANCE(thread_id_once);

    return reinterpret_cast<thread_id_t>(&ADOBE_THREAD_LOCAL_STORAGE_ACCESS(thread_id));
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

ADOBE_ONCE_DEFINITION(thread_id_once, init_thread_id_once)

/**************************************************************************************************/

#endif
