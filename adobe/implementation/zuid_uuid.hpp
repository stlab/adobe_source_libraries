/*
    Copyright 2005-2007 Adobe Systems Incorporated and others
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ZUID_UUID_HPP
#define ADOBE_ZUID_UUID_HPP

#include <adobe/config.hpp>

#include <adobe/zuid.hpp>

#include <boost/cstdint.hpp>

/*************************************************************************************************/

/*
    Relevant copyright information is provided below and may not be removed from this file. 
*/

/*************************************************************************************************/

/*
    Copyright (c) 1990 - 1993, 1996 Open Software Foundation, Inc.
    Copyright (c) 1989 by Hewlett-Packard Company, Palo Alto, Ca. &
    Digital Equipment Corporation, Maynard, Mass.
    Copyright (c) 1998 Microsoft.
  
    To anyone who acknowledges that this file is provided "AS IS" without any 
    express or implied warranty: permission to use, copy, modify, and 
    distribute this file for any purpose is hereby granted without fee, 
    provided that the above copyright notices and this notice appears in all 
    source code copies, and that none of the names of Open Software Foundation, 
    Inc., Hewlett-Packard Company, or Digital Equipment Corporation be used in 
    advertising or publicity pertaining to distribution of the software without 
    specific, written prior permission. Neither Open Software Foundation, Inc., 
    Hewlett-Packard Company, Microsoft, nor Digital Equipment Corporation makes 
    any representations about the suitability of this software for any purpose.
*/

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/


//REVISIT(mmarcus) Boost 1.35.0 seems to pull in an MS system header
//that #defines uuid_t. For now we #undef it here.

#ifdef uuid_t
#undef uuid_t  
#endif

/* uuid_create -- generate a UUID */
boost::int16_t uuid_create(uuid_t * uuid);

/* uuid_create_from_name -- create a UUID using a "name" from a "name space" */
void uuid_create_from_name( uuid_t*         uuid,           /*  resulting UUID */
                            uuid_t          nsid,           /*  UUID to serve as context, so identical
                                                                names from different name spaces generate
                                                                different UUIDs */
                            boost::uint8_t* name,           /*  the name from which to generate a UUID */
                            boost::uint16_t namelen);       /*  the length of the name */

/*
uuid_compare --  Compare two UUID's "lexically" and return
      -1   u1 is lexically before u2
       0   u1 is equal to u2
       1   u1 is lexically after u2

Note: lexical ordering is not temporal ordering!
*/
boost::int16_t uuid_compare(const uuid_t *u1, const uuid_t *u2);

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
