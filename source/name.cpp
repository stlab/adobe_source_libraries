/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

// identity
#include <adobe/name.hpp>

// stdc++
#include <iostream>
#include <unordered_map>
#include <mutex>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

typedef std::unordered_map<std::size_t, const char*> map_t;
typedef std::lock_guard<std::mutex>                  lock_t;

map_t& map()
{
    static map_t value_s;

    return value_s;
}

std::mutex& map_mutex()
{
    static std::mutex value_s;

    return value_s;
}

constexpr const char* empty_string_s = "";

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

static_name_t::operator bool() const
{
    return static_cast<bool>(static_cast<name_t>(*this));
}

/****************************************************************************************************/

bool operator<(const static_name_t& x, const static_name_t& y)
{
    return name_t(x) < name_t(y);
}

/****************************************************************************************************/

name_t::operator bool() const
{
    return ptr_m != empty_string_s;
}

/****************************************************************************************************/

const char* name_t::map_string(const char* str)
{
    if (!str || !*str)
        str = empty_string_s;

    // Once fnv1a is in master we can make the hash faster
    // with a call to the sentinel variant.
    std::size_t hash(detail::name_hash(str, std::strlen(str)));

    return map_string(str, hash);
}

/****************************************************************************************************/

const char* name_t::map_string(const char* str, std::size_t hash)
{
    lock_t lock(map_mutex());

    return map().emplace(hash, str).first->second;
}

/****************************************************************************************************/

std::ostream& operator<<(std::ostream& s, const static_name_t& name)
{
    return s
#if ADOBE_NAME_DEBUG
         << "[ '"
#endif
         << name.string_m
#if ADOBE_NAME_DEBUG
         << "', 0x" << std::hex << name.hash_m << std::dec << " ]"
#endif
        ;
}

/****************************************************************************************************/

std::ostream& operator<<(std::ostream& s, const name_t& name)
{
    return s
#if ADOBE_NAME_DEBUG
         << "[ '"
#endif
         << name.ptr_m
#if ADOBE_NAME_DEBUG
         << "', 0x" << std::hex << static_cast<const void*>(name.ptr_m) << std::dec << " ]"
#endif
        ;
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
