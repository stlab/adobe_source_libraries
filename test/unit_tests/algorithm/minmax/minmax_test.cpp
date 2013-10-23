/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/algorithm/minmax.hpp>

#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

struct test_t {
    bool before(const test_t&) const { return false; }
};

BOOST_AUTO_TEST_CASE(clamp_test) {
    using adobe::min;
    using adobe::max;

    {
        int a = 1, b = 2;
        BOOST_CHECK_EQUAL(&min(b, a), &a);
        BOOST_CHECK_EQUAL(&min(a, b), &a);
        BOOST_CHECK_EQUAL(&max(b, a), &b);
        BOOST_CHECK_EQUAL(&max(a, b), &b);
    }
    {
        int a = 1, b = 1;
        BOOST_CHECK_EQUAL(&min(a, b), &a);
        BOOST_CHECK_EQUAL(&max(a, b), &b);
    }
    {
        int a = 1, b = 1;
        BOOST_CHECK_EQUAL(&min(a, b), &a);
        BOOST_CHECK_EQUAL(&max(a, b), &b);
    }
    {
        test_t a, b;
        BOOST_CHECK_EQUAL(&min(a, b, &test_t::before), &a);
        BOOST_CHECK_EQUAL(&max(a, b, &test_t::before), &b);
    }
    {
        int a = 1, b = 2;
        min(a, b) = 0;
        max(a, b) = 3;
        BOOST_CHECK_EQUAL(a, 0);
        BOOST_CHECK_EQUAL(b, 3);
    }
}

