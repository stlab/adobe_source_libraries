/*
    Copyright 2005-2007 Adobe Systems Incorporated and others
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>
#include <adobe/zuid.hpp>
#include <adobe/once.hpp>
#include <adobe/implementation/zuid_uuid.hpp>
#include <adobe/implementation/zuid_sys_dep.hpp>

#include <boost/array.hpp>
#include <boost/cstdint.hpp>

#include <string>
#include <cstdio>
#include <memory>
#include <vector>

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

ADOBE_ONCE_DECLARATION(zuid_once)

/*************************************************************************************************/

#if defined(BOOST_MSVC) && BOOST_MSVC >= 1400
    // Silence warnings about deprecated cstdio functions (sprintf,sscanf).

    #pragma warning ( disable : 4996 )
#endif

/*************************************************************************************************/

namespace {

/*************************************************************************************************/

typedef boost::array<char, adobe::zuid_t::string_size_k + 1> zuid_char_buffer_t;

ADOBE_THREAD_LOCAL_STORAGE(zuid_char_buffer_t, zuid_char_buffer)

/*************************************************************************************************/

void init_zuid_once()
{
    // initialize the thread-specific data

    ADOBE_THREAD_LOCAL_STORAGE_INITIALIZE(zuid_char_buffer);
}

/*************************************************************************************************/

void format_uuid(zuid_char_buffer_t& buffer, const adobe::uuid_t& uuid)
{
#if ADOBE_PLATFORM_CYGWIN
    typedef unsigned int tmp_t;
#else
    typedef boost::uint32_t tmp_t;
#endif

    tmp_t temp_data1(uuid.data1_m);

    std::sprintf(&buffer[0], "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                 temp_data1, uuid.data2_m, uuid.data3_m, uuid.data4_m[0], uuid.data4_m[1],
                 uuid.data4_m[2], uuid.data4_m[3], uuid.data4_m[4], uuid.data4_m[5],
                 uuid.data4_m[6], uuid.data4_m[7]);
}

/*************************************************************************************************/

const adobe::uuid_t& empty_uuid()
{
    static adobe::uuid_t uuid_s = { 0 };

    return uuid_s;
}

/*************************************************************************************************/

} // namespace

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
#if !defined(ADOBE_NO_DOCUMENTATION)

zuid_t::zuid_t () :
    uuid_m(empty_uuid())
    { uuid_create(&uuid_m); }

/*************************************************************************************************/

zuid_t::zuid_t (const zuid_t& zuid_t) :
    uuid_m(zuid_t.uuid_m)
    { }

#endif
/*************************************************************************************************/
    
zuid_t::zuid_t (const uuid_t &uuid) :
    uuid_m(uuid)
    { }

/*************************************************************************************************/
    
zuid_t::zuid_t (const std::string& zuid) :
    uuid_m(empty_uuid())
{
    /*
    Initially this constructor was written using streams. It was overly
    complicated because the zuid_t is formatted well to be read directly with an
    istream. It was rewritten to ensure it wouldn't fail.
    */

    char buffer[zuid_t::string_size_k + 1] = { 0 }; // Buffer large enough for a null terminated zuid_t

    zuid.copy(buffer, zuid_t::string_size_k);

    uuid_m = zuid_t(&buffer[0]).uuid_m;
}
    
/*************************************************************************************************/

zuid_t::zuid_t (const char* zuid_t) :
    uuid_m(empty_uuid())
{
    unsigned long temp_data1;
    int           temp_data[8];

    std::sscanf(zuid_t, "%8lx-%4hx-%4hx-%2x%2x-%2x%2x%2x%2x%2x%2x",
                &temp_data1, &uuid_m.data2_m, &uuid_m.data3_m, &temp_data[0], &temp_data[1],
                &temp_data[2], &temp_data[3], &temp_data[4], &temp_data[5],
                &temp_data[6], &temp_data[7]);

    uuid_m.data1_m = temp_data1;

    for (int i = 0; i < 8; ++i)
        uuid_m.data4_m[i] = static_cast<boost::uint8_t>(temp_data[i]);
}

/*************************************************************************************************/

zuid_t::zuid_t(const zuid_t& name_space, const std::string& name) :
    uuid_m(empty_uuid())
{
    uuid_create_from_name(  &uuid_m,
                            name_space.uuid_m,
                            const_cast<boost::uint8_t*>(reinterpret_cast<const boost::uint8_t*>(name.data())),
                            static_cast<boost::uint16_t>(name.length()));
}

/*************************************************************************************************/
#if !defined(ADOBE_NO_DOCUMENTATION)

zuid_t::zuid_t(zeroed) :
    uuid_m(empty_uuid())
{ }

/*************************************************************************************************/

zuid_t& zuid_t::operator = (const zuid_t& zuid_t)
{
    uuid_m = zuid_t.uuid_m;

    return *this;
}

/*************************************************************************************************/

zuid_t& zuid_t::operator = (const uuid_t &uuid)
{
    uuid_m = uuid;

    return *this;
}

#endif
/*************************************************************************************************/

std::string zuid_t::str() const
{
    zuid_char_buffer_t buffer = {{ 0 }};

    format_uuid(buffer, uuid_m);

    buffer[buffer.size() - 1] = 0;

    return std::string(&buffer[0]);
}

/*************************************************************************************************/

char* zuid_t::c_str() const
{
    ADOBE_ONCE_INSTANCE(zuid_once);

    zuid_char_buffer_t& buffer(ADOBE_THREAD_LOCAL_STORAGE_ACCESS(zuid_char_buffer));

    format_uuid(buffer, uuid_m);

    buffer[buffer.size() - 1] = 0;

    return &buffer[0];
}

/*************************************************************************************************/

bool operator == (const zuid_t& a, const zuid_t& b)
{
    return uuid_compare(&a.uuid_m, &b.uuid_m) == 0;
}

/*************************************************************************************************/

bool operator < (const zuid_t& a, const zuid_t& b)
{
    return uuid_compare(&a.uuid_m, &b.uuid_m) == -1;
}

/*************************************************************************************************/

const zuid_t zuid_t::null = zuid_t(zeroed());

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

ADOBE_ONCE_DEFINITION(zuid_once, init_zuid_once)

/*************************************************************************************************/
    
