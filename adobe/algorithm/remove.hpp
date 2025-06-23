/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_REMOVE_HPP
#define ADOBE_ALGORITHM_REMOVE_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>
#include <functional>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup remove remove
\ingroup mutating_algorithm

\see
    - [STL documentation for remove](https://www.boost.org/sgi/stl/remove.html)
    - [STL documentation for remove_if](https://www.boost.org/sgi/stl/remove_if.html)
    - [STL documentation for remove_copy](https://www.boost.org/sgi/stl/remove_copy.html)
    - [STL documentation for remove_copy_if](https://www.boost.org/sgi/stl/remove_copy_if.html)
*/
/**************************************************************************************************/
/*!
    \ingroup remove

    \brief remove implementation
*/
template <class InputRange, class T>
inline typename boost::range_iterator<InputRange>::type remove(InputRange& range, const T& value) {
    return std::remove(boost::begin(range), boost::end(range), value);
}

/*!
    \ingroup remove

    \brief remove implementation
*/
template <class InputIterator, class Predicate>
inline InputIterator remove_if(InputIterator first, InputIterator last, Predicate pred) {
    return std::remove_if(first, last, std::bind(pred, std::placeholders::_1));
}

/*!
    \ingroup remove

    \brief remove implementation
*/
template <class InputRange, class Predicate>
inline typename boost::range_iterator<InputRange>::type remove_if(InputRange& range,
                                                                  Predicate pred) {
    return adobe::remove_if(boost::begin(range), boost::end(range), pred);
}

/*!
    \ingroup remove

    \brief remove implementation
*/
template <class InputRange, class OutputIterator, class T>
inline typename boost::range_iterator<InputRange>::type
remove_copy(InputRange& range, OutputIterator result, const T& value) {
    return std::remove_copy(boost::begin(range), boost::end(range), result, value);
}

/*!
    \ingroup remove

    \brief remove implementation
*/
template <class InputRange, class OutputIterator, class T>
inline typename boost::range_const_iterator<InputRange>::type
remove_copy(const InputRange& range, OutputIterator result, const T& value) {
    return std::remove_copy(boost::begin(range), boost::end(range), result, value);
}

/*!
    \ingroup remove

    \brief remove implementation
*/
template <class InputIterator, class OutputIterator, class Predicate>
inline InputIterator remove_copy_if(InputIterator first, InputIterator last, OutputIterator result,
                                    Predicate pred) {
    return std::remove_copy_if(first, last, result, std::bind(pred, std::placeholders::_1));
}

/*!
    \ingroup remove

    \brief remove implementation
*/
template <class InputRange, class OutputIterator, class Predicate>
inline typename boost::range_iterator<InputRange>::type
remove_copy_if(InputRange& range, OutputIterator result, Predicate pred) {
    return adobe::remove_copy_if(boost::begin(range), boost::end(range), result, pred);
}

/*!
    \ingroup remove

    \brief remove implementation
*/
template <class InputRange, class OutputIterator, class Predicate>
inline typename boost::range_const_iterator<InputRange>::type
remove_copy_if(const InputRange& range, OutputIterator result, Predicate pred) {
    return adobe::remove_copy_if(boost::begin(range), boost::end(range), result, pred);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
