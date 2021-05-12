/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/config.hpp>
#include <adobe/implementation/swap.hpp>
#include <boost/test/unit_test.hpp>

namespace adobe {
/*!
\addtogroup testing
@{
*/

template <typename T>
T arbitrary_regular_value();


template <typename T>
void check_regular(const T& x) {
    using std::swap;

    BOOST_CHECK(x != T());
    T y = x;
    BOOST_CHECK_MESSAGE(x == y, "copy-ctor");
    T z = T();
    BOOST_CHECK_MESSAGE(z != x, "default-ctor");
    z = y;
    BOOST_CHECK_MESSAGE(x == z, "assignment");
    T w = T();
    swap(y, w);
    BOOST_CHECK(x == w && x != y && y == T());
}


BOOST_TEST_CASE_TEMPLATE_FUNCTION(check_regulars, T) {
    check_regular(arbitrary_regular_value<T>());
}
} // namespace adobe

//! @}
