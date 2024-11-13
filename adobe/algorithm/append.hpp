/*
    Copyright 2024 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_APPEND_HPP
#define ADOBE_ALGORITHM_APPEND_HPP

#include <iterator>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

/// Insert the elements  of _input_range_ `r` at the end of the _sequence container_ `c`,
/// returning the position corresponding to the incoming `end` of `c`.
template <class T, class R>
inline auto append(T& c, const R& r) {
    return c.insert(std::end(c), std::begin(r), std::end(r));
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif
