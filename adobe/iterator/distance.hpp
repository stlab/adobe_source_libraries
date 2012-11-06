/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ITERATOR_DISTANCE_HPP
#define ADOBE_ITERATOR_DISTANCE_HPP

#include <adobe/config.hpp>

#include <iterator>

#include <boost/range/difference_type.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

/*!
\ingroup adobe_iterator
*/

template <typename I> // I models InputRange
inline boost::difference_type<I>::type distance(I& range)
{ return std::distance(boost::begin(range), boost::end(range)); }

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif
// ADOBE_ITERATOR_DISTANCE_HPP
