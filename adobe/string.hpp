/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_STRING_HPP
#define ADOBE_STRING_HPP

#include <adobe/config.hpp>

#include <cstring>
#include <functional>
#include <iterator>
#include <string>
#include <vector>

#if defined(ADOBE_STD_SERIALIZATION)
#include <iosfwd>
#endif

#include <adobe/string/to_string.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

inline std::string make_string(const char* a, const char* b) {
    std::string result;
    result.reserve(std::strlen(a) + std::strlen(b));
    result += a;
    result += b;
    return result;
}

/**************************************************************************************************/

inline std::string make_string(const char* a, const char* b, const char* c) {
    std::string result;
    result.reserve(std::strlen(a) + std::strlen(b) + std::strlen(b));
    result += a;
    result += b;
    result += c;
    return result;
}

/**************************************************************************************************/

//!\ingroup misc_functional
struct str_less_t {
    bool operator()(const char* x, const char* y) const { 
        while (*x && *x == *y) {
            ++x;
            ++y;
        }
        return *x < *y;
    }
};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/


#endif

/**************************************************************************************************/
