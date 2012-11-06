/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)

    Author(s): Sean Parent, Mat Marcus
*/

/*************************************************************************************************/

#include <cassert>
#include <algorithm>
#include <list>
#include <vector>
#include <deque>

#include <boost/range/functions.hpp>

#include <adobe/any_iterator.hpp>

/*************************************************************************************************/

typedef adobe::poly<adobe::bidirectional_iter<int> > any_bd_iterator_to_int;
typedef adobe::poly<adobe::random_access_iter<int> > any_ra_iterator_to_int;

// Concrete (non-template) function
void reverse(any_bd_iterator_to_int first, any_bd_iterator_to_int last)
{
    any_ra_iterator_to_int* first_ra = adobe::poly_cast<any_ra_iterator_to_int*>(&first);
    any_ra_iterator_to_int* last_ra  = adobe::poly_cast<any_ra_iterator_to_int*>(&last);

    if(first_ra && last_ra) 
        std::reverse(*first_ra, *last_ra);
    else 
        std::reverse(first, last);
}

int main()
{
    const int a[] = { 0, 1, 2, 3, 4, 5 };
    
    std::list<int> l(boost::begin(a), boost::end(a));
    reverse(any_bd_iterator_to_int(boost::begin(l)), any_bd_iterator_to_int(boost::end(l)));
    
    std::vector<int> v(boost::begin(a), boost::end(a));
    any_ra_iterator_to_int begin_ra(boost::begin(v));
    any_ra_iterator_to_int end_ra(boost::end(v));
    reverse(any_bd_iterator_to_int(begin_ra), any_bd_iterator_to_int(end_ra));
    
    std::deque<int> d(boost::begin(a), boost::end(a));
    begin_ra = any_ra_iterator_to_int(boost::begin(d));
    end_ra = any_ra_iterator_to_int(boost::end(d));
    reverse(any_bd_iterator_to_int(begin_ra), any_bd_iterator_to_int(end_ra));
    
    any_bd_iterator_to_int i1((int*)NULL); // default constructor
    any_bd_iterator_to_int i2(i1); // copy empty constructor
    assert(i1 == i2);
    i1 = any_bd_iterator_to_int(boost::begin(l)); // assignment to empty
    assert(i1 == any_bd_iterator_to_int(boost::begin(l)));
    i2 = any_bd_iterator_to_int(boost::end(l));
    i1 = i2;
    assert(i1 == i2);
    assert(i1.cast<std::list<int>::iterator>() == boost::end(l));
    
    return 0;
}
