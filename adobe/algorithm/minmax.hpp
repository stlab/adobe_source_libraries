/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_MINMAX_HPP
#define ADOBE_ALGORITHM_MINMAX_HPP

#include <adobe/config.hpp>

#include <algorithm>
#include <functional>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <adobe/algorithm/select.hpp>

#include <adobe/functional/operator.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
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
    - [STL documentation for min](https://www.boost.org/sgi/stl/min.html)
    - [STL documentation for max](https://www.boost.org/sgi/stl/max.html)
    - [STL documentation for min_element](https://www.boost.org/sgi/stl/min_element.html)
    - [STL documentation for max_element](https://www.boost.org/sgi/stl/max_element.html)
*/
/**************************************************************************************************/

/*!
    \ingroup minmax

    \brief minmax implementation
*/

template <class T, class R>
inline const T&(min)(const T& a, const T& b, R r) {
    return select_0_2(a, b, std::bind(r, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/

template <class T, class R>
inline T&(min)(T& a, T& b, R r) {
    return select_0_2(a, b, std::bind(r, std::placeholders::_1, std::placeholders::_2));
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

template <class T, class R>
inline const T&(max)(const T& a, const T& b, R r) {
    return select_1_2(a, b, std::bind(r, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/

template <class T, class R>
inline T&(max)(T& a, T& b, R r) {
    return select_1_2(a, b, std::bind(r, std::placeholders::_1, std::placeholders::_2));
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
template <class ForwardIterator, class R>
inline ForwardIterator min_element(ForwardIterator first, ForwardIterator last, R r) {
    return std::min_element(first, last,
                            std::bind(r, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange, class R>
inline typename boost::range_iterator<ForwardRange>::type min_element(ForwardRange& range, R r) {
    return adobe::min_element(boost::begin(range), boost::end(range), r);
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange, class R>
inline typename boost::range_const_iterator<ForwardRange>::type
min_element(const ForwardRange& range, R r) {
    return adobe::min_element(boost::begin(range), boost::end(range), r);
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
template <class ForwardIterator, class R>
inline ForwardIterator max_element(ForwardIterator first, ForwardIterator last, R r) {
    return std::max_element(first, last,
                            std::bind(r, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange, class R>
inline typename boost::range_iterator<ForwardRange>::type max_element(ForwardRange& range, R r) {
    return adobe::max_element(boost::begin(range), boost::end(range), r);
}

/*!
    \ingroup minmax

    \brief minmax implementation
*/
template <class ForwardRange, class R>
inline typename boost::range_const_iterator<ForwardRange>::type
max_element(const ForwardRange& range, R r) {
    return adobe::max_element(boost::begin(range), boost::end(range), r);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
