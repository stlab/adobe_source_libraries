/*
    Copyright 2005-2007 Adobe Systems Incorporated and others
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ZUID_HPP
#define ADOBE_ZUID_HPP

#include <adobe/config.hpp>

#include <boost/operators.hpp>
#include <boost/cstdint.hpp>

#include <string>

/*************************************************************************************************/

/*
    Relevant copyright information is provided below and may not be removed from this file. 

    Derived from the RSA Data Security, Inc. MD5 Message-Digest Algorithm.
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

/*
    MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm

    Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights 
    reserved.

    License to copy and use this software is granted provided that it is 
    identified as the "RSA Data Security, Inc. MD5 Message-Digest Algorithm" in 
    all material mentioning or referencing this software or this function. 

    License is also granted to make and use derivative works provided that such 
    works are identified as "derived from the RSA Data Security, Inc. MD5 
    Message-Digest Algorithm" in all material mentioning or referencing the 
    derived work. 
    
    RSA Data Security, Inc. makes no representations concerning either the 
    merchantability of this software or the suitability of this software for 
    any particular purpose. It is provided "as is" without express or implied 
    warranty of any kind. 
    
    These notices must be retained in any copies of any part of this 
    documentation and/or software. 
*/

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

struct uuid_t
{
#if !defined(ADOBE_NO_DOCUMENTATION)
    boost::uint32_t data1_m;
    boost::uint16_t data2_m;
    boost::uint16_t data3_m;
    boost::uint8_t  data4_m[8];
#endif
};

class zuid_t
#if !defined(ADOBE_NO_DOCUMENTATION)
    : private boost::totally_ordered<zuid_t>
#endif
{
public:
#if !defined(ADOBE_NO_DOCUMENTATION)
    zuid_t ();
    zuid_t (const zuid_t&);
#endif

    explicit zuid_t (const uuid_t&);
    explicit zuid_t (const std::string&);
    explicit zuid_t (const char*);

    zuid_t (const zuid_t& name_space, const std::string& name);

#if !defined(ADOBE_NO_DOCUMENTATION)
    zuid_t& operator = (const zuid_t&);
    zuid_t& operator = (const uuid_t&);
#endif

    std::string str () const;
    char*       c_str () const;

    static const zuid_t null;

#if !defined(ADOBE_NO_DOCUMENTATION)
    enum { string_size_k = 36 };

private:
    friend bool operator == (const zuid_t& a, const zuid_t& b);
    friend bool operator < (const zuid_t& a, const zuid_t& b);

    struct zeroed { };

    zuid_t(zeroed);

    uuid_t uuid_m;
#endif
};

/*************************************************************************************************/
#if !defined(ADOBE_NO_DOCUMENTATION)

bool operator == (const zuid_t& a, const zuid_t& b);
bool operator < (const zuid_t& a, const zuid_t& b);

#endif

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
    
#endif

/*************************************************************************************************/
