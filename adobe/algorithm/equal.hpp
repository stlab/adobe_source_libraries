/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_EQUAL_HPP
#define ADOBE_ALGORITHM_EQUAL_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>
#include <functional>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup equal equal
\ingroup non_mutating_algorithm

\see
    - STL documentation for \ref stldoc_equal
*/
/*************************************************************************************************/
/*!
    \ingroup equal
*/
template <class InputIterator1, class InputIterator2, class BinaryPredicate>
inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                  BinaryPredicate pred) {
    return std::equal(first1, last1, first2, std::bind(pred, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup equal
*/
template <class InputRange1, class InputIterator2>
inline bool equal(const InputRange1& range1, InputIterator2 first2) {
    return std::equal(boost::begin(range1), boost::end(range1), first2);
}

/*!
    \ingroup equal
*/
template <class InputRange1, class InputIterator2, class BinaryPredicate>
inline bool equal(const InputRange1& range1, InputIterator2 first2, BinaryPredicate pred) {
    return adobe::equal(boost::begin(range1), boost::end(range1), first2, pred);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
