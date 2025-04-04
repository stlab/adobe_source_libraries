/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/algorithm/clamp.hpp>
#include <stdexcept>

#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(clamp_test) {
    using adobe::clamp;
    using adobe::clamp_unordered;

    {
        int a = 1, b = 2, c = 3;
        BOOST_CHECK_EQUAL(&clamp(b, a, c), &b);
        BOOST_CHECK_EQUAL(&clamp(a, b, c), &b);
        BOOST_CHECK_EQUAL(&clamp(c, a, b), &b);
    }
    {
        int a = 1, b = 1, c = 3;
        BOOST_CHECK_EQUAL(&clamp(b, a, c), &b);
        BOOST_CHECK_EQUAL(&clamp(c, a, b), &b);
    }
    {
        int a = 1, b = 3, c = 3;
        BOOST_CHECK_EQUAL(&clamp(b, a, c), &b);
        BOOST_CHECK_EQUAL(&clamp(a, b, c), &b);
    }
    {
        int a = 1, b = 1, c = 1;
        BOOST_CHECK_EQUAL(&clamp(b, a, c), &b);
    }

    {
        int a = 1, b = 1, c = 1;
        BOOST_CHECK_EQUAL(&clamp_unordered(b, a, c), &b);
        /* clamp_unordered fully tested by median. This verifies correct
            order of arguments to median */
    }
}
