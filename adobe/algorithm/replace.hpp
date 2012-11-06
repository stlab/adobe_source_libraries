/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_REPLACE_HPP
#define ADOBE_ALGORITHM_REPLACE_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/bind.hpp>

#include <algorithm>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup replace replace
\ingroup mutating_algorithm

\see
    - STL documentation for \ref stldoc_replace
    - STL documentation for \ref stldoc_replace_if
    - STL documentation for \ref stldoc_replace_copy
    - STL documentation for \ref stldoc_replace_copy_if
*/
/*************************************************************************************************/
/*!
    \ingroup replace

    \brief replace implementation
*/
template <class ForwardRange, class T>
inline void replace(ForwardRange& range, const T& old_value, const T& new_value)
{
    std::replace(boost::begin(range), boost::end(range), old_value, new_value);
}

/*!
    \ingroup replace

    \brief replace implementation
*/
template <class ForwardIterator, class Predicate, class T>
inline void
replace_if(ForwardIterator first, ForwardIterator last, Predicate pred, const T& new_value)
{
    std::replace_if(first, last, boost::bind(pred, _1), new_value);
}

/*!
    \ingroup replace

    \brief replace implementation
*/
template <class ForwardRange, class Predicate, class T>
inline void replace_if(ForwardRange& range, Predicate pred, const T& new_value)
{
    adobe::replace_if(boost::begin(range), boost::end(range), pred, new_value);
}

/*!
    \ingroup replace

    \brief replace implementation
*/
template <class ForwardRange, class OutputIterator, class T>
inline OutputIterator
replace_copy(ForwardRange& range, OutputIterator result, const T& old_value, const T& new_value)
{
    return std::replace_copy(boost::begin(range), boost::end(range), result, old_value, new_value);
}

/*!
    \ingroup replace

    \brief replace implementation
*/
template <class ForwardRange, class OutputIterator, class T>
inline OutputIterator
replace_copy(const ForwardRange& range, OutputIterator result,
             const T& old_value, const T& new_value)
{
    return std::replace_copy(boost::begin(range), boost::end(range), result, old_value, new_value);
}

/*!
    \ingroup replace

    \brief replace implementation
*/
template <class ForwardIterator, class OutputIterator, class Predicate, class T>
inline OutputIterator
replace_copy_if(ForwardIterator first, ForwardIterator last,
                OutputIterator result, Predicate pred, const T& new_value)
{
    return std::replace_copy_if(first, last, result, boost::bind(pred, _1), new_value);
}

/*!
    \ingroup replace

    \brief replace implementation
*/
template <class ForwardRange, class OutputIterator, class Predicate, class T>
inline OutputIterator
replace_copy_if(ForwardRange& range, OutputIterator result, Predicate pred, const T& new_value)
{
    return adobe::replace_copy_if(boost::begin(range), boost::end(range), result, pred, new_value);
}

/*!
    \ingroup replace

    \brief replace implementation
*/
template <class ForwardRange, class OutputIterator, class Predicate, class T>
inline OutputIterator
replace_copy_if(const ForwardRange& range, OutputIterator result,
                Predicate pred, const T& new_value)
{
    return adobe::replace_copy_if(boost::begin(range), boost::end(range), result, pred, new_value);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
