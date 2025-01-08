/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/config.hpp>

#include <functional>

#define BOOST_TEST_MAIN
#include <adobe/test/check_less_than_comparable.hpp>
#include <adobe/test/check_null.hpp>
#include <adobe/test/check_regular.hpp>
#include <boost/test/unit_test.hpp>

#include <adobe/name.hpp>

// To do:
// operator << ADOBE_STD_SERIALIZATION ?

BOOST_AUTO_TEST_CASE(name_test) {
    adobe::name_t simple("simple");
    adobe::name_t test("test");
    adobe::name_t cases("cases");

    // regular
    adobe::check_regular(simple);

    // operator<
    adobe::check_less_than_comparable(cases, simple, test, std::less<adobe::name_t>());

    // operator>
    adobe::check_less_than_comparable(test, simple, cases, std::greater<adobe::name_t>());

    // operator !, bool
    adobe::check_null(test);

    // 1 arg ctor, c_str
    BOOST_CHECK_EQUAL(std::strcmp(simple.c_str(), "simple"), 0);

    // is_pod
    BOOST_CHECK_EQUAL(boost::is_pod<adobe::name_t>::value, true);
}
