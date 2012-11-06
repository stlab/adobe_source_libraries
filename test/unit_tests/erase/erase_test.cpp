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
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/size.hpp>
#include <boost/test/utils/nullstream.hpp>


#include <adobe/algorithm/erase_if.hpp>
#include <adobe/algorithm/find.hpp>
#include <adobe/functional.hpp>
#include <adobe/utility/pair.hpp>

/*************************************************************************************************/

BOOST_TEST_CASE_TEMPLATE_FUNCTION(test_erase_if, Container)
{
    using namespace adobe;
    using namespace boost;
    {
    const int a[] = { 9, 2, 4, 4, 7, 3, 5 };
    Container x(begin(a), end(a));
    
    erase_if(x, bind(adobe::equal_to(), 4, _1));
//REVISIT(mmarcus) : BOOST_SIZE_ISSUE replaced boost::size with
//distance(begin, end) Until figure out how to work around boost
//1.35.0 apparently buggy size, begin, end return type calculation
//metafunction. maybe right solution is to entirely replace
//begin/end/size, given the as_literal issue (search
//BOOST_AS_LITERAL_ISSUE)
    BOOST_CHECK(distance(boost::begin(x),boost::end(x)) == 5);
    BOOST_CHECK(find(x, 4) == boost::end(x));
    }
}

BOOST_TEST_CASE_TEMPLATE_FUNCTION(test_key_value_erase_if, Container)
{
    using namespace adobe;
    using namespace boost;
    {
    typedef typename Container::value_type value_type;
    
    const value_type a[] =
        { value_type( 9, 0 ), value_type( 2, 1 ), value_type( 4, 2 ), value_type( 4, 2 ),
            value_type( 7, 4 ), value_type( 3, 5 ), value_type( 5, 6 ) };
    Container x(begin(a), end(a));
    
    erase_if(x, boost::bind(adobe::equal_to(), typename Container::value_type(4, 2), _1));
//REVISIT(mmarcus) : BOOST_SIZE_ISSUE replaced boost::size with
//distance(begin, end) Until figure out how to work around boost
//1.35.0 apparently buggy size, begin, end return type calculation
//metafunction. maybe right solution is to entirely replace
//begin/end/size, given the as_literal issue (search
//BOOST_AS_LITERAL_ISSUE)
    BOOST_CHECK(distance(boost::begin(x), boost::end(x)) == 5);
    BOOST_CHECK(find(x, typename Container::value_type(4, 2)) == boost::end(x));
    }
}

/* We wrap the call to test->add inside of a BOOST_AUTO_TEST_CASE
   so that user can add other simple testcases without having to manage 
   the init routine manually. It's still not clear which is the lesser
   of the two evils: bolierplate here, or init routine management.
   It may become more clear with further study of boost test, or with
   experience in this style of unit testing.
*/

BOOST_AUTO_TEST_CASE(test_erase_ifs)
{
//BOILERPLATE
    namespace ut=boost::unit_test;
    boost::onullstream    null_output;
    ut::unit_test_log.set_stream(null_output);
    ut::test_suite* test = BOOST_TEST_SUITE("");
//END BOILERPLATE

// CUSTOM CODE
    typedef boost::mpl::list< 
                    std::vector<int>, 
                    std::list<int>, 
                    std::set<int>,
                    std::deque<int>,
                    std::multiset<int>
            > test_types;
            
    typedef boost::mpl::list< 
                    std::map<int, int>, 
                    std::multimap<int, int>
            > test_key_value_types;
    test->add(BOOST_TEST_CASE_TEMPLATE(test_erase_if, test_types));
    test->add(BOOST_TEST_CASE_TEMPLATE(test_key_value_erase_if, test_key_value_types));
// END CUSTOM CODE

//BOILERPLATE
    ut::framework::run(test);
    ut::test_results const& tr = ut::results_collector.results(test->p_id);

    ut::unit_test_log.set_stream(std::cout);
    BOOST_CHECK_EQUAL(tr.p_assertions_failed, (std::size_t)0);
    BOOST_CHECK(!tr.p_aborted);
//END BOILERPLATE
}
