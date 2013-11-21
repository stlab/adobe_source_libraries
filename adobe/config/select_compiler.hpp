/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_CONFIG_SELECT_COMPILER_HPP
#define ADOBE_CONFIG_SELECT_COMPILER_HPP

/*************************************************************************************************/

#if defined __GNUC__
//  GNU C++: (including Darwin)
#include <adobe/config/compiler/gcc.hpp>

#elif defined _MSC_VER
//  Must remain the last #elif since some other vendors (Metrowerks, for
//  example) also #define _MSC_VER
#include <adobe/config/compiler/visualc.hpp>

#endif

/*************************************************************************************************/

#endif

/*************************************************************************************************/
