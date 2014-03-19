/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_PARTITION_HPP
#define ADOBE_ALGORITHM_PARTITION_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>
#include <functional>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup partition partition
\ingroup mutating_algorithm

\see
    - STL documentation for \ref stldoc_partition
    - STL documentation for \ref stldoc_stable_partition
*/
/*************************************************************************************************/
/*!
    \ingroup partition

    \brief partition implementation
*/
template <class BidirectionalIterator, class Predicate>
inline BidirectionalIterator partition(BidirectionalIterator first, BidirectionalIterator last,
                                       Predicate pred) {
    return std::partition(first, last, std::bind(pred, std::placeholders::_1));
}

/*!
    \ingroup partition

    \brief partition implementation
*/
template <class BidirectionalRange, class Predicate>
inline typename boost::range_iterator<BidirectionalRange>::type partition(BidirectionalRange& range,
                                                                          Predicate pred) {
    return adobe::partition(boost::begin(range), boost::end(range), pred);
}

/*!
    \ingroup partition

    \brief partition implementation
*/
template <class BidirectionalIterator, class Predicate>
inline BidirectionalIterator stable_partition(BidirectionalIterator first,
                                              BidirectionalIterator last, Predicate pred) {
    return std::stable_partition(first, last, std::bind(pred, std::placeholders::_1));
}

/*!
    \ingroup partition

    \brief partition implementation
*/
template <class BidirectionalRange, class Predicate>
inline typename boost::range_iterator<BidirectionalRange>::type
stable_partition(BidirectionalRange& range, Predicate pred) {
    return adobe::stable_partition(boost::begin(range), boost::end(range), pred);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
