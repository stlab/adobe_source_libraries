/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <boost/test/unit_test.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <adobe/string.hpp>
#include <adobe/utility/ignore_unused.hpp>
#include <cassert>
#include <iterator>

/*************************************************************************************************/

namespace adobe {

// Precondition: x != T()

template <typename T>
void test_regular(const T& x)
{
    assert(x != T());
    T y = x; // Copy-construct
    BOOST_CHECK(x == y);
    T z;     // Default-construcct
    BOOST_CHECK(z != x);
    z = y;   // Assignment
    BOOST_CHECK(x == z);
    T w;
    swap(y, w);
    BOOST_CHECK(x == w && x != y && y == T());
}

/*
    Precondition: x != T(), remote_address(T) is defined.
*/

template <typename T>
void test_movable(const T& x)
{
    assert(x != T());
    assert(is_movable<T>::value);
    
    test_regular(x);
    
    T y = x;
    // move construction (and RVO)
    const void* addr = remote_address(y);
    T z = adobe::move(y);
    BOOST_CHECK(y == T());
    BOOST_CHECK(z == x);
    BOOST_CHECK(remote_address(z) == addr);
    // move assignment
    y = adobe::move(z);
    BOOST_CHECK(z == T());
    BOOST_CHECK(y == x);
    BOOST_CHECK(remote_address(y) == addr);
}

const void* remote_address(const string_t& x)
{
    assert(!x.empty());
    return x.begin();
}

} // namespace adobe

void string_test()
{
    using namespace adobe;
    
    adobe::test_movable(string_t("01234"));

    {  // empty properties
        string_t x;
        BOOST_CHECK(x.begin() == x.end());
        BOOST_CHECK(x.empty());
        BOOST_CHECK(x.begin() == 0);
        BOOST_CHECK(x.capacity() == 0);
        BOOST_CHECK(x.size() == 0);
    }

    {  // non-empty properties
        string_t x("01234");
        BOOST_CHECK(x.begin() != x.end());
        BOOST_CHECK(!x.empty());
        BOOST_CHECK(x.begin() != 0);
        BOOST_CHECK(x.begin() + 5 == x.end());
        BOOST_CHECK(x.capacity() != 0);
        BOOST_CHECK(x.size() == 5);
    }
    #if 0
    {   // fill c-tor
        vector_t x(5, 7);
        BOOST_CHECK(x.front() == 7 && x.back() == 7 && x[2] == 7);
    }
    {    // zero fill c-tor
        vector_t x(5);
        BOOST_CHECK(x.size() == 5);
        BOOST_CHECK(x.back() == 0);
    }
    
    {   // iterators & indexing
        const vector_t x(5, 7);
        vector_t y(5, 8);
        BOOST_CHECK(x.begin() + 5 == x.end());
        BOOST_CHECK(x.rbegin() + 5 == x.rend());
        BOOST_CHECK(y.rbegin() + 5 == y.rend());
        BOOST_CHECK(y[3] == 8);
        BOOST_CHECK(x[3] == 7);
    }
    
    {   // iterator based ctor
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(&a[0], &a[0] + sizeof(a) / sizeof(int));
        BOOST_CHECK(x.front() == 0);
        BOOST_CHECK(x.back() == 5);
        BOOST_CHECK(x.size() == 6);
    }
    {   // iterator based move ctor
        vector_vector_t x(5, vector_t(3, 7));
        const void* addr = remote_address(x.back());
        vector_vector_t y(x.begin(), x.end(), move_ctor());
        BOOST_CHECK(x.front().empty() && x.back().empty());
        BOOST_CHECK(addr = remote_address(y.back()));
    }
    {   // reserve
        vector_vector_t x(5, vector_t(3, 7));
        const void* addr = remote_address(x.back());
        std::size_t c = x.capacity();
        x.reserve(2 * x.capacity());
        BOOST_CHECK(x.capacity() == 2 * c);
        BOOST_CHECK(x.size() == 5);
        BOOST_CHECK(addr = remote_address(x.back()));
    }
    {   // push_back
        vector_t x(3, 7);
        const void* addr = remote_address(x);
        vector_vector_t y;
        y.push_back(x);
        BOOST_CHECK(y.front() == x);
        y.push_back(adobe::move(x));
        BOOST_CHECK(y.front() == y.back());
        BOOST_CHECK(y.size() == 2);
        BOOST_CHECK(addr == remote_address(y.back()));
        BOOST_CHECK(x.empty());
        y.pop_back();
        y.pop_back();
        BOOST_CHECK(y.size() == 0);
    }
    {   // swap
        vector_t x(3, 7);
        vector_t y(4, 5);
        const void* addr_x = remote_address(x);
        const void* addr_y = remote_address(y);
        swap(x, y);
        BOOST_CHECK(addr_x = remote_address(y));
        BOOST_CHECK(addr_y = remote_address(x));
    }
    {   // insert
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(boost::begin(a), boost::end(a));
        int b[] = { 0, 1, 7, 2, 3, 4, 5 };
        vector_t y(boost::begin(b), boost::end(b));
        x.insert(x.begin() + 2, 7);
        BOOST_CHECK(x == y);
    }

    {   // insert (move case)
        vector_t a[] = { vector_t(1, 1), vector_t(2, 2), vector_t(3, 3) };
        vector_vector_t x(boost::begin(a), boost::end(a));
        vector_t b[] = { vector_t(1, 1), vector_t(2, 2), vector_t(7, 7), vector_t(3, 3) };
        vector_vector_t y(boost::begin(b), boost::end(b));
        vector_t z(7, 7);
        const void* addr = remote_address(z);
        x.insert(x.begin() + 2, adobe::move(z));
        BOOST_CHECK(x == y);
        BOOST_CHECK(z.empty());
#if defined(__GNUC__) &&  __GNUC__ < 4
        adobe::ignore_unused(addr);
#else
        BOOST_CHECK(addr == remote_address(x[2]));
#endif
    }
    {   // insert range
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(boost::begin(a), boost::end(a));
        int b[] = { 7, 8, 9 };
        int c[] = { 0, 1, 7, 8, 9, 2, 3, 4, 5 };
        vector_t y(boost::begin(c), boost::end(c));
        
        x.insert(x.begin() + 2, boost::begin(b), boost::end(b));
        BOOST_CHECK(x == y);
    }
    {   // insert fill
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(boost::begin(a), boost::end(a));
        int b[] = { 0, 1, 7, 7, 7, 2, 3, 4, 5 };
        vector_t y(boost::begin(b), boost::end(b));
        x.insert(x.begin() + 2, 3, 7);
        BOOST_CHECK(x == y);
    }
    {   // erase
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(boost::begin(a), boost::end(a));
        int b[] = { 0, 1, 3, 4, 5 };
        vector_t y(boost::begin(b), boost::end(b));
        x.erase(x.begin() + 2);
        BOOST_CHECK(x == y);
    }
    {   // erase range
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(boost::begin(a), boost::end(a));
        int b[] = { 0, 1, 5 };
        vector_t y(boost::begin(b), boost::end(b));
        x.erase(x.begin() + 2, x.begin() + 5);
        BOOST_CHECK(x == y);
    }
    {   // clear
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(boost::begin(a), boost::end(a));
        x.clear();
        BOOST_CHECK(x.size() == 0);
    }
    {   // resize smaller
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(boost::begin(a), boost::end(a));
        int b[] = { 0, 1, 2 };
        vector_t y(boost::begin(b), boost::end(b));
        x.resize(3);
        BOOST_CHECK(x == y);
    }
    {   // resize larger
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(boost::begin(a), boost::end(a));
        int b[] = { 0, 1, 2, 3, 4, 5, 0, 0, 0 };
        vector_t y(boost::begin(b), boost::end(b));
        x.resize(x.size() + 3);
        BOOST_CHECK(x == y);
    }
    {   // resize fill
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(boost::begin(a), boost::end(a));
        int b[] = { 0, 1, 2, 3, 4, 5, 7, 7, 7 };
        vector_t y(boost::begin(b), boost::end(b));
        x.resize(x.size() + 3, 7);
        BOOST_CHECK(x == y);
    }
    {   // comparison operators
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(boost::begin(a), boost::end(a));
        int b[] = { 0, 1, 2, 5, 6 };
        vector_t y(boost::begin(b), boost::end(b));
        BOOST_CHECK(x != y);
        BOOST_CHECK(x < y);
        BOOST_CHECK(y > x);
        BOOST_CHECK(x <= y);
        BOOST_CHECK(y >= x);
    }
    #endif
}


using namespace boost::unit_test;

test_suite*
init_unit_test_suite( int , char* [] ) 
{
    framework::master_test_suite().
        add( BOOST_TEST_CASE( &string_test ) );

    return 0;
}

