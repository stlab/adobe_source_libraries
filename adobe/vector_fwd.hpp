/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#error Deprecated
#ifndef ADOBE_VECTOR_FWD_HPP
#define ADOBE_VECTOR_FWD_HPP

/*************************************************************************************************/

#include <adobe/config.hpp>
#include <adobe/memory_fwd.hpp>

/*************************************************************************************************/

namespace adobe {
namespace version_1 {

/*************************************************************************************************/

template <typename T, typename A = capture_allocator<T>>
class vector;

/*************************************************************************************************/

} // namespace version_1

using version_1::vector;

} // namespace adobe

#endif

/*************************************************************************************************/
