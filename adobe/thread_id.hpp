/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_THREAD_ID_HPP
#define ADOBE_THREAD_ID_HPP

#include <adobe/config.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

typedef const struct thread_id_opaque_t* thread_id_t;

thread_id_t thread_id();

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
