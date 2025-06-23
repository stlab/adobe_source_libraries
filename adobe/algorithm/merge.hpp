/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_MERGE_HPP
#define ADOBE_ALGORITHM_MERGE_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>
#include <functional>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup merge merge
\ingroup sorting

\see
    - [STL documentation for merge](https://www.boost.org/sgi/stl/merge.html)
*/
/**************************************************************************************************/
/*!
    \ingroup merge

    \brief merge implementation
*/
template <class InputRange1, class InputRange2, class OutputIterator>
inline OutputIterator merge(const InputRange1& range1, const InputRange2& range2,
                            OutputIterator result) {
    return std::merge(boost::begin(range1), boost::end(range1), boost::begin(range2),
                      boost::end(range2), result);
}

/*!
    \ingroup merge

    \brief merge implementation
*/
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
inline OutputIterator merge(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                            InputIterator2 last2, OutputIterator result, Compare comp) {
    return std::merge(first1, last1, first2, last2, result,
                      std::bind(comp, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup merge

    \brief merge implementation
*/
template <class InputRange1, class InputRange2, class OutputIterator, class Compare>
inline OutputIterator merge(const InputRange1& range1, const InputRange2& range2,
                            OutputIterator result, Compare comp) {
    return adobe::merge(boost::begin(range1), boost::end(range1), boost::begin(range2),
                        boost::end(range2), result, comp);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
