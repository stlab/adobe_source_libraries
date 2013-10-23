/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <stdexcept>
#include <adobe/algorithm/clamp.hpp>

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
