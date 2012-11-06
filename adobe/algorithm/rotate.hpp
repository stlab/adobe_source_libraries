/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_ROTATE_HPP
#define ADOBE_ALGORITHM_ROTATE_HPP

#include <adobe/config.hpp>

#include <adobe/algorithm/reverse.hpp>

#include <algorithm>
#include <iterator>
#include <utility>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
    \defgroup rotate rotate
    \ingroup mutating_algorithm

    A better algorithm than std::rotate because:
        - It returns both m and m' in the order in which they show up in the sequence,
          allowing you to derive whether or not m is before or after m'.
*/
/*************************************************************************************************/
/*!
    \ingroup rotate

    This is the bidirectional optimization for rotate.
*/
template <typename I> // I models Bidirectional Iterator
std::pair<I, I> rotate(I f, I m, I l, std::bidirectional_iterator_tag)
{
    using std::reverse;

    reverse(f, m);

    reverse(m, l);

    std::pair<I, I> p = reverse_until(f, m, l);

    reverse(p.first, p.second);

    return p;
}

/*!
    \ingroup rotate

    \param f first item in the sequence
    \param m the midpoint around which the range will be rotated
    \param l one past the last item in the sequence

    \returns
        m and m' as a pair in the order in which they appear in the range.
*/
template <typename I> // I models Forward Iterator
std::pair<I, I> rotate(I f, I m, I l)
{
    typedef typename std::iterator_traits<I>::iterator_category iterator_category;

    return rotate(f, m, l, iterator_category());
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
