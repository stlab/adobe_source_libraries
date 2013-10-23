/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_REDUCE_HPP
#define ADOBE_ALGORITHM_REDUCE_HPP

#include <adobe/config.hpp>

#include <adobe/algorithm/identity_element.hpp>
#include <adobe/algorithm/other_of.hpp>
#include <adobe/functional.hpp>
#include <adobe/iterator/type_functions.hpp>

#include <algorithm>
#include <functional>
#include <vector>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
    \defgroup reduce reduce
    \ingroup numeric
*/
/*************************************************************************************************/
/*!
    \ingroup reduce

    \brief reduce implementation
*/
template <typename I,  // I models InputIterator 
          typename Op> // Op model BinaryOperation 
typename std::iterator_traits<I>::value_type reduce_nonzeros(I f,
                                                             I l,
                                                             Op op,
                                                             ADOBE_VALUE_TYPE(I) z =
                                                                 adobe::identity_element<Op>()())
{
    // skip zeros
    f = adobe::find_not(f, l, z);

    if (f == l)
        return z;

    ADOBE_VALUE_TYPE(I) result(*f);

    ++f;

    while (f != l)
    {
        if (*f != z)
            result = op(result, *f);

        ++f;
    }

    return result;
}

/*************************************************************************************************/
/*!
    \ingroup reduce

    \brief reduce implementation
*/
template <typename I,  // I models Forward Iterator
          typename Op> // Op models Binary Operation
typename std::iterator_traits<I>::value_type add_to_counter(I                   f,
                                                            I                   l,
                                                            Op                  op,
                                                            ADOBE_VALUE_TYPE(I) x,
                                                            ADOBE_VALUE_TYPE(I) z =
                                                                adobe::identity_element<Op>()())
{
    if (x == z)
        return z;

    while (f != l)
    {
        if (*f != z)
        {
            // NOTE (stepanov) : op(*f, x) and not op(x, *f) because the partial
            //                   result pointed to by f is the result of adding elements
            //                   earlier in the sequence.
            x = op(*f, x);

            *f = z;
        }
        else
        {
            *f = x;

            return z;
        }

        ++f;
    }

    return x;
}

/*************************************************************************************************/
/*!
    \ingroup reduce

    \brief reduce implementation
*/
template <typename I,  // I models InputIterator
          typename Op> // Op models BinaryOperation
typename std::iterator_traits<I>::value_type reduce_balanced(I                   f,
                                                             I                   l,
                                                             Op                  op,
                                                             ADOBE_VALUE_TYPE(I) z =
                                                                 adobe::identity_element<Op>()())
{
    std::vector<ADOBE_VALUE_TYPE(I)> v;

    while (f != l)
    {
        ADOBE_VALUE_TYPE(I) tmp = add_to_counter(v.begin(), v.end(), op, *f, z);

        if (tmp != z)
            v.push_back(tmp);

        ++f;
    }

    return reduce_nonzeros(v.begin(), v.end(), f_transpose(op), z);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
