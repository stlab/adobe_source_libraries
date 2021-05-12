/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef ADOBE_IS_RANGE_HPP
#define ADOBE_IS_RANGE_HPP

#include <adobe/type_inspection.hpp>

namespace adobe {

/*!
\ingroup inspection

\brief does T model the boost::range concepts?
*/
template <typename T>
struct is_range {
    static const bool value = has_type_type<boost::range_iterator<T>>::value;
};
} // namespace adobe

#endif // include guard
