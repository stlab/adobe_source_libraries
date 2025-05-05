/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_STRING_POOL_HPP
#define ADOBE_STRING_POOL_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <cstddef>

#include <boost/noncopyable.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

class unique_string_pool_t : boost::noncopyable {
public:
    unique_string_pool_t();

    ~unique_string_pool_t();

    const char* add(const char* str);
    const char* add(const char* str, std::size_t hash, bool is_static);

private:
    struct implementation_t;

    implementation_t* object_m;
};

/**************************************************************************************************/

namespace detail {

/**************************************************************************************************/

constexpr const char* empty_string_s() { return ""; }

/**************************************************************************************************/

} // namespace detail

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
