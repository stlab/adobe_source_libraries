/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

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
