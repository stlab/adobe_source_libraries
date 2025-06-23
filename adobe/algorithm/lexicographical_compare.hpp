/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP
#define ADOBE_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>
#include <functional>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup lexicographical_compare lexicographical_compare
\ingroup sorting

\see
    - [STL documentation for lexicographical_compare](https://www.boost.org/sgi/stl/lexicographical_compare.html)
*/
/**************************************************************************************************/
/*!
    \ingroup lexicographical_compare

    \brief lexicographical_compare implementation
*/
template <class InputRange1, class InputRange2>
inline bool lexicographical_compare(const InputRange1& range1, const InputRange2& range2) {
    return std::lexicographical_compare(boost::begin(range1), boost::end(range1),
                                        boost::begin(range2), boost::end(range2));
}

/*!
    \ingroup lexicographical_compare

    \brief lexicographical_compare implementation
*/
template <class InputIterator1, class InputIterator2, class Compare>
inline bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                                    InputIterator2 first2, InputIterator2 last2, Compare comp) {
    return std::lexicographical_compare(
        first1, last1, first2, last2,
        std::bind(comp, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup lexicographical_compare

    \brief lexicographical_compare implementation
*/
template <class InputRange1, class InputRange2, class Compare>
inline bool lexicographical_compare(const InputRange1& range1, const InputRange2& range2,
                                    Compare comp) {
    return adobe::lexicographical_compare(boost::begin(range1), boost::end(range1),
                                          boost::begin(range2), boost::end(range2), comp);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
