/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_IOTA_HPP
#define ADOBE_ALGORITHM_IOTA_HPP

#include <adobe/config.hpp>

#include <adobe/functional.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup iota iota
\ingroup numeric

\c iota assigns sequentially increasing values to a range. That is, it assigns \c value to
\c *first, <code>value + 1</code> to <code>*(first + 1)</code> and so on. In general, each
iterator \c i in the range <code>[first, last)</code> is assigned
<code>value + (i - first)</code>.

\result the next value is returned.

\requirements
    - \c ForwardIterator is mutable.
    - \c T is Assignable.
    - If \c x is an object of type \c T, then \c ++x is defined.
    - \c T is convertible to <code>ForwardIterator::value_type</code>
    - <code>[first, last)</code> is a valid range.

\complexity
    - Linear. Exactly <code>last - first</code> assignments.
*/
/*************************************************************************************************/
/*!
    \ingroup iota

    \brief iota implementation
*/
template <class ForwardIterator, class T>
T iota (ForwardIterator first, ForwardIterator last, const T& value)
{
    T result = value;
    while (first != last) {
        *first = result;
        ++first;
        ++result;
    }
    return result;
}

/*!
    \ingroup iota

    \brief iota implementation
*/
template <class ForwardRange, class T>
T iota (ForwardRange& range, const T& value)
{
    return adobe::iota(boost::begin(range), boost::end(range), value);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
