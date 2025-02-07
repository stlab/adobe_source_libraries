/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/algorithm/median.hpp>

#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(median_test) {
    using adobe::median;

    {
        int a = 1, b = 2, c = 3;
        BOOST_CHECK_EQUAL(&median(a, b, c), &b);
        BOOST_CHECK_EQUAL(&median(a, c, b), &b);
        BOOST_CHECK_EQUAL(&median(b, a, c), &b);
        BOOST_CHECK_EQUAL(&median(b, c, a), &b);
        BOOST_CHECK_EQUAL(&median(c, a, b), &b);
        BOOST_CHECK_EQUAL(&median(c, b, a), &b);
    }
    {
        int a = 1, b = 1, c = 3;
        BOOST_CHECK_EQUAL(&median(a, b, c), &b);
        BOOST_CHECK_EQUAL(&median(a, c, b), &b);
        BOOST_CHECK_EQUAL(&median(c, a, b), &b);
    }
    {
        int a = 1, b = 3, c = 3;
        BOOST_CHECK_EQUAL(&median(a, b, c), &b);
        BOOST_CHECK_EQUAL(&median(b, a, c), &b);
        BOOST_CHECK_EQUAL(&median(b, c, a), &b);
    }
    {
        int a = 1, b = 1, c = 1;
        BOOST_CHECK_EQUAL(&median(a, b, c), &b);
    }

    {
        int a = 1, c = 3;
        const int b = 2;
        BOOST_CHECK_EQUAL(&median(a, b, c), &b);
        BOOST_CHECK_EQUAL(&median(a, c, b), &b);
        BOOST_CHECK_EQUAL(&median(b, a, c), &b);
        BOOST_CHECK_EQUAL(&median(b, c, a), &b);
        BOOST_CHECK_EQUAL(&median(c, a, b), &b);
        BOOST_CHECK_EQUAL(&median(c, b, a), &b);
    }
}
