/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/config.hpp>

#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <adobe/name.hpp>
#include <adobe/utility/ignore_unused.hpp>

BOOST_AUTO_TEST_CASE(name_int) {
    adobe::name_t not_an_int("not_an_int");
    int i = not_an_int;
    adobe::ignore_unused(i);
}
