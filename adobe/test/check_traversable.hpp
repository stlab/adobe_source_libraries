/*
	Copyright 2008 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <boost/concept_check.hpp>

#include <adobe/test/check_regular.hpp>
#include <adobe/implementation/swap.hpp>
#include <iostream> 

namespace adobe {
/*!
\ingroup testing
*/
template <typename T>
void check_traversable(const T& c)
{

// http://www.sgi.com/tech/stl/Container.html

    boost::function_requires< boost::ContainerConcept<T> >();
    check_regular(c);
    {
        // some valid expressions
        const T d1(c);
        T d2(c);
        typename T::const_iterator i1=d1.begin(), i2=d1.end(), i3=d2.begin(), i4=d2.end();
        typename T::iterator j1=d2.begin(), j2=d2.end();
    }


    {
        T d(c);
        BOOST_CHECK_MESSAGE(c.size() == d.size(), "container copy-ctor size");
        typename T::const_iterator i=c.begin(), j=d.begin();
    
        BOOST_CHECK_MESSAGE(d==c, "container copy-ctor values");
    }
    
    {
        T d;
        d = c;
        BOOST_CHECK_MESSAGE(c.size() == d.size(), "container assignment copy-ctor size");

    
        BOOST_CHECK_MESSAGE(d==c, "container copy-ctor values");
    }


    {
        T d;
        BOOST_CHECK_MESSAGE(c.max_size() >= c.size() && 0 <= c.size(), "container maximum size");
        BOOST_CHECK_MESSAGE(d.max_size() >= d.size() && 0 <= d.size(), "container maximum size");
        BOOST_CHECK_MESSAGE(c.empty() == (c.size() == 0), "container empty");
        BOOST_CHECK_MESSAGE(d.empty() == (d.size() == 0), "container empty");
    }
    
     
    {
        using std::swap;
        T d1(c);
        T d2(c);
        T d3;
        T d4;
        
        d1.swap(d3);
        swap(d3,d4);
        BOOST_CHECK_MESSAGE(d1 == d3 && d2 == d4, "container swap");
    }

	{

		typename T::iterator x;
		typename T::const_iterator y;
		
		y = x; // Make sure mutable iterator can be converted to const.
		
		y == x; // Make sure const/mutable iterators can be compared.
		x == y;

	}
    


}

#if 0
//gcc always instantiates this
BOOST_TEST_CASE_TEMPLATE_FUNCTION(check_traversables, T)
{
    check_traversable(arbitrary_traversable<T>());
}
#endif

}

