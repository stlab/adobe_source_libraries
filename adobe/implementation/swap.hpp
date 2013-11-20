/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_SWAP_HPP
#define ADOBE_SWAP_HPP

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <algorithm>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup swap swap
\ingroup algorithm

We pull swap in from \c std, with a using std::swap declaration,
and employ the idiom that all code in namespace adobe that calls swap
should call it unqualified. This is less cluttered than employing local using declaration and is
forward compatible with C++ '0x constrained swap. We recommend this idiom to clients.
*/
/*************************************************************************************************/
/*!
\ingroup swap


*/
using std::swap;

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
