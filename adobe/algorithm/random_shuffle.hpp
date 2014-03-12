/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_RANDOM_SHUFFLE_HPP
#define ADOBE_ALGORITHM_RANDOM_SHUFFLE_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>
#include <functional>

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
inline void random_shuffle(RandomAccessRange& range) {
    return std::random_shuffle(boost::begin(range), boost::end(range));
}

/*!
    \ingroup random_shuffle

    \brief random_shuffle implementation
*/
template <class RandomAccessIterator, class RandomNumberGenerator>
inline void random_shuffle(RandomAccessIterator first, RandomAccessIterator last,
                           RandomNumberGenerator& rand) {
    return std::random_shuffle(first, last, std::bind(rand, std::placeholders::_1));
}

/*!
    \ingroup random_shuffle

    \brief random_shuffle implementation
*/
template <class RandomAccessRange, class RandomNumberGenerator>
inline void random_shuffle(RandomAccessRange& range, RandomNumberGenerator& rand) {
    return adobe::random_shuffle(boost::begin(range), boost::end(range), rand);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
