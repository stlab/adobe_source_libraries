/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_SWAP_RANGES_HPP
#define ADOBE_ALGORITHM_SWAP_RANGES_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup swap_ranges swap_ranges
\ingroup mutating_algorithm

\see
    - STL documentation for \ref stldoc_swap_ranges
*/
/*************************************************************************************************/
/*!
    \ingroup swap_ranges

    \brief swap_ranges implementation
*/
template <class ForwardRange1, class ForwardIterator2>
inline ForwardIterator2 swap_ranges(ForwardRange1& range1, ForwardIterator2 first2) {
    return std::swap_ranges(boost::begin(range1), boost::end(range1), first2);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
