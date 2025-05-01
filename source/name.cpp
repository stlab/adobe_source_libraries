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
#include <mutex>

// asl
#include <adobe/closed_hash.hpp>
#include <adobe/implementation/string_pool.hpp>

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

constexpr std::size_t empty_hash_s = adobe::detail::name_hash("");

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
        return map_string(detail::empty_string_s(), empty_hash_s);

    // Once fnv1a is in master we can make the hash faster
    // with a call to the sentinel variant.
    std::size_t hash(detail::name_hash(str, std::strlen(str)));

    return map_string(str, hash);
}

/**************************************************************************************************/

const char* name_t::map_string(const char* str, std::size_t hash) {
    using map_t = closed_hash_map<std::size_t, const char*>;
    using lock_t = std::scoped_lock<std::mutex>;

    static std::mutex sync_s;

    static adobe::unique_string_pool_t pool_s;
    static map_t map_s;

    // There may be an opportunity here to use a shared_mutex, but it's not clear that would be
    // faster than the current implementation. The idea would be to do a find under a shared_lock
    // and then a find / insert under an exclusive_lock if the key is not found. Unfortunately, I
    // don't see a way to try_upgrade a shared_lock to an exclusive_lock. Another approach would be:
    // 1. Try to aquire an exclusive_lock.
    //    if success - do the find/insert and return the result
    // 2. If failure, aquire a shared_lock and find the value
    //    if found - return the value
    // 3. If not found, aquire an exclusive_lock and do the find/insert and return the result.
    //
    // This would have to be coded multiple ways and profiled to see which is faster but since
    // use on thread is likely low, the current implementation is probably fine.

    lock_t lock(sync_s);

    map_t::const_iterator found(map_s.find(hash));

    return found == map_s.end()
               ? map_s.insert(map_t::value_type(hash, pool_s.add(str))).first->second
               : found->second;
}

/**************************************************************************************************/

std::ostream& operator<<(std::ostream& s, const static_name_t& name) { return s << name.string_m; }

/**************************************************************************************************/

std::ostream& operator<<(std::ostream& s, const name_t& name) { return s << name.ptr_m; }

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
