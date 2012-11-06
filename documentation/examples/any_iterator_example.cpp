/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

// start_of_example

#include <iostream>
#include <list>
#include <vector>
#include <deque>

#include <boost/range/functions.hpp>

#include <adobe/iterator.hpp>

typedef adobe::any_iterator<int, std::bidirectional_iterator_tag> any_iterator_to_int;

// Concrete (non-template) function
void reverse_and_print(any_iterator_to_int first, any_iterator_to_int last)
{
    std::reverse(first, last);
    while (first != last)
    {
        std::cout << *first << " ";
        ++first;
    }
    std::cout << std::endl;
}

int main()
{
    const int a[] = { 0, 1, 2, 3, 4, 5 };
    
    std::list<int> l(boost::begin(a), boost::end(a));
    reverse_and_print(any_iterator_to_int(boost::begin(l)), any_iterator_to_int(boost::end(l)));
    
    std::vector<int> v(boost::begin(a), boost::end(a));
    reverse_and_print(any_iterator_to_int(boost::begin(v)), any_iterator_to_int(boost::end(v)));
    
    std::deque<int> d(boost::begin(a), boost::end(a));
    reverse_and_print(any_iterator_to_int(boost::begin(d)), any_iterator_to_int(boost::end(d)));
    
    return 0;
}

// end_of_example
