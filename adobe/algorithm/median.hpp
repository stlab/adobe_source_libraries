/*
    Copyright 2005-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_MEDIAN_HPP
#define ADOBE_ALGORITHM_MEDIAN_HPP

#include <boost/bind.hpp>

#include <adobe/algorithm/select.hpp>
#include <adobe/functional.hpp>

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup median median
\ingroup sorting

As with adobe::min and adobe::max, adobe::median is a select algorithm. The median algorithm returns
the second of three arguments. The algorithm is stable, which is to say that if the arguments are
in non-decresing order then the identity of the returned element will be the second identity of the
second argument.

\see
    - \ref minmax
    - \ref clamp
    - \ref select
*/

/**************************************************************************************************/

/*!
    \ingroup median
    \brief median implementation
*/
template <typename T, typename R>
inline const T& median(const T& a, const T& b, const T& c, R r)
{ return select_1_3(a, b, c, boost::bind(r, _1, _2)); }

/*!
    \ingroup median
    \brief median implementation
*/
template <typename T, typename R>
inline T& median(T& a, T& b, T& c, R r)
{ return select_1_3(a, b, c, boost::bind(r, _1, _2)); }

/*!
    \ingroup median
    \brief median implementation
*/
template <typename T>
inline const T& median(const T& a, const T& b, const T& c)
{ return select_1_3(a, b, c, less()); }

/*!
    \ingroup median
    \brief median implementation
*/
template <typename T>
inline T& median(T& a, T& b, T& c)
{ return select_1_3(a, b, c, less()); }

/**************************************************************************************************/

} // namespace adobe
#endif
