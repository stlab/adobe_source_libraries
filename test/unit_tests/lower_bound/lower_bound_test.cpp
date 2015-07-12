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

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/size.hpp>
#include <adobe/functional.hpp>
#include <adobe/algorithm/lower_bound.hpp>
#include <adobe/utility/pair.hpp>


BOOST_AUTO_TEST_CASE(lower_bound_test) {
    using std::pair;
    using adobe::less;
    using adobe::lower_bound;
    using adobe::lower_bound_n;
    using adobe::aggregate_pair;
    using boost::begin;
    using boost::end;
    using boost::size;

    aggregate_pair<int> a[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};
    const aggregate_pair<int> c[] = {{0, 0}, {0, 1}, {1, 2}, {1, 3}, {3, 4}, {3, 5}};

    BOOST_CHECK_EQUAL(lower_bound_n(begin(a), size(a), a[0]), &a[0]);
    BOOST_CHECK_EQUAL(lower_bound_n(begin(a), size(a), a[1], less()), &a[1]);
    BOOST_CHECK_EQUAL(lower_bound_n(begin(a), size(a), 1, less(), &aggregate_pair<int>::first),
                      &a[2]);

    BOOST_CHECK_EQUAL(lower_bound(begin(a), end(a), a[0]), &a[0]);
    BOOST_CHECK_EQUAL(adobe::lower_bound(begin(a), end(a), a[1], less()), &a[1]);
    BOOST_CHECK_EQUAL(lower_bound(begin(a), end(a), 1, less(), &aggregate_pair<int>::first), &a[2]);

    BOOST_CHECK_EQUAL(lower_bound(a, a[2]), &a[2]);
    BOOST_CHECK_EQUAL(lower_bound(c, c[3]), &c[3]);

    BOOST_CHECK_EQUAL(lower_bound(a, a[4], less()), &a[4]);
    BOOST_CHECK_EQUAL(lower_bound(c, c[5], less()), &c[5]);

    BOOST_CHECK_EQUAL(lower_bound(a, 1, less(), &aggregate_pair<int>::first), &a[2]);
    BOOST_CHECK_EQUAL(lower_bound(c, 1, less(), &aggregate_pair<int>::first), &c[2]);
}
