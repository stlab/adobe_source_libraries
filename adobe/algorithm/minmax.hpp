/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_MINMAX_HPP
#define ADOBE_ALGORITHM_MINMAX_HPP

#include <adobe/config.hpp>

#include <algorithm>
#include <functional>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <adobe/algorithm/select.hpp>

#include <adobe/functional/operator.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup minmax min / max
\ingroup sorting

\note adobe::max differs is stable with respect to sort order where std::max is not. That is:

<code>
int a = 1;
int b = 1;

assert(&adobe::max(a, b) == b);
assert(&std::max(a, b) == a);
</code>

adobe::min and adobe::max also implement non-const forms which are not available in the standard
which allow the result to be used as an l-value.

<code>
int a = 1;
int b = 2;

adobe::min(a, b) = 10;

assert(a == 10);
</code>

\see
    - STL documentation for \ref stldoc_min
    - STL documentation for \ref stldoc_max
    - STL documentation for \ref stldoc_min_element
    - STL documentation for \ref stldoc_max_element
*/
/*************************************************************************************************/

/*!
    \ingroup minmax

    \brief minmax implementation
*/

template <class T, class C>
inline const T&(min)(const T& a, const T& b, C c) {
    return select_0_2(a, b, std::bind(c, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/

template <class T, class C, class P>
inline const T&(min)(const T& a, const T& b, C c, P p) {
    return select_0_2(a, b, std::bind(c, std::bind(p, std::placeholders::_1),
                                     std::bind(p, std::placeholders::_2)));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/

template <class T, class C>
inline T&(min)(T& a, T& b, C c) {
    return select_0_2(a, b, std::bind(c, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/

template <class T, class C, class P>
inline T&(min)(T& a, T& b, C c, P p) {
    return select_0_2(a, b, std::bind(c, std::bind(p, std::placeholders::_1),
                                     std::bind(p, std::placeholders::_2)));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/

template <class T>
inline const T&(min)(const T& a, const T& b) {
    return select_0_2(a, b, less());
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/

template <class T>
inline T&(min)(T& a, T& b) {
    return select_0_2(a, b, less());
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/

template <class T, class C>
inline const T&(max)(const T& a, const T& b, C c) {
    return select_1_2(a, b, std::bind(c, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/


template <class T, class C, class P>
inline const T&(max)(const T& a, const T& b, C c, P p) {
    return select_1_2(a, b, std::bind(c, std::bind(p, std::placeholders::_1),
                                     std::bind(p, std::placeholders::_2)));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/

template <class T, class C>
inline T&(max)(T& a, T& b, C c) {
    return select_1_2(a, b, std::bind(c, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/

template <class T, class C, class P>
inline T&(max)(T& a, T& b, C c, P p) {
    return select_1_2(a, b, std::bind(c, std::bind(p, std::placeholders::_1),
                                     std::bind(p, std::placeholders::_2)));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/


template <class T>
inline const T&(max)(const T& a, const T& b) {
    return select_1_2(a, b, less());
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/

template <class T>
inline T&(max)(T& a, T& b) {
    return select_1_2(a, b, less());
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange>
inline typename boost::range_iterator<ForwardRange>::type min_element(ForwardRange& range) {
    return std::min_element(boost::begin(range), boost::end(range));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange>
inline typename boost::range_const_iterator<ForwardRange>::type
min_element(const ForwardRange& range) {
    return std::min_element(boost::begin(range), boost::end(range));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardIterator, class C>
inline ForwardIterator min_element(ForwardIterator first, ForwardIterator last, C c) {
    return std::min_element(first, last, std::bind(c, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardIterator, class C, class P>
inline ForwardIterator min_element(ForwardIterator first, ForwardIterator last, C c, P p) {
    return std::min_element(first, last, std::bind(c, std::bind(p, std::placeholders::_1),
                                     std::bind(p, std::placeholders::_2)));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange, class C>
inline typename boost::range_iterator<ForwardRange>::type min_element(ForwardRange& range, C c) {
    return adobe::min_element(boost::begin(range), boost::end(range), c);
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange, class C, class P>
inline typename boost::range_iterator<ForwardRange>::type min_element(ForwardRange& range, C c, P p) {
    return adobe::min_element(boost::begin(range), boost::end(range), c, p);
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange, class C>
inline typename boost::range_const_iterator<ForwardRange>::type
min_element(const ForwardRange& range, C c) {
    return adobe::min_element(boost::begin(range), boost::end(range), c);
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange, class C, class P>
inline typename boost::range_const_iterator<ForwardRange>::type
min_element(const ForwardRange& range, C c, P p) {
    return adobe::min_element(boost::begin(range), boost::end(range), c, p);
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange>
inline typename boost::range_iterator<ForwardRange>::type max_element(ForwardRange& range) {
    return std::max_element(boost::begin(range), boost::end(range));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange>
inline typename boost::range_const_iterator<ForwardRange>::type
max_element(const ForwardRange& range) {
    return std::max_element(boost::begin(range), boost::end(range));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardIterator, class C>
inline ForwardIterator max_element(ForwardIterator first, ForwardIterator last, C c) {
    return std::max_element(first, last, std::bind(c, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardIterator, class C, class P>
inline ForwardIterator max_element(ForwardIterator first, ForwardIterator last, C c, P p) {
    return std::max_element(first, last, std::bind(c, std::bind(p, std::placeholders::_1),
                                     std::bind(p, std::placeholders::_2)));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange, class C>
inline typename boost::range_iterator<ForwardRange>::type max_element(ForwardRange& range, C c) {
    return adobe::max_element(boost::begin(range), boost::end(range), c);
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange, class C, class P>
inline typename boost::range_iterator<ForwardRange>::type max_element(ForwardRange& range, C c, P p) {
    return adobe::max_element(boost::begin(range), boost::end(range), c, p);
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange, class C>
inline typename boost::range_const_iterator<ForwardRange>::type
max_element(const ForwardRange& range, C c) {
    return adobe::max_element(boost::begin(range), boost::end(range), c);
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange, class C, class P>
inline typename boost::range_const_iterator<ForwardRange>::type
max_element(const ForwardRange& range, C c, P p) {
    return adobe::max_element(boost::begin(range), boost::end(range), c, p);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
