/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_SEARCH_HPP
#define ADOBE_ALGORITHM_SEARCH_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <algorithm>
#include <functional>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup search search
\ingroup non_mutating_algorithm

\see
    - [STL documentation for search](https://www.boost.org/sgi/stl/search.html)
    - [STL documentation for search_n](https://www.boost.org/sgi/stl/search_n.html)
*/
/**************************************************************************************************/
/*!
    \ingroup search

    \brief search implementation
*/
template <class ForwardRange1, class ForwardRange2>
inline typename boost::range_const_iterator<ForwardRange1>::type
search(const ForwardRange1& range1, const ForwardRange2& range2) {
    return std::search(boost::begin(range1), boost::end(range1), boost::begin(range2),
                       boost::end(range2));
}

/*!
    \ingroup search

    \brief search implementation
*/
template <class ForwardRange1, class ForwardRange2>
inline typename boost::range_iterator<ForwardRange1>::type search(ForwardRange1& range1,
                                                                  const ForwardRange2& range2) {
    return std::search(boost::begin(range1), boost::end(range1), boost::begin(range2),
                       boost::end(range2));
}

/*!
    \ingroup search

    \brief search implementation
*/
template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
inline ForwardIterator1 search(ForwardIterator1 first1, ForwardIterator1 last1,
                               ForwardIterator2 first2, ForwardIterator2 last2,
                               BinaryPredicate pred) {
    return std::search(first1, last1, first2, last2,
                       std::bind(pred, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup search

    \brief search implementation
*/
template <class ForwardRange1, class ForwardRange2, class BinaryPredicate>
inline typename boost::range_iterator<ForwardRange1>::type
search(ForwardRange1& range1, const ForwardRange2& range2, BinaryPredicate pred) {
    return adobe::search(boost::begin(range1), boost::end(range1), boost::begin(range2),
                         boost::end(range2), pred);
}

/*!
    \ingroup search

    \brief search implementation
*/
template <class ForwardRange1, class ForwardRange2, class BinaryPredicate>
inline typename boost::range_const_iterator<ForwardRange1>::type
search(const ForwardRange1& range1, const ForwardRange2& range2, BinaryPredicate pred) {
    return adobe::search(boost::begin(range1), boost::end(range1), boost::begin(range2),
                         boost::end(range2), pred);
}

/*!
    \ingroup search

    \brief search implementation
*/
template <class ForwardRange, class Size, class T>
inline typename boost::range_iterator<ForwardRange>::type search_n(ForwardRange& range, Size count,
                                                                   const T& value) {
    return std::search_n(boost::begin(range), boost::end(range), count, value);
}

/*!
    \ingroup search

    \brief search implementation
*/
template <class ForwardRange, class Size, class T>
inline typename boost::range_const_iterator<ForwardRange>::type
search_n(const ForwardRange& range, Size count, const T& value) {
    return std::search_n(boost::begin(range), boost::end(range), count, value);
}

/*!
    \ingroup search

    \brief search implementation
*/
template <class ForwardIterator, class Size, class T, class BinaryPredicate>
inline ForwardIterator search_n(ForwardIterator first, ForwardIterator last, Size count,
                                const T& value, BinaryPredicate pred) {
    return std::search_n(first, last, count, value,
                         std::bind(pred, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup search

    \brief search implementation
*/
template <class ForwardRange, class Size, class T, class BinaryPredicate>
inline typename boost::range_iterator<ForwardRange>::type
search_n(ForwardRange& range, Size count, const T& value, BinaryPredicate pred) {
    return adobe::search_n(boost::begin(range), boost::end(range), count, value, pred);
}

/*!
    \ingroup search

    \brief search implementation
*/
template <class ForwardRange, class Size, class T, class BinaryPredicate>
inline typename boost::range_const_iterator<ForwardRange>::type
search_n(const ForwardRange& range, Size count, const T& value, BinaryPredicate pred) {
    return adobe::search_n(boost::begin(range), boost::end(range), count, value, pred);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
