/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/thread_id.hpp>
#include <adobe/once.hpp>

/*************************************************************************************************/

namespace {

/*************************************************************************************************/

ADOBE_ONCE_DECLARATION(thread_id_once)
ADOBE_THREAD_LOCAL_STORAGE(int, thread_id)

/*************************************************************************************************/

void init_thread_id_once()
{
    ADOBE_THREAD_LOCAL_STORAGE_INITIALIZE(thread_id);
}

/*************************************************************************************************/

} // namespace

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

thread_id_t thread_id()
{
    ADOBE_ONCE_INSTANCE(thread_id_once);

    return reinterpret_cast<thread_id_t>(&ADOBE_THREAD_LOCAL_STORAGE_ACCESS(thread_id));
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

ADOBE_ONCE_DEFINITION(thread_id_once, init_thread_id_once)

/*************************************************************************************************/
