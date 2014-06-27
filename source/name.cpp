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

// asl
#include <adobe/implementation/string_pool.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

constexpr std::size_t empty_hash_s = adobe::detail::name_hash("");

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

static_name_t::operator bool() const { return static_cast<bool>(static_cast<name_t>(*this)); }

/****************************************************************************************************/

bool operator<(const static_name_t& x, const static_name_t& y) { return name_t(x) < name_t(y); }

/****************************************************************************************************/

name_t::operator bool() const { return ptr_m != detail::empty_string_s(); }

/****************************************************************************************************/

const char* name_t::map_string(const char* str) {
    if (!str || !*str)
        return map_string(detail::empty_string_s(), empty_hash_s);

    // Once fnv1a is in master we can make the hash faster
    // with a call to the sentinel variant.
    std::size_t hash(detail::name_hash(str, std::strlen(str)));

    return map_string(str, hash);
}

/****************************************************************************************************/

const char* name_t::map_string(const char* str, std::size_t hash) {
    typedef std::unordered_map<std::size_t, const char*> map_t;
    typedef std::lock_guard<std::mutex> lock_t;

    static std::mutex sync_s;

    lock_t lock(sync_s);

    static adobe::unique_string_pool_t pool_s;
    static map_t map_s;
    map_t::const_iterator found(map_s.find(hash));

    return found == map_s.end() ? map_s.emplace(hash, pool_s.add(str)).first->second
                                : found->second;
}

/****************************************************************************************************/

std::ostream& operator<<(std::ostream& s, const static_name_t& name) { return s << name.string_m; }

/****************************************************************************************************/

std::ostream& operator<<(std::ostream& s, const name_t& name) { return s << name.ptr_m; }

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/
