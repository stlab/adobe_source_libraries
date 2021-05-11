/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_PERMUTATION_HPP
#define ADOBE_ALGORITHM_PERMUTATION_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>
#include <functional>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup permutation permutation
\ingroup sorting

\see
    - STL documentation for \ref stldoc_next_permutation
    - STL documentation for \ref stldoc_prev_permutation
*/
/**************************************************************************************************/
/*!
    \ingroup permutation

    \brief permutation implementation
*/
template <class BidirectionalRange>
inline bool next_permutation(BidirectionalRange& range) {
    return std::next_permutation(boost::begin(range), boost::end(range));
}

/*!
    \ingroup permutation

    \brief permutation implementation
*/
template <class BidirectionalIterator, class Compare>
inline bool next_permutation(BidirectionalIterator first, BidirectionalIterator last,
                             Compare comp) {
    return std::next_permutation(first, last,
                                 std::bind(comp, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup permutation

    \brief permutation implementation
*/
template <class BidirectionalRange, class Compare>
inline bool next_permutation(BidirectionalRange& range, Compare comp) {
    return adobe::next_permutation(boost::begin(range), boost::end(range), comp);
}

/*!
    \ingroup permutation

    \brief permutation implementation
*/
template <class BidirectionalRange>
inline bool prev_permutation(BidirectionalRange& range) {
    return std::prev_permutation(boost::begin(range), boost::end(range));
}

/*!
    \ingroup permutation

    \brief permutation implementation
*/
template <class BidirectionalIterator, class Compare>
inline bool prev_permutation(BidirectionalIterator first, BidirectionalIterator last,
                             Compare comp) {
    return std::prev_permutation(first, last,
                                 std::bind(comp, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup permutation

    \brief permutation implementation
*/
template <class BidirectionalRange, class Compare>
inline bool prev_permutation(BidirectionalRange& range, Compare comp) {
    return adobe::prev_permutation(boost::begin(range), boost::end(range), comp);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
