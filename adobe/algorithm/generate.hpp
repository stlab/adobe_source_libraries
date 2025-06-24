/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_GENERATE_HPP
#define ADOBE_ALGORITHM_GENERATE_HPP

#include <adobe/config.hpp>

#include <algorithm>
#include <iterator>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup generate generate
\ingroup mutating_algorithm

\see
    - [STL documentation for generate](https://www.boost.org/sgi/stl/generate.html)
*/
/**************************************************************************************************/
/*!
    \ingroup generate

    \brief generate implementation
*/
template <class ForwardRange, class Generator>
inline void generate(ForwardRange& range, Generator gen) {
    std::generate(std::begin(range), std::end(range), gen);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
