/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <functional>

#include <boost/test/unit_test.hpp>
#include <adobe/test/check_regular.hpp>
#include <adobe/test/check_less_than_comparable.hpp>
#include <adobe/test/check_null.hpp>

#include <adobe/name.hpp>

//To do:
// operator << ADOBE_STD_SERIALIZATION ?

void name_test() {
    adobe::name_t simple("simple");
    adobe::name_t test("test");
    adobe::name_t cases("cases");
    
    //regular
    adobe::check_regular(simple);

    //operator<
    adobe::check_less_than_comparable(cases, simple, test, std::less<adobe::name_t>());
    
    //operator>
    adobe::check_less_than_comparable(test, simple, cases, std::greater<adobe::name_t>());

    //operator !, bool
    adobe::check_null(test);

    //1 arg ctor, c_str
    BOOST_CHECK_EQUAL(std::strcmp(simple.c_str(), "simple"),0);
    
    //is_pod
    BOOST_CHECK_EQUAL(boost::is_pod<adobe::name_t>::value, true);
}

using namespace boost::unit_test;

test_suite*
init_unit_test_suite( int , char* [] ) 
{
    framework::master_test_suite().
        add( BOOST_TEST_CASE( &name_test ) );

    return 0;
}
