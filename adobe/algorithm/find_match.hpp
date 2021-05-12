/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_FIND_MATCH_HPP
#define ADOBE_ALGORITHM_FIND_MATCH_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>
#include <functional>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup find_match find_match
\ingroup non_mutating_algorithm

Returns the first iterator \c i in the range <code>[first, last)</code> such that <code>comp(value,
*i)</code> return true. Returns last if no such iterator exists.

\complexity
Linear: at most <code>last - first</code> applications of \c comp.
*/
/**************************************************************************************************/
/*!
    \ingroup find_match

    \brief find_match implementation
*/
template <class InputIterator, class T, class Compare>
inline InputIterator find_match(InputIterator first, InputIterator last, const T& value,
                                Compare comp) {
    return std::find_if(first, last, std::bind(comp, value, std::placeholders::_1));
}

/*!
    \ingroup find_match

    \brief find_match implementation
*/
template <class InputRange, class T, class Compare>
inline typename boost::range_iterator<InputRange>::type find_match(InputRange& range,
                                                                   const T& value, Compare comp) {
    return adobe::find_match(boost::begin(range), boost::end(range), value, comp);
}

/*!
    \ingroup find_match

    \brief find_match implementation
*/
template <class InputRange, class T, class Compare>
inline typename boost::range_const_iterator<InputRange>::type
find_match(const InputRange& range, const T& value, Compare comp) {
    return adobe::find_match(boost::begin(range), boost::end(range), value, comp);
}

/*!
    \ingroup find_match

    \brief find_match implementation
*/
template <class InputIterator, class T, class Compare>
inline InputIterator find_match(InputIterator first, InputIterator last, const T& value) {
    return std::find_if(first, last, std::bind(std::equal_to<T>(), value, std::placeholders::_1));
}

/*!
    \ingroup find_match

    \brief find_match implementation
*/
template <class InputRange, class T, class Compare>
inline typename boost::range_iterator<InputRange>::type find_match(InputRange& range,
                                                                   const T& value) {
    return adobe::find_match(boost::begin(range), boost::end(range), value);
}

/*!
    \ingroup find_match

    \brief find_match implementation
*/
template <class InputRange, class T, class Compare>
inline typename boost::range_const_iterator<InputRange>::type find_match(const InputRange& range,
                                                                         const T& value) {
    return adobe::find_match(boost::begin(range), boost::end(range), value);
}


/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
