/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_PARTITION_HPP
#define ADOBE_ALGORITHM_PARTITION_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/bind.hpp>

#include <algorithm>

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
inline BidirectionalIterator
partition(BidirectionalIterator first, BidirectionalIterator last, Predicate pred)
{
    return std::partition(first, last, boost::bind(pred, _1));
}

/*!
    \ingroup partition

    \brief partition implementation
*/
template <class BidirectionalRange, class Predicate>
inline typename boost::range_iterator<BidirectionalRange>::type
partition(BidirectionalRange& range, Predicate pred)
{
    return adobe::partition(boost::begin(range), boost::end(range), pred);
}

/*!
    \ingroup partition

    \brief partition implementation
*/
template <class BidirectionalIterator, class Predicate>
inline BidirectionalIterator
stable_partition(BidirectionalIterator first, BidirectionalIterator last, Predicate pred)
{
    return std::stable_partition(first, last, boost::bind(pred, _1));
}

/*!
    \ingroup partition

    \brief partition implementation
*/
template <class BidirectionalRange, class Predicate>
inline typename boost::range_iterator<BidirectionalRange>::type
stable_partition(BidirectionalRange& range, Predicate pred)
{
    return adobe::stable_partition(boost::begin(range), boost::end(range), pred);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
