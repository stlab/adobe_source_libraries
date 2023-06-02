/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_CSTDINT_HPP
#define ADOBE_CSTDINT_HPP

#include <adobe/config.hpp>

#include <cstdint>

/**************************************************************************************************/

namespace adobe {

/*!
\defgroup tr1 TR1 Components
\ingroup utility
@{
*/

/**************************************************************************************************/

[[deprecated]] using std::intptr_t;
[[deprecated]] using std::uintptr_t;

//! @}
/**************************************************************************************************/
} // namespace adobe

/**************************************************************************************************/

#endif
