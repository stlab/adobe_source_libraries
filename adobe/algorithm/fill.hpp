/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_FILL_HPP
#define ADOBE_ALGORITHM_FILL_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup fill fill
\ingroup mutating_algorithm

\see
    - STL documentation for \ref stldoc_fill
*/
/*************************************************************************************************/
/*!
    \ingroup fill

    \brief fill implementation
*/
template <class ForwardRange, class T>
inline void fill(ForwardRange& range, const T& value)
{
    std::fill(boost::begin(range), boost::end(range), value);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
