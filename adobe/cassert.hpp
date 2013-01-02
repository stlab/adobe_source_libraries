/*
    Copyright 2012 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

/*!
    \file
    
    The header \c <adobe/cassert.hpp> includes \c <cassert> and defines the macro \c ADOBE_ASSERT if 
    it is not already defined.
    
    The header may be included multiple times with differing definitions of either \c NDEBUG or
    \c ADOBE_ASSERT.
*/

#include <cassert>

/**************************************************************************************************/

/*!
    \def ADOBE_ASSERT(p)
    
    If undefined, \c ADOBE_ASSERT(p) is defined to be \c assert(p). \c ADOBE_ASSERT may be defined 
    by library clients to us an alternative \c assert() mechanism. The result of mapping 
    \c ADOBE_ASSERT to a function with semantics that differ from \c assert() are undefined.
*/

#ifndef ADOBE_ASSERT
#define ADOBE_ASSERT(p) assert(p)
#endif

/**************************************************************************************************/

