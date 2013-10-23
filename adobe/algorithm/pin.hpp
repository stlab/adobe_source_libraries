/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_PIN_HPP
#define ADOBE_ALGORITHM_PIN_HPP

#include <adobe/config.hpp>

#if ADOBE_IS_DEPRECATED_ERROR(100039)
#error "deprecated adobe/algorithm/pin.hpp use adobe/algorithm/clamp.hpp instead."
#endif

#include <adobe/algorithm/clamp.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup pin pin
\ingroup non_mutating_algorithm

\deprecated
The \c pin() and \c pin_unsafe() functions have been deprecated in favor of \c clamp() and
\c clamp_unordered(). Note that the parameter order is different.

If <code>x</code> is outside the range <code>(min, max)</code> then the closest of either
<code>min</code> or <code>max</code> is returned. If <code>x</code> is within the range
specified then <code>x</code> is returned.

\pre
    <code>min <= max</code>

\post
    <code>min <= result <= max</code>

pin_safe is similar to pin but should be used when you are not sure which bound is the minimum
and which is the maximum. It is 'safe' in that, at the cost of an extra comparison, will assert
the min and max bounds are placed where they should be when the actual pin is executed.
*/
/*************************************************************************************************/
/*!
    \ingroup pin

    \brief pin implementation
*/
template <typename T, typename R>
inline const T& pin(const T& min, const T& x, const T& max, R r)
{ return clamp(x, min, max, r); }

/*************************************************************************************************/
/*!
    \ingroup pin

    \brief pin implementation
*/
template <typename T>
inline const T& pin(const T& min, const T& x, const T& max)
{ return clamp(x, min, max); }


/****************************************************************************************************/
/*!
    \ingroup pin

    \brief pin_safe implementation
*/
template <typename T, typename R>
const T& pin_safe(const T& min, const T& x, const T& max, R r)
{ return clamp_unordered(x, min, max, r); }

/****************************************************************************************************/
/*!
    \ingroup pin

    \brief pin_safe implementation
*/
template <typename T, typename R>
const T& pin_safe(const T& min, const T& x, const T& max)
{ return clamp_unordered(x, min, max); }

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
