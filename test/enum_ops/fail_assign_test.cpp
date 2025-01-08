/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/config.hpp>

#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <adobe/enum_ops.hpp>


enum A { min_a = INT_MIN, max_a = INT_MAX };
enum B { min_b = INT_MIN, max_b = INT_MAX };

ADOBE_DEFINE_ARITHMETIC_OPS(A)
ADOBE_DEFINE_ARITHMETIC_OPS(B)

BOOST_AUTO_TEST_CASE(strongly_typed_ints) {
    A a(1);
    B b(1);
    BOOST_CHECK(a == b); // compilation must fail--incompatible int-like types
}
