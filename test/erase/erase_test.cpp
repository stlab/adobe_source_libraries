/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)
*/

/**************************************************************************************************/

#define BOOST_TEST_MAIN


#include <deque>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <vector>

#include <boost/mpl/list.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/size.hpp>
#include <boost/test/results_collector.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/utils/nullstream.hpp>

#include <adobe/algorithm/erase_if.hpp>
#include <adobe/algorithm/find.hpp>
#include <adobe/functional.hpp>
#include <adobe/utility/pair.hpp>

/**************************************************************************************************/

using namespace std::placeholders;

/**************************************************************************************************/

typedef boost::mpl::list<std::vector<int>, std::list<int>, std::set<int>, std::deque<int>,
                         std::multiset<int>>
    test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(test_erase_if, Container, test_types) {
    using namespace adobe;
    using namespace std;
    {
        const int a[] = {9, 2, 4, 4, 7, 3, 5};
        Container x(begin(a), end(a));

        erase_if(x, std::bind(adobe::equal_to(), 4, _1));
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
    using namespace std;
    {
        typedef typename Container::value_type value_type;

        const value_type a[] = {value_type(9, 0), value_type(2, 1), value_type(4, 2),
                                value_type(4, 2), value_type(7, 4), value_type(3, 5),
                                value_type(5, 6)};
        Container x(begin(a), end(a));

        erase_if(x, std::bind(adobe::equal_to(), typename Container::value_type(4, 2), _1));
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
