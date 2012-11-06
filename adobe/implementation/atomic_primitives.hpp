/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

#ifndef ADOBE_IMPLEMENTATION_ATOMIC_PRIMITIVES_HPP
#define ADOBE_IMPLEMENTATION_ATOMIC_PRIMITIVES_HPP

#include <adobe/config.hpp>

#include <cstddef>
#ifndef NDEBUG
    #include <iostream>
#endif

#if defined(BOOST_HAS_THREADS)
    #include <tbb/atomic.h>
#endif

#include <boost/static_assert.hpp>

#include <adobe/typeinfo.hpp>

/******************************************************************************/

namespace adobe {

/******************************************************************************/

namespace implementation {

/******************************************************************************/
template <typename T>
struct atomic
{
    typedef T value_type;

#if defined(BOOST_HAS_THREADS)
    typedef tbb::atomic<T> type;
#else
    typedef T type;
#endif
};

/******************************************************************************/

#ifndef NDEBUG
template <typename T>
std::ostream& operator<<(std::ostream& s, const typename atomic<T>::type& x)
{
    return s << static_cast<T>(x);
}
#endif

/******************************************************************************/

typedef adobe::implementation::atomic<std::size_t> atomic_t;

BOOST_STATIC_ASSERT((sizeof(atomic_t::type) ==
                     sizeof(atomic_t::value_type)));

/******************************************************************************/

} // namespace implementation

/******************************************************************************/

} // namespace adobe

/******************************************************************************/
// ADOBE_IMPLEMENTATION_ATOMIC_PRIMITIVES_HPP
#endif

/******************************************************************************/
