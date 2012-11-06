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

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/size.hpp>
#include <adobe/functional.hpp>
#include <adobe/algorithm/equal_range.hpp>
#include <adobe/utility/pair.hpp>


void equal_range_test() {
    using std::pair;
    using adobe::less;
    using adobe::equal_range;
    using adobe::equal_range_n;
    using adobe::aggregate_pair;
    using boost::begin;
    using boost::end;
    using boost::size;

    aggregate_pair<int> a[] = { { 0, 0 }, { 0, 1 }, { 1, 2 }, { 1, 3 }, { 3, 4 }, { 3, 5 } };
    const aggregate_pair<int> c[] = { { 0, 0 }, { 0, 1 }, { 1, 2 }, { 1, 3 }, { 3, 4 }, { 3, 5 } };

    BOOST_CHECK_EQUAL(equal_range_n(begin(a), size(a), a[0]).first, &a[0]);
    BOOST_CHECK_EQUAL(equal_range_n(begin(a), size(a), a[1], less()).first, &a[1]);
    BOOST_CHECK_EQUAL(equal_range_n(begin(a), size(a), 1, less(), &aggregate_pair<int>::first).first, &a[2]);

    BOOST_CHECK_EQUAL(equal_range(begin(a), end(a), a[0]).first, &a[0]);
    BOOST_CHECK_EQUAL(adobe::equal_range(begin(a), end(a), a[1], less()).first, &a[1]);
    BOOST_CHECK_EQUAL(equal_range(begin(a), end(a), 1, less(), &aggregate_pair<int>::first).first, &a[2]);

    BOOST_CHECK_EQUAL(equal_range(a, a[2]).first, &a[2]);
    BOOST_CHECK_EQUAL(equal_range(c, c[3]).first, &c[3]);

    BOOST_CHECK_EQUAL(equal_range(a, a[4], less()).first, &a[4]);
    BOOST_CHECK_EQUAL(equal_range(c, c[5], less()).first, &c[5]);

    BOOST_CHECK_EQUAL(equal_range(a, 1, less(), &aggregate_pair<int>::first).first, &a[2]);
    BOOST_CHECK_EQUAL(equal_range(c, 1, less(), &aggregate_pair<int>::first).first, &c[2]);
}


using namespace boost::unit_test;

test_suite*
init_unit_test_suite( int , char* [] ) 
{
    framework::master_test_suite().
        add( BOOST_TEST_CASE( &equal_range_test ) );

    return 0;
}
