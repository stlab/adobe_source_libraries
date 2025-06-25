/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_DICTIONARY_HPP
#define ADOBE_DICTIONARY_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/dictionary_fwd.hpp>

#include <stdexcept>

#include <adobe/any_regular.hpp>
#include <adobe/closed_hash.hpp>
#include <adobe/name.hpp>
#include <adobe/string.hpp>

#ifdef ADOBE_STD_SERIALIZATION
#include <iosfwd>
#endif

/**************************************************************************************************/

namespace adobe {
inline namespace version_1 {

/**************************************************************************************************/

template <typename T>
inline bool get_value(const dictionary_t& dict, name_t key, T& value) {
    dictionary_t::const_iterator i = dict.find(key);
    if (i == dict.end())
        return false;
    return i->second.cast(value);
}

inline bool get_value(const dictionary_t& dict, name_t key, any_regular_t& value) {
    dictionary_t::const_iterator i = dict.find(key);
    if (i == dict.end())
        return false;
    value = i->second;
    return true;
}

inline const any_regular_t& get_value(const dictionary_t& dict, name_t key) {
    dictionary_t::const_iterator i = dict.find(key);
    if (i == dict.end())
        throw std::out_of_range(make_string("dictionary_t: key '", key.c_str(), "' not found"));

    return i->second;
}

/**************************************************************************************************/

#ifdef ADOBE_STD_SERIALIZATION

// NOTE (sparent@adobe.com) : Code for serialization is located in source/any_regular.cpp.
std::ostream& operator<<(std::ostream& out, const dictionary_t& x);

#endif

/**************************************************************************************************/

} // namespace version_1

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
