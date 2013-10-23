/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_RANDOM_SHUFFLE_HPP
#define ADOBE_ALGORITHM_RANDOM_SHUFFLE_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/bind.hpp>

#include <algorithm>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup random_shuffle random_shuffle
\ingroup mutating_algorithm

\see
    - STL documentation for \ref stldoc_random_shuffle
*/
/*************************************************************************************************/
/*!
    \ingroup random_shuffle

    \brief random_shuffle implementation
*/
template <class RandomAccessRange>
inline void random_shuffle(RandomAccessRange& range)
{
    return std::random_shuffle(boost::begin(range), boost::end(range));
}

/*!
    \ingroup random_shuffle

    \brief random_shuffle implementation
*/
template <class RandomAccessIterator, class RandomNumberGenerator>
inline void
random_shuffle(RandomAccessIterator first, RandomAccessIterator last, RandomNumberGenerator& rand)
{
    return std::random_shuffle(first, last, boost::bind(rand, _1));
}

/*!
    \ingroup random_shuffle

    \brief random_shuffle implementation
*/
template <class RandomAccessRange, class RandomNumberGenerator>
inline void random_shuffle(RandomAccessRange& range, RandomNumberGenerator& rand)
{
    return adobe::random_shuffle(boost::begin(range), boost::end(range), rand);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
