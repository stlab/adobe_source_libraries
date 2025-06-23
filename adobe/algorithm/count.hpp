/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_COUNT_HPP
#define ADOBE_ALGORITHM_COUNT_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/difference_type.hpp>
#include <boost/range/end.hpp>

#include <algorithm>
#include <functional>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup count count
\ingroup non_mutating_algorithm

\see
    - [STL documentation for count](https://www.boost.org/sgi/stl/count.html)
    - [STL documentation for count_if](https://www.boost.org/sgi/stl/count_if.html)
*/
/**************************************************************************************************/
/*!
    \ingroup count

    \brief count implementation
*/
template <class InputRange, class T>
inline typename boost::range_difference<InputRange>::type count(InputRange& range, T& value) {
    return std::count(boost::begin(range), boost::end(range), value);
}

/*!
    \ingroup count

    \brief count implementation
*/
template <class InputRange, class T>
inline typename boost::range_difference<InputRange>::type count(const InputRange& range, T& value) {
    return std::count(boost::begin(range), boost::end(range), value);
}

/*!
    \ingroup count

    \brief count implementation
*/
template <class InputIterator, class Predicate>
inline typename std::iterator_traits<InputIterator>::difference_type
count_if(InputIterator first, InputIterator last, Predicate pred) {
    return std::count_if(first, last, std::bind(pred, std::placeholders::_1));
}

/*!
    \ingroup count

    \brief count implementation
*/
template <class InputRange, class Predicate>
inline typename boost::range_difference<InputRange>::type count_if(InputRange& range,
                                                                   Predicate pred) {
    return adobe::count_if(boost::begin(range), boost::end(range), pred);
}

/*!
    \ingroup count

    \brief count implementation
*/
template <class InputRange, class Predicate>
inline typename boost::range_difference<InputRange>::type count_if(const InputRange& range,
                                                                   Predicate pred) {
    return adobe::count_if(boost::begin(range), boost::end(range), pred);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
