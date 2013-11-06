/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
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
namespace version_1 {

/**************************************************************************************************/

template <typename T> // T models Regular
inline void push_back(array_t &v, T x) {
    v.push_back(any_regular_t(std::move(x)));
}

inline void push_back(array_t &v, any_regular_t x) { v.push_back(std::move(x)); }

/**************************************************************************************************/

} // namespace version_1

using version_1::push_back;

/**************************************************************************************************/

#ifdef ADOBE_STD_SERIALIZATION

inline std::ostream &operator<<(std::ostream &out, const array_t &x) {
    out << begin_sequence;

    for (const auto &e : x)
        out << format(e);

    out << end_sequence;

    return out;
}

#endif

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif
