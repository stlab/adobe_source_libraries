/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_SET_HPP
#define ADOBE_ALGORITHM_SET_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/bind.hpp>

#include <algorithm>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup set set operations on sorted ranges
\ingroup sorting

\see
    - STL documentation for \ref stldoc_includes
    - STL documentation for \ref stldoc_set_union
    - STL documentation for \ref stldoc_set_intersection
    - STL documentation for \ref stldoc_set_difference
    - STL documentation for \ref stldoc_set_symmetric_difference
*/
/*************************************************************************************************/
/*!
    \ingroup set

    \brief set implementation
*/
template <class InputRange1, class InputRange2>
inline bool includes(const InputRange1& range1, const InputRange2& range2)
{
    return std::includes(   boost::begin(range1), boost::end(range1),
                            boost::begin(range2), boost::end(range2));
}

/*!
    \ingroup set

    \brief set implementation
*/
template <class InputIterator1, class InputIterator2, class Compare>
inline bool includes(   InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        Compare comp)

{
    return std::includes(first1, last1, first2, last2, boost::bind(comp, _1, _2));
}

/*!
    \ingroup set

    \brief set implementation
*/
template <class InputRange1, class InputRange2, class Compare>
inline bool includes(const InputRange1& range1, const InputRange2& range2, Compare comp)
{
    return adobe::includes( boost::begin(range1), boost::end(range1),
                                boost::begin(range2), boost::end(range2),
                                comp);
}

/*!
    \ingroup set

    \brief set implementation
*/
template <class InputRange1, class InputRange2, class OutputIterator>
inline OutputIterator set_union(const InputRange1& range1, const InputRange2& range2, OutputIterator result)
{
    return std::set_union(  boost::begin(range1), boost::end(range1),
                            boost::begin(range2), boost::end(range2),
                            result);
}

/*!
    \ingroup set

    \brief set implementation
*/
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
inline OutputIterator set_union(InputIterator1 first1, InputIterator1 last1, 
                                InputIterator2 first2, InputIterator2 last2,
                                OutputIterator result, Compare comp)
{
    return std::set_union(first1, last1, first2, last2, result, boost::bind(comp, _1, _2));
}

/*!
    \ingroup set

    \brief set implementation
*/
template <class InputRange1, class InputRange2, class OutputIterator, class Compare>
inline OutputIterator set_union(const InputRange1& range1, const InputRange2& range2, OutputIterator result, Compare comp)
{
    return adobe::set_union(boost::begin(range1), boost::end(range1),
                                boost::begin(range2), boost::end(range2),
                                result, comp);
}

/*!
    \ingroup set

    \brief set implementation
*/
template <class InputRange1, class InputRange2, class OutputIterator>
inline OutputIterator set_intersection(const InputRange1& range1, const InputRange2& range2, OutputIterator result)
{
    return std::set_intersection(   boost::begin(range1), boost::end(range1),
                                    boost::begin(range2), boost::end(range2),
                                    result);
}

/*!
    \ingroup set

    \brief set implementation
*/
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
inline OutputIterator set_intersection( InputIterator1 first1, InputIterator1 last1, 
                                        InputIterator2 first2, InputIterator2 last2,
                                        OutputIterator result, Compare comp)
{
    return std::set_intersection(first1, last1, first2, last2, result, boost::bind(comp, _1, _2));
}

/*!
    \ingroup set

    \brief set implementation
*/
template <class InputRange1, class InputRange2, class OutputIterator, class Compare>
inline OutputIterator set_intersection(const InputRange1& range1, const InputRange2& range2, OutputIterator result, Compare comp)
{
    return adobe::set_intersection( boost::begin(range1), boost::end(range1),
                                        boost::begin(range2), boost::end(range2),
                                        result, comp);
}

/*!
    \ingroup set

    \brief set implementation
*/
template <class InputRange1, class InputRange2, class OutputIterator>
inline OutputIterator set_difference(const InputRange1& range1, const InputRange2& range2, OutputIterator result)
{
    return std::set_difference( boost::begin(range1), boost::end(range1),
                                    boost::begin(range2), boost::end(range2),
                                    result);
}

/*!
    \ingroup set

    \brief set implementation
*/
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
inline OutputIterator set_difference(   InputIterator1 first1, InputIterator1 last1, 
                                        InputIterator2 first2, InputIterator2 last2,
                                        OutputIterator result, Compare comp)
{
    return std::set_difference(first1, last1, first2, last2, result, boost::bind(comp, _1, _2));
}

/*!
    \ingroup set

    \brief set implementation
*/
template <class InputRange1, class InputRange2, class OutputIterator, class Compare>
inline OutputIterator set_difference(const InputRange1& range1, const InputRange2& range2, OutputIterator result, Compare comp)
{
    return adobe::set_difference(   boost::begin(range1), boost::end(range1),
                                        boost::begin(range2), boost::end(range2),
                                        result, comp);
}

/*!
    \ingroup set

    \brief set implementation
*/
template <class InputRange1, class InputRange2, class OutputIterator>
inline OutputIterator set_symmetric_difference(const InputRange1& range1, const InputRange2& range2, OutputIterator result)
{
    return std::set_symmetric_difference(   boost::begin(range1), boost::end(range1),
                                            boost::begin(range2), boost::end(range2),
                                            result);
}

/*!
    \ingroup set

    \brief set implementation
*/
template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
inline OutputIterator set_symmetric_difference( InputIterator1 first1, InputIterator1 last1, 
                                                InputIterator2 first2, InputIterator2 last2,
                                                OutputIterator result, Compare comp)
{
    return std::set_symmetric_difference(first1, last1, first2, last2, result, boost::bind(comp, _1, _2));
}

/*!
    \ingroup set

    \brief set implementation
*/
template <class InputRange1, class InputRange2, class OutputIterator, class Compare>
inline OutputIterator set_symmetric_difference(const InputRange1& range1, const InputRange2& range2, OutputIterator result, Compare comp)
{
    return adobe::set_symmetric_difference( boost::begin(range1), boost::end(range1),
                                                boost::begin(range2), boost::end(range2),
                                                result, comp);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
