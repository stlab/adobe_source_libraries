/*
	Copyright 2008 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <algorithm>

#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include <adobe/forest.hpp>
#include <adobe/test/check_traversable.hpp>


namespace adobe {


BOOST_AUTO_TEST_CASE(forest_int_traversable_test) {
    forest<int> f;
	f.push_back(1);
	f.push_back(2);
	f.push_back(3);
	f.push_back(42);
     
    check_traversable(f);
}


BOOST_AUTO_TEST_CASE(forest_int_iterator_test) {
    forest<int> f;
	f.push_back(1);
	f.push_back(2);
	f.push_back(3);
	f.push_back(42);
	forest<int>::iterator begin=f.begin(), end=f.end();
	boost::function_requires< boost::BidirectionalIteratorConcept<forest<int>::iterator> >();
    boost::function_requires< boost::BidirectionalIteratorConcept<forest<int>::const_iterator> >(); 
	boost::function_requires< boost::BidirectionalIteratorConcept<forest<int>::preorder_iterator> >();
    boost::function_requires< boost::BidirectionalIteratorConcept<forest<int>::const_preorder_iterator> >(); 
	boost::function_requires< boost::BidirectionalIteratorConcept<forest<int>::postorder_iterator> >();
    boost::function_requires< boost::BidirectionalIteratorConcept<forest<int>::const_postorder_iterator> >();     
	boost::function_requires< boost::BidirectionalIteratorConcept<forest<int>::child_iterator> >();
    boost::function_requires< boost::BidirectionalIteratorConcept<forest<int>::const_child_iterator> >(); 
    boost::function_requires< boost::BidirectionalIteratorConcept<forest<int>::reverse_child_iterator> >(); 	
	boost::function_requires< boost::BidirectionalIteratorConcept<forest<int>::reverse_iterator> >();
    boost::function_requires< boost::BidirectionalIteratorConcept<forest<int>::const_reverse_iterator> >();     

}


}
