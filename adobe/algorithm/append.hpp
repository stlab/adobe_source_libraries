/*
    Copyright 2024 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_APPEND_HPP
#define ADOBE_ALGORITHM_APPEND_HPP

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

/// Insert the range `r` at the end of the sequence container `x`.
/// Returns an iterator pointing to the first element inserted, or `x.end()` if `r` is empty.
template <class T, // T models SequenceContainer
          class R> // R models Range
inline auto append(T& x, const R& r) {
    return x.insert(std::end(x), std::begin(r), std::end(r));
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif
