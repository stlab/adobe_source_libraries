/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <boost/test/unit_test.hpp>

#include <iterator>
#include <vector>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <adobe/memory.hpp>
#include <adobe/move.hpp>
#include <adobe/vector.hpp>

/*************************************************************************************************/

void move_test()
{
    typedef adobe::vector<int>          vector_t;
    typedef adobe::vector<vector_t>     vector_vector_t;
    typedef std::vector<int>            std_vector_t;
    typedef adobe::vector<std_vector_t> vector_std_vector_t;

    // Test is_movable<> template function

    BOOST_CHECK(!adobe::is_movable<std_vector_t>::value);
    BOOST_CHECK(adobe::is_movable<vector_t>::value);

    { // Test move via push_back
    vector_t x(3, 4);
    x.push_back(7);
    int* addr = x.begin();

    vector_vector_t y;
    y.push_back(adobe::move(x));

    BOOST_CHECK_EQUAL(addr, &y[0][0]);
    }

    { // Test move with a copy only type
    std_vector_t x(3, 4);
    vector_std_vector_t y;
    y.push_back(adobe::move(x)); // really copies
    }

    { // Test move algorithm and back_mover
        vector_t x(3, 4);
        x.push_back(7);

        vector_vector_t y;
        y.push_back(x);
        y.push_back(x);
        y.push_back(x);

        vector_vector_t z;
        int* addr = &y.back().back();
        adobe::move(y, adobe::back_mover(z));
        BOOST_CHECK_EQUAL(addr, &z.back().back());
    }

    { // Test move_backward
        vector_t x(3, 4);
        x.push_back(7);

        vector_vector_t y;
        y.push_back(x);
        y.push_back(x);
        y.push_back(x);

        vector_vector_t z(y);
        int* addr = &y.back().back();
        adobe::move_backward(y, z.end());
        BOOST_CHECK_EQUAL(addr, &z.back().back());
    }

    { // Test move_construct
        vector_t x(3, 4);
        vector_t y(x);
        x.~vector_t();
        int* addr = y.begin();
        adobe::construct(&x, adobe::move(y));
        BOOST_CHECK_EQUAL(addr, x.begin());
    }

    { // Test uninitialized_move
        vector_t x[3];
        vector_t y[3];
        y[0].push_back(7);

        adobe::destroy(&x[0], &x[3]);

        int* addr = y[0].begin();
        adobe::uninitialized_move(&y[0], &y[3], &x[0]);
        BOOST_CHECK_EQUAL(addr, x[0].begin());
    }
}


using namespace boost::unit_test;

test_suite*
init_unit_test_suite( int , char* [] )
{
    framework::master_test_suite().
        add( BOOST_TEST_CASE( &move_test ) );

    return 0;
}

