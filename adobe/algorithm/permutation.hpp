/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_PERMUTATION_HPP
#define ADOBE_ALGORITHM_PERMUTATION_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/bind.hpp>

#include <algorithm>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup permutation permutation
\ingroup sorting

\see
    - STL documentation for \ref stldoc_next_permutation
    - STL documentation for \ref stldoc_prev_permutation
*/
/*************************************************************************************************/
/*!
    \ingroup permutation

    \brief permutation implementation
*/
template <class BidirectionalRange>
inline bool next_permutation(BidirectionalRange& range)
{
    return std::next_permutation(boost::begin(range), boost::end(range));
}

/*!
    \ingroup permutation

    \brief permutation implementation
*/
template <class BidirectionalIterator, class Compare>
inline bool next_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare comp)
{
    return std::next_permutation(first, last, boost::bind(comp, _1, _2));
}

/*!
    \ingroup permutation

    \brief permutation implementation
*/
template <class BidirectionalRange, class Compare>
inline bool next_permutation(BidirectionalRange& range, Compare comp)
{
    return adobe::next_permutation(boost::begin(range), boost::end(range), comp);
}

/*!
    \ingroup permutation

    \brief permutation implementation
*/
template <class BidirectionalRange>
inline bool prev_permutation(BidirectionalRange& range)
{
    return std::prev_permutation(boost::begin(range), boost::end(range));
}

/*!
    \ingroup permutation

    \brief permutation implementation
*/
template <class BidirectionalIterator, class Compare>
inline bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last, Compare comp)
{
    return std::prev_permutation(first, last, boost::bind(comp, _1, _2));
}

/*!
    \ingroup permutation

    \brief permutation implementation
*/
template <class BidirectionalRange, class Compare>
inline bool prev_permutation(BidirectionalRange& range, Compare comp)
{
    return adobe::prev_permutation(boost::begin(range), boost::end(range), comp);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
