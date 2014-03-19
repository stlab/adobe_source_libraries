/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_FOR_EACH_POSITION_HPP
#define ADOBE_ALGORITHM_FOR_EACH_POSITION_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>
#include <functional>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup for_each_position for_each_position
\ingroup non_mutating_algorithm

\c for_each_position applies the function \c f to each iterator, as opposed to element, in the range
[first, last); \c f's return value, if any, is ignored. Applications are performed in forward order,
i.e. from first to last.

\complexity
Linear. Exactly <code>last - first</code> applications of \c f.
*/
/*************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
namespace implementation {

/*************************************************************************************************/

template <class InputIterator, class UnaryFunction>
void for_each_position(InputIterator first, InputIterator last, UnaryFunction f) {
    while (first != last) {
        f(first);
        ++first;
    }
}

/*************************************************************************************************/

} // namespace implementation
#endif
/*************************************************************************************************/
/*!
    \ingroup for_each_position

    \brief for_each_position implementation
*/
template <class InputIterator, class UnaryFunction>
inline void for_each_position(InputIterator first, InputIterator last, UnaryFunction f) {
    adobe::implementation::for_each_position(first, last, std::bind(f, std::placeholders::_1));
}

/*!
    \ingroup for_each_position

    \brief for_each_position implementation
*/
template <class InputRange, class UnaryFunction>
inline void for_each_position(InputRange& range, UnaryFunction f) {
    adobe::for_each_position(boost::begin(range), boost::end(range), f);
}

/*!
    \ingroup for_each_position

    \brief for_each_position implementation
*/
template <class InputRange, class UnaryFunction>
inline void for_each_position(const InputRange& range, UnaryFunction f) {
    adobe::for_each_position(boost::begin(range), boost::end(range), f);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
