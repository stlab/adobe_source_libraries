/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_ARRAY_FWD_HPP
#define ADOBE_ARRAY_FWD_HPP

#include <adobe/config.hpp>

#include <vector>

#include <adobe/any_regular_fwd.hpp>

/*************************************************************************************************/

namespace adobe {
namespace version_1 {

/*************************************************************************************************/

//!\ingroup abi_container
typedef std::vector<any_regular_t> array_t;

/*************************************************************************************************/

} // namespace version_1

using version_1::array_t;

#if defined(ADOBE_NO_DOCUMENTATION)
/* REVISIT (mmarcus) : doxygen doesn't seem to understand using
declarartions.  This is a doxygen only hack to make the typedef appear
in the documentation
*/
//!\ingroup abi_container
typedef version_1::vector<any_regular_t> array_t;
#endif

} // namespace adobe

/*************************************************************************************************/

#endif
