/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_EMPTY_HPP
#define ADOBE_EMPTY_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <boost/mpl/bool.hpp>
#include <boost/operators.hpp>
#include <boost/type_traits/has_trivial_constructor.hpp>
#include <boost/type_traits/has_trivial_destructor.hpp>
#include <boost/type_traits/is_empty.hpp>
#include <boost/type_traits/is_pod.hpp>

#if defined(ADOBE_STD_SERIALIZATION)
#include <iosfwd>
#endif

/**************************************************************************************************/

namespace adobe {
namespace version_1 {

/**************************************************************************************************/

/*!
\ingroup abi_misc

\model_of
    - \ref concept_regular_type
    - \ref stldoc_LessThanComparable

\brief An empty regular- and less-than-comparable- type.

\rationale
    empty_t is useful for default values. For example, value_t() will construct a value_t
containing
an empty_t by default.
*/

struct empty_t : private boost::totally_ordered<empty_t> {
    friend inline bool operator==(const empty_t&, const empty_t&) { return true; }
    friend inline bool operator<(const empty_t&, const empty_t&) { return false; }
    friend inline void swap(empty_t&, empty_t&) {}
};

#if defined(ADOBE_STD_SERIALIZATION)
//!\ingroup abi_misc
std::ostream& operator<<(std::ostream& stream, const empty_t&);
#endif

/**************************************************************************************************/

} // namespace version_1

using version_1::empty_t;

/**************************************************************************************************/

/*!
    empty_base is used to provide a unique empty base class when base class chaining is used to
    reduce object size overhead such as with Boost operators in Boost 1.31.0
*/

//!\ingroup util_misc
template <typename Derived>
struct empty_base {};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

namespace boost {

template <>
struct is_pod<adobe::version_1::empty_t> : boost::mpl::true_ {};
template <>
struct is_empty<adobe::version_1::empty_t> : boost::mpl::true_ {};
template <>
struct has_trivial_constructor<adobe::version_1::empty_t> : boost::mpl::true_ {};
template <>
struct has_trivial_destructor<adobe::version_1::empty_t> : boost::mpl::true_ {};
} // namespace boost

/**************************************************************************************************/

#endif

/**************************************************************************************************/
