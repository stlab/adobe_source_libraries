/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

// identity
#include <adobe/name.hpp>

// stdc++
#include <iostream>

// asl
#include <adobe/implementation/string_pool.hpp>

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

constexpr std::size_t empty_hash_s = adobe::detail::name_hash("");

struct str_name_hash {
    std::size_t operator()(const char* str) const {
        return adobe::detail::name_hash(str, std::strlen(str));
    }
};

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

static_name_t::operator bool() const { return static_cast<bool>(static_cast<name_t>(*this)); }

/**************************************************************************************************/

bool operator<(const static_name_t& x, const static_name_t& y) { return name_t(x) < name_t(y); }

/**************************************************************************************************/

name_t::operator bool() const { return ptr_m != detail::empty_string_s(); }

/**************************************************************************************************/

const char* name_t::map_string(const char* str) {
    if (!str || !*str)
        return map_string(detail::empty_string_s(), empty_hash_s, true);

    // Revisit, fnv1a is in main but not constexpr???

    // Once fnv1a is in main we can make the hash faster
    // with a call to the sentinel variant.
    std::size_t hash(detail::name_hash(str, std::strlen(str)));

    return map_string(str, hash, false);
}

/**************************************************************************************************/

const char* name_t::map_string(const char* str, std::size_t hash, bool is_static) {
    static adobe::unique_string_pool_t pool_s;
    return pool_s.add(str, hash, is_static);
}

/**************************************************************************************************/

std::ostream& operator<<(std::ostream& s, const static_name_t& name) { return s << name.string_m; }

/**************************************************************************************************/

std::ostream& operator<<(std::ostream& s, const name_t& name) { return s << name.ptr_m; }

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
