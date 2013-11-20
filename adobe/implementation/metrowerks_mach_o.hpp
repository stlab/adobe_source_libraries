/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#include <adobe/config.hpp>

/*
These are the switches required to build using the MSL C header for a Mach-O target.
*/

#pragma c99 on
#define _MSL_USING_MW_C_HEADERS 1

/*

These defines allow <Carbon/Carbon.h> to build

*/

#ifndef __NOEXTENSIONS__
#define __NOEXTENSIONS__
#endif
#ifndef __CF_USE_FRAMEWORK_INCLUDES__
#define __CF_USE_FRAMEWORK_INCLUDES__
#endif


/*************************************************************************************************/
