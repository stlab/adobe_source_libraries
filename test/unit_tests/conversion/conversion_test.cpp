/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <functional>
#include <utility>

#include <boost/test/unit_test.hpp>

#include <adobe/conversion.hpp>

/*************************************************************************************************/

namespace {

struct base { virtual ~base() { } };
struct derived : base { };
struct not_derived { };
struct other_derived : base { };

} // namespace


void conversion_test() {
    using adobe::runtime_cast;

    {
    base* x = new derived;
    BOOST_CHECK(runtime_cast<derived*>(x));
    BOOST_CHECK(runtime_cast<const derived*>(x));
    // BOOST_CHECK(runtime_cast<not_derived*>(x));

    BOOST_CHECK(runtime_cast<other_derived*>(x) == 0);

    runtime_cast<derived&>(*x) = derived();
    }

    {
    const base* x = new derived;
   // BOOST_CHECK(runtime_cast<derived*>(x));
    BOOST_CHECK(runtime_cast<const derived*>(x));
    derived y = runtime_cast<const derived&>(*x);
    }
}

using namespace boost::unit_test;

test_suite*
init_unit_test_suite( int , char* [] ) 
{
    framework::master_test_suite().
        add( BOOST_TEST_CASE( &conversion_test ) );

    return 0;
}
