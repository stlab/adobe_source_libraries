/*
    Copyright 2005-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_SELECT_HPP
#define ADOBE_ALGORITHM_SELECT_HPP

#include <cassert>

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup select select
\ingroup sorting

The select() algorithms are building blocks for other algorithms such as median() and
clamp(). The general form of a select algorithm is:

select_<argument_index>_<argument_ordering>()

For example: <code>select_1_3_ab(a, b, c)</code> means "select the second of three element
(index starts at zero) assuming that arguments \c a and \c b are supplied in non-decreasing order."

Because these routines are low level, they do not atomatically bind their comparison argument
nor provide convience functions defaulting the comparison to less(). That is handled at a higher
level. 

All of the select function are stable. An algorithm is stable if it respects the original order of
equivalent objects. So if we think of minimum and maximum as selecting, respectively, the smallest
and second-smallest from a list of two arguments, stability requires that when called with 
equivalent elements, minimum should return the first and maximum the second.

\see
    - \ref minmax
    - \ref median
    - \ref clamp
*/

/**************************************************************************************************/

/*!
    \ingroup select
    \brief select_0 implementation
*/
template <class T, class R>
inline const T& select_0_2(const T& a, const T& b, R r)
{
    return r(b, a) ? b : a;
}

/*!
    \ingroup select
    \brief select_0 implementation
*/
template <class T, class R>
inline T& select_0_2(T& a, T& b, R r)
{
    return r(b, a) ? b : a;
}

/*!
    \ingroup select
    \brief select_0 implementation
*/
template <class T, class R>
inline const T& select_1_2(const T& a, const T& b, R r)
{
    return r(b, a) ? a : b;
}

/*!
    \ingroup select
    \brief select_0 implementation
*/
template <class T, class R>
inline T& select_1_2(T& a, T& b, R r)
{
    return r(b, a) ? a : b;
}

/*!
    \ingroup select
    \brief select_1_ab implementation
*/
template <typename T, typename R>
inline const T& select_1_3_ac(const T& a, const T& b, const T& c, R r)
{
    assert(!r(c, a) && "WARNING (sparent) : a and b must be non-decreasing");
    return r(b, a) ? a : select_0_2(b, c, r);
}

/*!
    \ingroup select
    \brief select_1_ab implementation
*/
template <typename T, typename R>
inline T& select_1_3_ac(T& a, T& b, T& c, R r)
{
    assert(!r(c, a) && "WARNING (sparent) : a and b must be non-decreasing");
    return r(b, a) ? a : select_0_2(b, c, r);
}

/*!
    \ingroup select
    \brief select_1_ab implementation
*/
template <typename T, typename R>
inline const T& select_1_3_ab(const T& a, const T& b, const T& c, R r)
{
    assert(!r(b, a) && "WARNING (sparent) : a and b must be non-decreasing");
    return r(c, b) ? select_1_2(a, c, r) : b;
}

/*!
    \ingroup select
    \brief select_1_ab implementation
*/
template <typename T, typename R>
inline T& select_1_3_ab(T& a, T& b, T& c, R r)
{
    assert(!r(b, a) && "WARNING (sparent) : a and b must be non-decreasing");
    return r(c, b) ? select_1_2(a, c, r) : b;
}

/*!
    \ingroup select
    \brief select_1 implementation
*/
template <typename T, typename R>
inline const T& select_1_3(const T& a, const T& b, const T& c, R r)
{ return r(b, a) ? select_1_3_ab(b, a, c, r) : select_1_3_ab(a, b, c, r); }

/*!
    \ingroup select
    \brief select_1 implementation
*/
template <typename T, typename R>
inline T& select_1_3(T& a, T& b, T& c, R r)
{ return r(b, a) ? select_1_3_ab(b, a, c, r) : select_1_3_ab(a, b, c, r); }

/**************************************************************************************************/

} // namespace adobe

#endif
