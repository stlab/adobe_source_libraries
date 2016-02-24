/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#define BOOST_TEST_MAIN


#include <list>
#include <set>
#include <vector>
#include <deque>
#include <map>
#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/test/results_collector.hpp>
#include <boost/mpl/list.hpp>
#include <boost/bind.hpp>


#include <boost/bind.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/size.hpp>
#include <boost/test/utils/nullstream.hpp>


#include <adobe/algorithm/erase_if.hpp>
#include <adobe/algorithm/find.hpp>
#include <adobe/functional.hpp>
#include <adobe/utility/pair.hpp>

/*************************************************************************************************/

typedef boost::mpl::list<std::vector<int>, std::list<int>, std::set<int>, std::deque<int>,
	std::multiset<int>> test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(test_erase_if, Container, test_types) {
    using namespace adobe;
    using namespace boost;
    {
        const int a[] = {9, 2, 4, 4, 7, 3, 5};
        Container x(begin(a), end(a));

        erase_if(x, bind(adobe::equal_to(), 4, _1));
        // REVISIT(mmarcus) : BOOST_SIZE_ISSUE replaced boost::size with
        // distance(begin, end) Until figure out how to work around boost
        // 1.35.0 apparently buggy size, begin, end return type calculation
        // metafunction. maybe right solution is to entirely replace
        // begin/end/size, given the as_literal issue (search
        // BOOST_AS_LITERAL_ISSUE)
        BOOST_CHECK(distance(boost::begin(x), boost::end(x)) == 5);
        BOOST_CHECK(find(x, 4) == boost::end(x));
    }
}

typedef boost::mpl::list<std::map<int, int>, std::multimap<int, int>> test_key_value_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(test_key_value_erase_if, Container, test_key_value_types) {
    using namespace adobe;
    using namespace boost;
    {
        typedef typename Container::value_type value_type;

        const value_type a[] = {value_type(9, 0), value_type(2, 1), value_type(4, 2),
                                value_type(4, 2), value_type(7, 4), value_type(3, 5),
                                value_type(5, 6)};
        Container x(begin(a), end(a));

        erase_if(x, boost::bind(adobe::equal_to(), typename Container::value_type(4, 2), _1));
        // REVISIT(mmarcus) : BOOST_SIZE_ISSUE replaced boost::size with
        // distance(begin, end) Until figure out how to work around boost
        // 1.35.0 apparently buggy size, begin, end return type calculation
        // metafunction. maybe right solution is to entirely replace
        // begin/end/size, given the as_literal issue (search
        // BOOST_AS_LITERAL_ISSUE)
        BOOST_CHECK(distance(boost::begin(x), boost::end(x)) == 5);
        BOOST_CHECK(find(x, typename Container::value_type(4, 2)) == boost::end(x));
    }
}
