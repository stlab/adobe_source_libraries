/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#include <adobe/config.hpp>

#include <functional>
#include <utility>

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <adobe/any_regular.hpp>

/*************************************************************************************************/

BOOST_AUTO_TEST_CASE(any_regular_test) {
    using adobe::any_regular_t;
    using adobe::runtime_cast;

    {
        any_regular_t x(10.0);
        double y = runtime_cast<double&>(x);
        BOOST_CHECK_EQUAL(y, 10.0);
        runtime_cast<double&>(x) = 11.0;
        y = runtime_cast<double&>(x);
        BOOST_CHECK_EQUAL(y, 11.0);

        BOOST_CHECK(runtime_cast(x, y));

        BOOST_CHECK_EQUAL(*runtime_cast<double*>(&x), 11.0);
        BOOST_CHECK(!runtime_cast<char*>(&x));
    }

    {
        const any_regular_t x(10.0);
        double y = runtime_cast<const double&>(x);
        BOOST_CHECK_EQUAL(y, 10.0);
    }

    {
        const any_regular_t x;
        const any_regular_t y(10.0);
        BOOST_CHECK(adobe::empty(x));
        BOOST_CHECK(!adobe::empty(y));
    }

#if 0
    {
    any_regular_t x(1);
    int y = runtime_cast<int&>(x);
    }
#endif
}
