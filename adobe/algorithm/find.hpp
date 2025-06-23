/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_FIND_HPP
#define ADOBE_ALGORITHM_FIND_HPP

#include <adobe/config.hpp>

#include <algorithm>
#include <functional>
#include <utility>

#include <boost/next_prior.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup find find
\ingroup non_mutating_algorithm

\see
    - [STL documentation for find](https://www.boost.org/sgi/stl/find.html)
    - [STL documentation for find_if](https://www.boost.org/sgi/stl/find_if.html)
    - [STL documentation for find_end](https://www.boost.org/sgi/stl/find_end.html)
    - [STL documentation for find_first_of](https://www.boost.org/sgi/stl/find_first_of.html)
    - [STL documentation for adjacent_find](https://www.boost.org/sgi/stl/adjacent_find.html)
*/

/**************************************************************************************************/

/*!
\defgroup find_not find_not
\ingroup find

\c find_not is similar to \c find, except \c find_not will return the first iterator
\c i in the range <code>[first, last)</code> for which \c *i is not equal to \c value. Returns \c
last if no such iterator exists.

\requirements
    - Same as for find_if

\complexity
    - Same as for find_if
*/

/**************************************************************************************************/

/**************************************************************************************************/
/*!
\defgroup find_if_not find_if_not
\ingroup find

\c find_if_not is similar to \c find_if, except \c find_if_not will return the first
iterator \c i in the range <code>[first, last)</code> for which \c pred(*i) is \c false.
Returns \c last if no such iterator exists.

\requirements
    - Same as for find_if

\complexity
    - Same as for find_if
*/
/**************************************************************************************************/
/*!
    \ingroup find_if_not

    \brief find_if_not implementation
*/
template <class InputIterator, class Predicate>
inline InputIterator find_if_not(InputIterator first, InputIterator last, Predicate pred) {
    return std::find_if_not(first, last, std::bind(pred, std::placeholders::_1));
}

/*!
    \ingroup find_if_not

    \brief find_if_not implementation
*/
template <class InputRange, class Predicate>
inline typename boost::range_iterator<InputRange>::type find_if_not(InputRange& range,
                                                                    Predicate pred) {
    return adobe::find_if_not(boost::begin(range), boost::end(range), pred);
}

/*!
    \ingroup find_if_not

    \brief find_if_not implementation
*/
template <class InputRange, class Predicate>
inline typename boost::range_const_iterator<InputRange>::type find_if_not(const InputRange& range,
                                                                          Predicate pred) {
    return adobe::find_if_not(boost::begin(range), boost::end(range), pred);
}

/**************************************************************************************************/

/*!
    \ingroup find_not
*/
template <class InputIterator, class T>
inline InputIterator find_not(InputIterator first, InputIterator last, const T& value) {
    return std::find_if_not(first, last,
                            std::bind(std::equal_to<T>(), value, std::placeholders::_1));
}

/*!
    \ingroup find_not

    \brief find_not implementation
*/
template <class InputRange, class T>
inline typename boost::range_iterator<InputRange>::type find_not(InputRange& range,
                                                                 const T& value) {
    return adobe::find_not(boost::begin(range), boost::end(range), value);
}

/*!
    \ingroup find_not

    \brief find_not implementation
*/
template <class InputRange, class T>
inline typename boost::range_const_iterator<InputRange>::type find_not(const InputRange& range,
                                                                       const T& value) {
    return adobe::find_not(boost::begin(range), boost::end(range), value);
}


/*!
    \ingroup find
    \brief find implementation
*/
template <class InputRange, class T>
inline typename boost::range_iterator<InputRange>::type find(InputRange& range, const T& value) {
    return std::find(boost::begin(range), boost::end(range), value);
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class InputRange, class T>
inline typename boost::range_const_iterator<InputRange>::type find(const InputRange& range,
                                                                   const T& value) {
    return std::find(boost::begin(range), boost::end(range), value);
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class InputIterator, class Predicate>
inline InputIterator find_if(InputIterator first, InputIterator last, Predicate pred) {
    return std::find_if(first, last, std::bind(pred, std::placeholders::_1));
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class InputRange, class Predicate>
inline typename boost::range_iterator<InputRange>::type find_if(InputRange& range, Predicate pred) {
    return adobe::find_if(boost::begin(range), boost::end(range), pred);
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class InputRange, class Predicate>
inline typename boost::range_const_iterator<InputRange>::type find_if(const InputRange& range,
                                                                      Predicate pred) {
    return adobe::find_if(boost::begin(range), boost::end(range), pred);
}

/*!
    \ingroup find
*/

template <typename I, // I models ForwardIterator
          typename T>
// T is value_type(I)
std::pair<I, I> find_range(I f, I l, const T& x) {
    f = std::find(f, l, x);
    if (f != l)
        l = find_not(boost::next(f), l, x);
    return std::make_pair(f, l);
}

/*!
    \ingroup find
*/

template <typename I, // I models ForwardIterator
          typename P>
// P models UnaryPredicate(value_type(I))
std::pair<I, I> find_range_if(I f, I l, P p) {
    f = adobe::find_if(f, l, p);
    if (f != l)
        l = adobe::find_if_not(boost::next(f), l, p);
    return std::make_pair(f, l);
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class ForwardRange1, class ForwardRange2>
inline typename boost::range_iterator<ForwardRange1>::type find_end(ForwardRange1& range1,
                                                                    const ForwardRange2& range2) {
    return std::find_end(boost::begin(range1), boost::end(range1), boost::begin(range2),
                         boost::end(range2));
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class ForwardRange1, class ForwardRange2>
inline typename boost::range_const_iterator<ForwardRange1>::type
find_end(const ForwardRange1& range1, const ForwardRange2& range2) {
    return std::find_end(boost::begin(range1), boost::end(range1), boost::begin(range2),
                         boost::end(range2));
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class ForwardIterator1, class ForwardIterator2, class BinaryPredicate>
inline ForwardIterator1 find_end(ForwardIterator1 first1, ForwardIterator1 last1,
                                 ForwardIterator2 first2, ForwardIterator2 last2,
                                 BinaryPredicate comp) {
    return std::find_end(first1, last1, first2, last2,
                         std::bind(comp, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class ForwardRange1, class ForwardRange2, class BinaryPredicate>
inline typename boost::range_iterator<ForwardRange1>::type
find_end(ForwardRange1& range1, const ForwardRange2& range2, BinaryPredicate comp) {
    return adobe::find_end(boost::begin(range1), boost::end(range1), boost::begin(range2),
                           boost::end(range2), comp);
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class ForwardRange1, class ForwardRange2, class BinaryPredicate>
inline typename boost::range_const_iterator<ForwardRange1>::type
find_end(const ForwardRange1& range1, const ForwardRange2& range2, BinaryPredicate comp) {
    return adobe::find_end(boost::begin(range1), boost::end(range1), boost::begin(range2),
                           boost::end(range2), comp);
}

#if 0

// find_first_of is a bad algorithm. Use find_first_of_set until we provide a better predicate.

/*!
    \ingroup find

    \brief find implementation
*/
template <class InputRange, class ForwardRange>
inline typename boost::range_iterator<InputRange>::type
find_first_of(InputRange& range1, const ForwardRange& range2)
{
    return std::find_first_of(boost::begin(range1), boost::end(range1),
                              boost::begin(range2), boost::end(range2));
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class InputRange, class ForwardRange>
inline typename boost::range_const_iterator<InputRange>::type
find_first_of(const InputRange& range1, const ForwardRange& range2)
{
    return std::find_first_of(boost::begin(range1), boost::end(range1),
                              boost::begin(range2), boost::end(range2));
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class InputIterator, class ForwardIterator, class BinaryPredicate>
inline InputIterator find_first_of(InputIterator first1, InputIterator last1,
                                   ForwardIterator first2, ForwardIterator last2,
                                   BinaryPredicate comp)

{
    return std::find_first_of(first1, last1, first2, last2, std::bind(comp, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class InputRange, class ForwardRange, class BinaryPredicate>
inline typename boost::range_iterator<InputRange>::type
find_first_of(InputRange& range1, const ForwardRange& range2, BinaryPredicate comp)
{
    return adobe::find_first_of(boost::begin(range1), boost::end(range1),
                                boost::begin(range2), boost::end(range2),
                                comp);
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class InputRange, class ForwardRange, class BinaryPredicate>
inline typename boost::range_const_iterator<InputRange>::type
find_first_of(const InputRange& range1, const ForwardRange& range2, BinaryPredicate comp)
{
    return adobe::find_first_of(boost::begin(range1), boost::end(range1),
                                boost::begin(range2), boost::end(range2),
                                comp);
}

#endif

/*!
    \ingroup find

    \brief find implementation
*/
template <class ForwardRange>
inline typename boost::range_iterator<ForwardRange>::type adjacent_find(ForwardRange& range) {
    return std::adjacent_find(boost::begin(range), boost::end(range));
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class ForwardRange>
inline typename boost::range_const_iterator<ForwardRange>::type
adjacent_find(const ForwardRange& range) {
    return std::adjacent_find(boost::begin(range), boost::end(range));
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class ForwardIterator, class BinaryPredicate>
inline ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last,
                                     BinaryPredicate pred) {
    return std::adjacent_find(first, last,
                              std::bind(pred, std::placeholders::_1, std::placeholders::_2));
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class ForwardRange, class BinaryPredicate>
inline typename boost::range_iterator<ForwardRange>::type adjacent_find(ForwardRange& range,
                                                                        BinaryPredicate pred) {
    return adobe::adjacent_find(boost::begin(range), boost::end(range), pred);
}

/*!
    \ingroup find

    \brief find implementation
*/
template <class ForwardRange, class BinaryPredicate>
inline typename boost::range_const_iterator<ForwardRange>::type
adjacent_find(const ForwardRange& range, BinaryPredicate pred) {
    return adobe::adjacent_find(boost::begin(range), boost::end(range), pred);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
