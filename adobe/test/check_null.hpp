/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/config.hpp>
#include <boost/test/unit_test.hpp>

namespace adobe {
/*!
\addtogroup testing
@{
*/

template <typename T>
void check_null(const T& x) {
    T t = T();
    if (t)
        BOOST_ERROR("operator bool");
    BOOST_CHECK_MESSAGE(!t, "operator!");

    BOOST_CHECK_MESSAGE(x, "operator!");
    if (!x)
        BOOST_ERROR("operator bool");
}
//! @}
} // namespace adobe
