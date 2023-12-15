/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

#ifndef ADOBE_IMPLEMENTATION_ATOMIC_PRIMITIVES_HPP
#define ADOBE_IMPLEMENTATION_ATOMIC_PRIMITIVES_HPP

#include <adobe/config.hpp>

#include <atomic>
#include <cstddef>
#ifndef NDEBUG
#include <iostream>
#endif

#include <adobe/typeinfo.hpp>

/******************************************************************************/

namespace adobe {

/******************************************************************************/

namespace implementation {

/******************************************************************************/
template <typename T>
struct atomic {
    typedef T value_type;

    typedef std::atomic<T> type;
};

/******************************************************************************/

#ifndef NDEBUG
template <typename T>
std::ostream& operator<<(std::ostream& s, const typename atomic<T>::type& x) {
    return s << static_cast<T>(x);
}
#endif

/******************************************************************************/

typedef adobe::implementation::atomic<std::size_t> atomic_t;

static_assert(sizeof(atomic_t::type) == sizeof(atomic_t::value_type));

/******************************************************************************/

} // namespace implementation

/******************************************************************************/

} // namespace adobe

/******************************************************************************/
// ADOBE_IMPLEMENTATION_ATOMIC_PRIMITIVES_HPP
#endif

/******************************************************************************/
