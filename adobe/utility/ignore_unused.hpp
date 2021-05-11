/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_IGNORE_UNUSED_HPP
#define ADOBE_IGNORE_UNUSED_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

//! \ingroup testing
template <typename T0>
inline void ignore_unused(const T0&) {}

//! \ingroup testing
template <typename T0, typename T1>
inline void ignore_unused(const T0&, const T1&) {}

//! \ingroup testing
template <typename T0, typename T1, typename T2>
inline void ignore_unused(const T0&, const T1&, const T2&) {}

//! \ingroup testing
template <typename T0, typename T1, typename T2, typename T3>
inline void ignore_unused(const T0&, const T1&, const T2&, const T3&) {}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

// ADOBE_IGNORE_UNUSED_HPP
#endif

/**************************************************************************************************/
