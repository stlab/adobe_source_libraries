/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_CONFIG_COMPILER_MSVC_HPP
#define ADOBE_CONFIG_COMPILER_MSVC_HPP

/*************************************************************************************************/

#ifndef ADOBE_CONFIG_HPP
#error                                                                                             \
    "This file is intended to be included by <adobe/config.hpp> -- please use that file directly."
#endif

/*************************************************************************************************/

#ifndef ADOBE_COMPILER_MSVC
#define ADOBE_COMPILER_MSVC 1
#endif

/*************************************************************************************************/

#ifndef ADOBE_TEST_MICROSOFT_NO_DEPRECATE
#define ADOBE_TEST_MICROSOFT_NO_DEPRECATE 1
#endif

#if ADOBE_TEST_MICROSOFT_NO_DEPRECATE
#if _MSC_VER >= 1400
/*
    The explanation for this check is explained at

     http://stlab.adobe.com/wiki/index.php/Troubleshooting
*/

#ifndef _CRT_SECURE_NO_DEPRECATE
#error                                                                                             \
    "Microsoft 'Safe Standard C Library' is not supported. See <http://stlab.adobe.com/wiki/index.php/Troubleshooting>"
#endif

#ifndef _SCL_SECURE_NO_DEPRECATE
#error                                                                                             \
    "Microsoft 'Safe Standard C++ Library' is not supported. See <http://stlab.adobe.com/wiki/index.php/Troubleshooting>"
#endif
#endif
#endif


/*************************************************************************************************/

#endif

/*************************************************************************************************/
