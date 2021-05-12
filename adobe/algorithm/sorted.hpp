/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_SORTED_HPP
#define ADOBE_ALGORITHM_SORTED_HPP

#include <algorithm>
#include <functional>
#include <iterator>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <adobe/functional/operator.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup sorted sorted
\ingroup sorting
*/

/*!
\ingroup sorted
*/
template <typename I, // I models InputIterator
          typename O>
// O models StrictWeakOrdering on value_type(I)
I sorted(I f, I l, O o) {

    f = std::adjacent_find(f, l,
                           std::bind(std::logical_not<bool>(),
                                     std::bind(o, std::placeholders::_1, std::placeholders::_2)));
    if (f != l)
        ++f;
    return f;
}

/**************************************************************************************************/

/*!
\ingroup sorted
*/
template <typename I> // I models InputIterator, value_type(I) models LessThanComparable
I sorted(I f, I l) {
    return sorted(f, l, less());
}

/**************************************************************************************************/

/*!
\ingroup sorted
*/
template <typename I, // I models InputIterator
          typename O>
// O models StrictWeakOrdering on value_type(I)
inline bool is_sorted(I f, I l, O o) {
    return std::adjacent_find(
               f, l,
               std::bind(std::logical_not<bool>(),
                         std::bind(o, std::placeholders::_1, std::placeholders::_2))) == l;
}

/**************************************************************************************************/

/*!
\ingroup sorted
*/
template <typename I> // I models InputIterator, value_type(I) models LessThanComparable
inline bool is_sorted(I f, I l) {
    return is_sorted(f, l, less());
}

/**************************************************************************************************/

/*!
\ingroup sorted
*/
template <typename I, // I models ForwardIterator
          typename C, // C models StrictWeakOrdering(T, T)
          typename P>
// P models UnaryFunction(value_type(I)) -> T
inline bool is_sorted(I f, I l, C c, P p) {
    return std::adjacent_find(f, l,
                              std::bind(std::logical_not<bool>(),
                                        std::bind(c, std::bind(p, std::placeholders::_1),
                                                  std::bind(p, std::placeholders::_2)))) == l;
}

/**************************************************************************************************/

/*!
\ingroup sorted
*/
template <typename I, // I models ForwardRange
          typename C, // C models StrictWeakOrdering(T, T)
          typename P>
// P models UnaryFunction(value_type(I)) -> T
inline bool is_sorted(const I& r, C c, P p) {
    return is_sorted(boost::begin(r), boost::end(r), c, p);
}

/**************************************************************************************************/

/*!
\ingroup sorted
*/
template <typename I, // I models ForwardRange
          typename C>
// C models StrictWeakOrdering(T, T)
inline bool is_sorted(const I& r, C c) {
    return is_sorted(boost::begin(r), boost::end(r), c,
                     identity<typename std::iterator_traits<I>::value_type>());
}

/**************************************************************************************************/

/*!
\ingroup sorted
*/
template <typename I> // I models ForwardRange
inline bool is_sorted(const I& r) {
    return is_sorted(boost::begin(r), boost::end(r), less());
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
