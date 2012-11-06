/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_OTHER_OF_HPP
#define ADOBE_ALGORITHM_OTHER_OF_HPP

#include <adobe/config.hpp>

#include <algorithm>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup other_of other_of
\ingroup non_mutating_algorithm

Given a pair and an EqualityComparable item, this will return the item that is not equal to the
item passed. In the case when neither item is equal to the item passed the results are
undefined.
*/
/*************************************************************************************************/
/*!
    \ingroup other_of

    \brief other_of implementation
*/
template <typename T, // T models EqualityComparable
          typename P, // P models pair<T, T>
          typename BinaryPredicate>
inline const T& other_of(const P& pair, const T& x, BinaryPredicate pred)
{
    return pred(pair.first, x) ? pair.second : pair.first;
}

/*!
    \ingroup other_of

    \brief other_of implementation
*/
template <typename T, // T models EqualityComparable
          typename P> // P models pair<T, T>
inline const T& other_of(const P& pair, const T& x)
{
    return other_of(pair, x, std::equal_to<T>());
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
