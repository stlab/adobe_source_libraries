/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_FILTER_HPP
#define ADOBE_ALGORITHM_FILTER_HPP

#include <adobe/config.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/bind.hpp>

#include <algorithm>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup filter filter
\ingroup mutating_algorithm

Filter is similar to \c std::tranform. The \c op function is passed an instance of an \ref
stldoc_output_iterator and can generate more than (or fewer than) a single result.

The \c op function is called once for each item in the range <code>[first, last)</code>. The \c
result parameter is passed into the first call to \c op, the result of \c op is passed to subsequent
calls to \c op.

\see std::transform, std::remove

\requirements
    - \em I models \ref stldoc_input_iterator.
    - \em O models \ref stldoc_output_iterator.
    - \em F models \ref concept_convertible_to_function with signature O op(value_type(I), O)
    - \em <code>[first, last)</code> is a valid range.

\result
Result of last invocation of \c op or \c result if <code>[first, last)</code> is empty.

\complexity
Linear. Exactly <code>distance(first, last)</code> calls to \c op.
*/
/*************************************************************************************************/
#ifndef ADOBE_NO_DOCUMENTATION
namespace implementation {

/*************************************************************************************************/

template <typename I, // I models InputIterator
          typename O, // O models OutputIterator
          typename F> // F is a function type of the form O F()(value_type(I), O)
O filter(I first, I last, O result, F op)
{
    while (first != last)
    {
        result = op(*first, result);
        ++first;
    }
    return result;
}

/*************************************************************************************************/

} // namespace implementation
#endif
/*************************************************************************************************/
/*!
    \ingroup filter

    \brief filter implementation
*/
template <typename I, // I models InputIterator
          typename O, // O models OutputIterator
          typename F> // F is a function type of the form O F()(value_type(I), O)
inline O filter(I first, I last, O result, F op)
{
    return implementation::filter(first, last, result, boost::bind<O>(op, _1, _2));
}

/*!
    \ingroup filter

    \brief filter implementation
*/
template <typename I, // I models InputRange
          typename O, // O models OutputIterator
          typename F> // F is a function type of the form O F()(value_type(I), O)
O filter(I& source, O result, F op)
{
    return adobe::filter(boost::begin(source), boost::end(source), result, op);
}

/*!
    \ingroup filter

    \brief filter implementation
*/
template <typename I, // I models InputRange
          typename O, // O models OutputIterator
          typename F> // F is a function type of the form O F()(value_type(I), O)
O filter(const I& source, O result, F op)
{
    return adobe::filter(boost::begin(source), boost::end(source), result, op);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
