/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_HEAP_HPP
#define ADOBE_ALGORITHM_HEAP_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/bind.hpp>

#include <algorithm>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup heap heap operations
\ingroup sorting

\see
    - STL documentation for \ref stldoc_push_heap
    - STL documentation for \ref stldoc_pop_heap
    - STL documentation for \ref stldoc_make_heap
    - STL documentation for \ref stldoc_sort_heap
*/
/*************************************************************************************************/
/*!
    \ingroup heap

    \brief heap implementation
*/
template <class RandomAccessRange>
inline void push_heap(RandomAccessRange& range)
{
    return std::push_heap(boost::begin(range), boost::end(range));
}

/*!
    \ingroup heap

    \brief heap implementation
*/
template <class RandomAccessIterator, class Compare>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    return std::push_heap(first, last, boost::bind(comp, _1, _2));
}

/*!
    \ingroup heap

    \brief heap implementation
*/
template <class RandomAccessRange, class Compare>
inline void push_heap(RandomAccessRange& range, Compare comp)
{
    return adobe::push_heap(boost::begin(range), boost::end(range), comp);
}

/*!
    \ingroup heap

    \brief heap implementation
*/
template <class RandomAccessRange>
inline void pop_heap(RandomAccessRange& range)
{
    return std::pop_heap(boost::begin(range), boost::end(range));
}

/*!
    \ingroup heap

    \brief heap implementation
*/
template <class RandomAccessIterator, class Compare>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    return std::pop_heap(first, last, boost::bind(comp, _1, _2));
}

/*!
    \ingroup heap

    \brief heap implementation
*/
template <class RandomAccessRange, class Compare>
inline void pop_heap(RandomAccessRange& range, Compare comp)
{
    return adobe::pop_heap(boost::begin(range), boost::end(range), comp);
}

/*!
    \ingroup heap

    \brief heap implementation
*/
template <class RandomAccessRange>
inline void make_heap(RandomAccessRange& range)
{
    return std::make_heap(boost::begin(range), boost::end(range));
}

/*!
    \ingroup heap

    \brief heap implementation
*/
template <class RandomAccessIterator, class Compare>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    return std::make_heap(first, last, boost::bind(comp, _1, _2));
}

/*!
    \ingroup heap

    \brief heap implementation
*/
template <class RandomAccessRange, class Compare>
inline void make_heap(RandomAccessRange& range, Compare comp)
{
    return adobe::make_heap(boost::begin(range), boost::end(range), comp);
}

/*!
    \ingroup heap

    \brief heap implementation
*/
template <class RandomAccessRange>
inline void sort_heap(RandomAccessRange& range)
{
    return std::sort_heap(boost::begin(range), boost::end(range));
}

/*!
    \ingroup heap

    \brief heap implementation
*/
template <class RandomAccessIterator, class Compare>
inline void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    return std::sort_heap(first, last, boost::bind(comp, _1, _2));
}

/*!
    \ingroup heap

    \brief heap implementation
*/
template <class RandomAccessRange, class Compare>
inline void sort_heap(RandomAccessRange& range, Compare comp)
{
    return adobe::sort_heap(boost::begin(range), boost::end(range), comp);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
