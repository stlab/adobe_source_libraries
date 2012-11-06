/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_GENERATE_HPP
#define ADOBE_ALGORITHM_GENERATE_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup generate generate
\ingroup mutating_algorithm

\see
    - STL documentation for \ref stldoc_generate
*/
/*************************************************************************************************/
/*!
    \ingroup generate

    \brief generate implementation
*/
template <class ForwardRange, class Generator>
inline void generate(ForwardRange& range, Generator gen)
{
    std::generate(boost::begin(range), boost::end(range), gen);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
