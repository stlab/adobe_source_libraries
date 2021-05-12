/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_TRANSFORM_HPP
#define ADOBE_ALGORITHM_TRANSFORM_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>
#include <functional>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup transform transform
\ingroup mutating_algorithm

\see
    - STL documentation for \ref stldoc_transform
*/
/**************************************************************************************************/
/*!
    \ingroup transform

    \brief transform implementation
*/
template <class InputIterator, class OutputIterator, class UnaryOperation>
inline OutputIterator transform(InputIterator first, InputIterator last, OutputIterator result,
                                UnaryOperation op) {
    return std::transform(first, last, result, std::bind(op, std::placeholders::_1));
}

/*!
    \ingroup transform

    \brief transform implementation
*/
template <class InputRange, class OutputIterator, class UnaryOperation>
inline OutputIterator transform(InputRange& range, OutputIterator result, UnaryOperation op) {
    return adobe::transform(boost::begin(range), boost::end(range), result, op);
}


/*!
    \ingroup transform

    \brief transform implementation
*/
template <class InputRange, class OutputIterator, class UnaryOperation>
inline OutputIterator transform(const InputRange& range, OutputIterator result, UnaryOperation op) {
    return adobe::transform(boost::begin(range), boost::end(range), result, op);
}


/*!
    \ingroup transform

    \brief transform implementation
*/
template <class InputIterator1, class InputIterator2, class OutputIterator, class BinaryOperation>
inline OutputIterator transform(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2,
                                OutputIterator result, BinaryOperation binary_op) {
    return std::transform(first1, last1, first2, result,
                          std::bind(binary_op, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup transform

    \brief transform implementation
*/
template <class InputRange1, class InputIterator2, class OutputIterator, class BinaryOperation>
inline OutputIterator transform(InputRange1& range1, InputIterator2 first2, OutputIterator result,
                                BinaryOperation binary_op) {
    return adobe::transform(boost::begin(range1), boost::end(range1), first2, result, binary_op);
}

/*!
    \ingroup transform

    \brief transform implementation
*/
template <class InputRange1, class InputIterator2, class OutputIterator, class BinaryOperation>
inline OutputIterator transform(const InputRange1& range1, InputIterator2 first2,
                                OutputIterator result, BinaryOperation binary_op) {
    return adobe::transform(boost::begin(range1), boost::end(range1), first2, result, binary_op);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
