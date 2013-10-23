/*
    Copyright 2005-2007 Adobe Systems Incorporated and others
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/implementation/zuid_uuid.hpp>
#include <adobe/implementation/zuid_sys_dep.hpp>
#include <cstring>

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

namespace {

/*************************************************************************************************/

/* make a UUID from the timestamp, clockseq, and node ID */

void format_uuid_v1(uuid_t* uuid, boost::uint16_t clock_seq, uuid_time_t timestamp, uuid_node_t node)
{
    // Construct a version 1 uuid with the information we've gathered plus a few constants.

    uuid->data1_m = (boost::uint32_t)(timestamp & 0xFFFFFFFF);
    uuid->data2_m = (boost::uint16_t)((timestamp >> 32) & 0xFFFF);
    uuid->data3_m = (boost::uint16_t)((timestamp >> 48) & 0x0FFF);
    uuid->data3_m |= (1 << 12);
    uuid->data4_m[1] = boost::uint8_t(clock_seq & 0xFF);
    uuid->data4_m[0] = boost::uint8_t((clock_seq & 0x3F00) >> 8);
    uuid->data4_m[0] |= 0x80;

    std::memcpy(&uuid->data4_m[2], &node, 6);
}
  
/*************************************************************************************************/

/* make a UUID from a (pseudo)random 128 bit number */

void format_uuid_v3(uuid_t* uuid, boost::uint8_t hash[16])
{
    /* Construct a version 3 uuid with the (pseudo-)random number
    * plus a few constants. */

    std::memcpy(uuid, hash, sizeof(uuid_t));

    /* convert UUID to local byte order */
    uuid->data1_m = net_to_host(uuid->data1_m);
    uuid->data2_m = net_to_host(uuid->data2_m);
    uuid->data3_m = net_to_host(uuid->data3_m);

    /* put in the variant and version bits */
    uuid->data3_m &= 0x0FFF;
    uuid->data3_m |= (3 << 12);
    uuid->data4_m[0] &= 0x3F;
    uuid->data4_m[0] |= 0x80;
}

/*************************************************************************************************/

/*
    get-current_time -- get time as 60 bit 100ns ticks since whenever.
    Compensate for the fact that real clock resolution is
    less than 100ns.

    NOTE (SRP) : This code has been modified for ZUIDs. The code used
    to throttle on the clock and be limited to UUIDS_PER_TICK. The new
    code will increment first and reset on the clock every UUIDS_PER_TICK.
*/

void get_current_time(uuid_time_t* timestamp)
{
    static uuid_time_t      time_last;
    static boost::uint16_t  uuids_this_tick(0);
    static bool             inited(false);

    if (!inited)
    {
        get_system_time(&time_last);
        inited = true;
    };

    if (uuids_this_tick < UUIDS_PER_TICK)
    {
        ++uuids_this_tick;
    }
    else
    {
        uuid_time_t time_now;

        get_system_time(&time_now);

        // if the clock is outrunning the counter

        if (time_now > uuid_time_t(time_last + uuids_this_tick))
        {
            // reset count of uuids gen'd with this clock reading
            uuids_this_tick = 0;
            time_last       = time_now;
        }
        else
        {
            ++uuids_this_tick;
        }
    }

    /* add the count of uuids to low order bits of the clock reading */
    *timestamp = time_last + uuids_this_tick;
}

/*************************************************************************************************/

} // namespace

/*************************************************************************************************/

/* uuid_create -- generator a UUID */
boost::int16_t uuid_create(uuid_t* uuid)
{
    uuid_time_t     timestamp;
    uuid_time_t     last_time;
    boost::uint16_t clockseq;
    uuid_node_t     node;
    uuid_node_t     last_node;
    boost::int16_t  f;

    /* get current time */
    get_current_time(&timestamp);

    /* get node ID */
    get_ieee_node_identifier(&node);

    /* get saved state from NV storage */
    f = read_state(&clockseq, &last_time, &last_node);

    /*
        if no NV state, or if clock went backwards, or node ID changed
        (e.g., net card swap) change clockseq
    */

    if (!f || node != last_node)
        clockseq = static_cast<boost::uint16_t>(true_random());
    else if (timestamp < last_time)
        clockseq++;

    /* stuff fields into the UUID */
    format_uuid_v1(uuid, clockseq, timestamp, node);

    /* save the state for next time */
    write_state(clockseq, timestamp, node);

    return boost::int16_t(1);
}

/*************************************************************************************************/

/* uuid_create_from_name -- create a UUID using a "name" from a "name space" */

void uuid_create_from_name( uuid_t*         uuid,
                            uuid_t          nsid,
                            boost::uint8_t* name,
                            boost::uint16_t namelen)
{
    uuid_t                  net_nsid;   /* context UUID in network byte order */

    /*
        put name space ID in network byte order so it hashes
        the same no matter what endian machine we're on
    */
    net_nsid = nsid;
    net_nsid.data1_m = host_to_net(net_nsid.data1_m);
    net_nsid.data2_m = host_to_net(net_nsid.data2_m);
    net_nsid.data3_m = host_to_net(net_nsid.data3_m);

    md5_t c;
    c.update(&net_nsid, sizeof(net_nsid));
    c.update(name, namelen);

    adobe::md5_t::digest_t hash(c.final());

    /* the hash is in network byte order at this point */
    format_uuid_v3(uuid, &hash[0]);
}

/*************************************************************************************************/

boost::int16_t uuid_compare(const uuid_t *u1, const uuid_t *u2)
{
    #define ADOBE_UUID_COMPARE_CHECK(f1, f2) if (f1 != f2) return (f1 < f2) ? boost::int16_t(-1) : boost::int16_t(1);

    ADOBE_UUID_COMPARE_CHECK(u1->data1_m, u2->data1_m);
    ADOBE_UUID_COMPARE_CHECK(u1->data2_m, u2->data2_m);
    ADOBE_UUID_COMPARE_CHECK(u1->data3_m, u2->data3_m);

    for (boost::int16_t i(0); i < 8; ++i)
    { ADOBE_UUID_COMPARE_CHECK(u1->data4_m[i], u2->data4_m[i]); }

    #undef ADOBE_UUID_COMPARE_CHECK

    return 0;
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
