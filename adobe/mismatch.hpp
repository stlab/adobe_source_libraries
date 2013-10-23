/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_MISMATCH_HPP
#define ADOBE_MISMATCH_HPP

#include <adobe/config.hpp>

#include <iterator>
#include <utility>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

namespace implementation {

/*************************************************************************************************/

template <typename C1, // Category of I1
          typename C2> // Category of I2
struct mismatch_t;

/*************************************************************************************************/

} // namespace implementation

/*************************************************************************************************/

/*
    REVISIT (sparent) : With mismatch_n() we are throwing away the resulting value of n - this is
    only because it complciates using it in the implementation of mismatch() but I'm not
    convinced that the interface to mismatch() is correct either.
*/

template <typename I1,  // I1 models InputIterator
          typename I2,  // I2 models InputIterator
          typename N>   // N models Integer
std::pair<I1, I2> mismatch_n(I1 first1, I2 first2, N n)
{
    while (n && *first1 == *first2)
    {
        --n;
        ++first1;
        ++first2;
    }
    
    return std::make_pair(first1, first2);
}

template <typename I1, // I1 models InputIterator
          typename I2> // I2 models InputIterator
std::pair<I1, I2> mismatch(I1 first1, I1 last1, I2 first2, I2 last2)
{
    return implementation::mismatch_t<typename std::iterator_traits<I1>::iterator_category,
        typename std::iterator_traits<I2>::iterator_category>()(first1, last1, first2, last2);
}

/*************************************************************************************************/

namespace implementation {

/*************************************************************************************************/

template <typename C1, // Category of I1
          typename C2> // Category of I2
struct mismatch_t
{
    template <typename I1, // I1 models InputIterator
              typename I2> // I2 models InputIterator
    std::pair<I1, I2> operator()(I1 first1, I1 last1, I2 first2, I2 last2) const
    {
        while (first1 != last1 && first2 != last2 && *first1 == * first2)
        {
            ++first1;
            ++first2;
        }
        return std::make_pair(first1, first2);
    }
};

template <>
struct mismatch_t <std::random_access_iterator_tag, std::random_access_iterator_tag>
{
    template <typename I1, // I1 models RandomAccessIterator
              typename I2> // I2 models RandomAccessIterator
    std::pair<I1, I2> operator()(I1 first1, I1 last1, I2 first2, I2 last2) const
    {
        if ((last1 - first1) < (last2 - first2))
        {
            return adobe::mismatch_n(first1, first2, last1 - first1);
        }

        return adobe::mismatch_n(first1, first2, last2 - first2);
    }
};

/*************************************************************************************************/

} // namespace implementation

} // namespace adobe

/*************************************************************************************************/

#endif
// ADOBE_MISMATCH_HPP

/*************************************************************************************************/
