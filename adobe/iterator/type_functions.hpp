/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_ITERATOR_TYPE_FUNCTIONS_HPP
#define ADOBE_ITERATOR_TYPE_FUNCTIONS_HPP

#include <adobe/config.hpp>

#include <iterator>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

/*! \addtogroup adobe_iterator
@{
*/
#define ADOBE_ITERATOR_TYPE_FUNCTION_BOILERPLATE(type_name)                                        \
    template <typename I> /*I models InputIterator*/ struct type_name {                            \
        typedef typename std::iterator_traits<I>::type_name type;                                  \
    };

ADOBE_ITERATOR_TYPE_FUNCTION_BOILERPLATE(difference_type)
ADOBE_ITERATOR_TYPE_FUNCTION_BOILERPLATE(iterator_category)
ADOBE_ITERATOR_TYPE_FUNCTION_BOILERPLATE(value_type)

#define ADOBE_DIFFERENCE_TYPE(I) typename adobe::difference_type<I>::type
#define ADOBE_ITERATOR_CATEGORY(I) typename adobe::iterator_category<I>::type
#define ADOBE_VALUE_TYPE(I) typename adobe::value_type<I>::type

//!@}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif
// ADOBE_ITERATOR_DISTANCE_HPP
