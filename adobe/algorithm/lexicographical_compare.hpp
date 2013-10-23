/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP
#define ADOBE_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/bind.hpp>

#include <algorithm>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup lexicographical_compare lexicographical_compare
\ingroup sorting

\see
    - STL documentation for \ref stldoc_lexicographical_compare
*/
/*************************************************************************************************/
/*!
    \ingroup lexicographical_compare

    \brief lexicographical_compare implementation
*/
template <class InputRange1, class InputRange2>
inline bool lexicographical_compare(const InputRange1& range1, const InputRange2& range2)
{
    return std::lexicographical_compare(boost::begin(range1), boost::end(range1),
                                        boost::begin(range2), boost::end(range2));
}

/*!
    \ingroup lexicographical_compare

    \brief lexicographical_compare implementation
*/
template <class InputIterator1, class InputIterator2, class Compare>
inline bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                                    InputIterator2 first2, InputIterator2 last2,
                                    Compare comp)

{
    return std::lexicographical_compare(first1, last1, first2, last2, boost::bind(comp, _1, _2));
}

/*!
    \ingroup lexicographical_compare

    \brief lexicographical_compare implementation
*/
template <class InputRange1, class InputRange2, class Compare>
inline bool
lexicographical_compare(const InputRange1& range1, const InputRange2& range2, Compare comp)
{
    return adobe::lexicographical_compare(  boost::begin(range1), boost::end(range1),
                                                boost::begin(range2), boost::end(range2),
                                                comp);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
