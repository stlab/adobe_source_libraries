/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_FILL_HPP
#define ADOBE_ALGORITHM_FILL_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup fill fill
\ingroup mutating_algorithm

\see
    - STL documentation for \ref stldoc_fill
*/
/**************************************************************************************************/
/*!
    \ingroup fill

    \brief fill implementation
*/
template <class ForwardRange, class T>
inline void fill(ForwardRange& range, const T& value) {
    std::fill(boost::begin(range), boost::end(range), value);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
