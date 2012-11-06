/*
    Copyright 2005-2007 Adobe Systems Incorporated and others
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ZUID_SYS_DEP_HPP
#define ADOBE_ZUID_SYS_DEP_HPP

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

#include <adobe/config.hpp>

#include <adobe/md5.hpp>

#include <boost/cstdint.hpp>

#include <ctime>

/*
    set the following to the number of 100ns ticks of
    the actual resolution of your system's clock
*/
#define UUIDS_PER_TICK 10

/*
    Set the following to a call to acquire a system wide global lock
*/
#define STD_LOCK
#define STD_UNLOCK

#ifdef BOOST_NO_INT64_T
    #error "Your platform has no 64-bit integral type."
#endif

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

typedef boost::uint64_t         uuid_time_t;
typedef boost::array<char, 6>   uuid_node_t;

/*************************************************************************************************/

void get_ieee_node_identifier(uuid_node_t *node);
void get_system_time(uuid_time_t *uuid_time);

boost::uint64_t true_random();

/*************************************************************************************************/

// NOTE (SRP) : From the HP man pages for these functions they swap between
// host and network byte order (I think it is going little to big endian).
// Mac is big endian so just NOP them.

template <typename I> // where I models a BinaryInteger
inline I swab(I x);

template <> // where I models a BinaryInteger
inline boost::uint32_t swab<boost::uint32_t>(boost::uint32_t x)
{
return  (x << 24) |                 // (x & 0x000000FF) << 24
        ((x & 0x0000FF00) <<  8) |
        ((x & 0x00FF0000) >>  8) |
        (x >> 24);                  // (x & 0xFF000000) >> 24
}

template <> // where I models a BinaryInteger
inline boost::uint16_t swab<boost::uint16_t>(boost::uint16_t x)
{
    return static_cast<boost::uint16_t>(
            (x << 8) |  // (x & 0x00FF) << 8
            (x >> 8));  // (x & 0xFF00) >> 8
}

/*************************************************************************************************/

#if ADOBE_PLATFORM_MAC

// Any other platforms that are Big Endian should use this code as well

template <typename I> // where I models a BinaryInteger
inline I host_to_net(I host)
{ return host; }

template <typename I> // where I models a BinaryInteger
inline I net_to_host(I net)
{ return net; }

#else

template <typename I> // where I models a BinaryInteger
inline I host_to_net(I host)
{ return swab(host); }

template <typename I> // where I models a BinaryInteger
inline I net_to_host(I net)
{ return swab(net); }

#endif

/*************************************************************************************************/

boost::int16_t  read_state(boost::uint16_t* clockseq, uuid_time_t* timestamp, uuid_node_t* node);
void            write_state(boost::uint16_t clockseq, uuid_time_t timestamp, uuid_node_t node);

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
