/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_UNIQUE_HPP
#define ADOBE_ALGORITHM_UNIQUE_HPP

#include <adobe/config.hpp>

#include <algorithm>
#include <functional>
#include <iterator>

#include <adobe/algorithm/sort.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup unique unique
\ingroup mutating_algorithm

\see
    - [STL documentation for unique](https://www.boost.org/sgi/stl/unique.html)
    - [STL documentation for unique_copy](https://www.boost.org/sgi/stl/unique_copy.html)
*/
/**************************************************************************************************/
/*!
    \ingroup unique

    \brief unique implementation
*/
template <class ForwardRange>
inline auto unique(ForwardRange& range) {
    return std::unique(std::begin(range), std::end(range));
}

/*!
    \ingroup unique

    \brief unique implementation
*/
template <class ForwardIterator, class BinaryPredicate>
inline ForwardIterator unique(ForwardIterator first, ForwardIterator last, BinaryPredicate pred) {
    return std::unique(first, last, std::bind(pred, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup unique

    \brief unique implementation
*/
template <class ForwardRange, class BinaryPredicate>
inline auto unique(ForwardRange& range,
                                                                 BinaryPredicate pred) {
    return adobe::unique(std::begin(range), std::end(range), pred);
}

/*!
    \ingroup unique

    \brief unique implementation
*/
template <class InputRange, class OutputIterator>
inline OutputIterator unique_copy(InputRange& range, OutputIterator result) {
    return std::unique_copy(std::begin(range), std::end(range), result);
}

/*!
    \ingroup unique

    \brief unique implementation
*/
template <class InputRange, class OutputIterator>
inline OutputIterator unique_copy(const InputRange& range, OutputIterator result) {
    return std::unique_copy(std::begin(range), std::end(range), result);
}

/*!
    \ingroup unique

    \brief unique implementation
*/
template <class InputIterator, class OutputIterator, class BinaryPredicate>
inline OutputIterator unique_copy(InputIterator first, InputIterator last, OutputIterator result,
                                  BinaryPredicate pred) {
    return std::unique_copy(first, last, result,
                            std::bind(pred, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup unique

    \brief unique implementation
*/
template <class InputRange, class OutputIterator, class BinaryPredicate>
inline OutputIterator unique_copy(InputRange& range, OutputIterator result, BinaryPredicate pred) {
    return adobe::unique_copy(std::begin(range), std::end(range), result, pred);
}

/*!
    \ingroup unique

    \brief unique implementation
*/
template <class InputRange, class OutputIterator, class BinaryPredicate>
inline OutputIterator unique_copy(const InputRange& range, OutputIterator result,
                                  BinaryPredicate pred) {
    return adobe::unique_copy(std::begin(range), std::end(range), result, pred);
}

/// Sorts the _sequence container_ `c` and erases all but one matching element.
template <class T>
inline void sort_unique(T& c) {
    sort(c);
    c.erase(unique(c), std::end(c));
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
