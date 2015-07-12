/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ARRAY_HPP
#define ADOBE_ARRAY_HPP

#include <adobe/config.hpp>

#include <adobe/array_fwd.hpp>

#include <adobe/any_regular.hpp>

#ifdef ADOBE_STD_SERIALIZATION
#include <adobe/iomanip.hpp>
#endif

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

template <typename T> // T models Regular
inline void push_back(array_t& v, T x) {
    v.push_back(any_regular_t(std::move(x)));
}

inline void push_back(array_t& v, any_regular_t x) { v.push_back(std::move(x)); }

/**************************************************************************************************/

#ifdef ADOBE_STD_SERIALIZATION

inline std::ostream& operator<<(std::ostream& out, const array_t& x) {
    out << begin_sequence;

    for (const auto& e : x)
        out << e; // format will be called by any_regular_t.

    out << end_sequence;

    return out;
}

#endif

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif
