/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <boost/next_prior.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <adobe/closed_hash.hpp>
#include <adobe/vector.hpp>
#include <adobe/test/check_regular.hpp>
#include <cassert>
#include <iterator>

/*************************************************************************************************/

namespace adobe {

/*
    Precondition: x != T(), remote_address(T) is defined.
*/

template <typename T>
void test_movable(const T& x)
{
    assert(x != T());
    assert(is_movable<T>::value);
    
	adobe::check_regular(x);
    
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

template <typename T>
const void* remote_address(const vector<T>& x)
{
    assert(!x.empty());
    return &*x.begin();
}

template <typename T>
const void* remote_address(const closed_hash_set<T>& x)
{
    assert(!x.empty());
    return &*x.begin();
}

template <typename Key, typename Value>
const void* remote_address(const closed_hash_map<Key, Value>& x)
{
    assert(!x.empty());
    return &*x.begin();
}

} // namespace adobe

BOOST_AUTO_TEST_CASE(vector_allocator_rtti)
{
    using namespace adobe;
    typedef adobe::vector<int, capture_allocator<int> > vector_t;
	//vector_t v;
	type_info_t t1 = adobe::type_info<vector_t>();

	BOOST_CHECK(!t1.requires_std_rtti());

    typedef adobe::vector<int, std::allocator<int> > vector_stda_t;
	type_info_t t2 = adobe::type_info<vector_stda_t>();
	BOOST_CHECK(t2.requires_std_rtti());
}


BOOST_AUTO_TEST_CASE(closed_hash_allocator_rtti)
{
    using namespace adobe;

    typedef adobe::closed_hash_set<int, adobe::identity<const int>, 
		boost::hash<int>, std::equal_to<int>, adobe::capture_allocator<int> > hash_set_t;

	type_info_t t1 = adobe::type_info<hash_set_t>();
	BOOST_CHECK(!t1.requires_std_rtti());

    typedef adobe::closed_hash_map<int, double, boost::hash<int>, 
		std::equal_to<int>, adobe::capture_allocator<adobe::pair<int, double> > > hash_map_t;
    
	type_info_t t2 = adobe::type_info<hash_map_t>();
	BOOST_CHECK(!t2.requires_std_rtti());

    typedef adobe::vector<int,capture_allocator<int> > vector_t;

    typedef adobe::closed_hash_map<int, vector_t, boost::hash<int>, 
		std::equal_to<int>, adobe::capture_allocator<adobe::pair<int, vector_t> > > hash_map_vector_t;

	type_info_t t3 = adobe::type_info<hash_map_vector_t>();
	BOOST_CHECK(!t3.requires_std_rtti());

    typedef adobe::closed_hash_set<int, adobe::identity<const int>, 
		boost::hash<int>, std::equal_to<int>, std::allocator<int> > hash_set_stda_t;

	type_info_t t4 = adobe::type_info<hash_set_stda_t>();
	BOOST_CHECK(t4.requires_std_rtti());

    typedef adobe::closed_hash_map<int, double, boost::hash<int>, 
		std::equal_to<int>, std::allocator<double> > hash_map_stda_t;
    
	type_info_t t5 = adobe::type_info<hash_map_stda_t>();
	BOOST_CHECK(t5.requires_std_rtti());

    typedef adobe::vector<int, std::allocator<int> > vector_stda_t;

    typedef adobe::closed_hash_map<int, vector_t, boost::hash<int>, 
		std::equal_to<int>, std::allocator<vector_stda_t> > hash_map_vector_stda_t;

	type_info_t t6 = adobe::type_info<hash_map_vector_stda_t>();
	BOOST_CHECK(t6.requires_std_rtti());
}

BOOST_AUTO_TEST_CASE(closed_hash)
{
    using namespace adobe;
    
    typedef adobe::closed_hash_set<int> hash_set_t;
    typedef adobe::closed_hash_map<int, double> hash_map_t;
    
    typedef adobe::vector<int> vector_t;
    typedef adobe::closed_hash_map<int, vector_t> hash_map_vector_t;
    
    {
    int a[] = { 0, 1, 2, 3, 4, 5 };
    adobe::test_movable(hash_set_t(boost::begin(a), boost::end(a)));
    }
    {
    std::pair<int, double> a[] = { std::make_pair(0, 0.0), std::make_pair(1, 1.1), std::make_pair(2, 2.2) };
    adobe::test_movable(hash_map_t(boost::begin(a), boost::end(a)));
    }

    {  // empty properties
        hash_set_t x;
        BOOST_CHECK(x.begin() == x.end());
        BOOST_CHECK(x.empty());
        BOOST_CHECK(x.capacity() == 0);
        BOOST_CHECK(x.size() == 0);
    }

    {  // empty properties
        hash_map_t x;
        BOOST_CHECK(x.begin() == x.end());
        BOOST_CHECK(x.empty());
        BOOST_CHECK(x.capacity() == 0);
        BOOST_CHECK(x.size() == 0);
    }

    {  // non-empty properties
        int a[] = { 0, 1, 2 };
        hash_set_t x(boost::begin(a), boost::end(a));
        BOOST_CHECK(x.begin() != x.end());
        BOOST_CHECK(!x.empty());
        BOOST_CHECK(boost::next(x.begin(), 3) == x.end());
        BOOST_CHECK(x.capacity() != 0);
        BOOST_CHECK(x.size() == 3);
    }

    {  // non-empty properties
        std::pair<int, double> a[] =
                { std::make_pair(0, 0.0), std::make_pair(1, 1.1), std::make_pair(2, 2.2) };
        hash_map_t x(boost::begin(a), boost::end(a));
        BOOST_CHECK(x.begin() != x.end());
        BOOST_CHECK(!x.empty());
        BOOST_CHECK(boost::next(x.begin(), 3) == x.end());
        BOOST_CHECK(x.capacity() != 0);
        BOOST_CHECK(x.size() == 3);
    }
    
    {   // iterators & indexing
        int a[] = { 0, 1, 2 };
        const hash_set_t x(boost::begin(a), boost::end(a));
        hash_set_t y = x;
        BOOST_CHECK(boost::next(x.begin(), 3) == x.end());
        BOOST_CHECK(boost::next(x.rbegin(), 3) == x.rend());
        BOOST_CHECK(boost::next(y.rbegin(), 3) == y.rend());
    }
    
    {   // iterators & indexing
        std::pair<int, double> a[] =
                { std::make_pair(0, 0.0), std::make_pair(1, 1.1), std::make_pair(2, 2.2) };
        const hash_map_t x(boost::begin(a), boost::end(a));
        hash_map_t y = x;
        BOOST_CHECK(boost::next(x.begin(), 3) == x.end());
        BOOST_CHECK(boost::next(x.rbegin(), 3) == x.rend());
        BOOST_CHECK(boost::next(y.rbegin(), 3) == y.rend());
        BOOST_CHECK(y[1] == 1.1);
        y[1] = 5.5;
        BOOST_CHECK(y[1] == 5.5);
    }
    
#if 0
    {   // iterator based move ctor
        std::pair<int, vector_t> a[] = { std::make_pair(1, vector_t(1, 1)),
                std::make_pair(2, vector_t(2, 2)), std::make_pair(3, vector_t(3, 3)) };
        hash_map_vector_t x(boost::begin(a), boost::end(a));
        const void* addr = remote_address(x.find(2)->second);
        hash_map_vector_t y(x.begin(), x.end(), move_ctor());
        BOOST_CHECK(x.begin()->second.empty());
        BOOST_CHECK(addr = remote_address(y.find(2)->second));
    }
#endif
    
    {   // reserve
        std::pair<int, vector_t> a[] = { std::make_pair(1, vector_t(1, 1)),
                std::make_pair(2, vector_t(2, 2)), std::make_pair(3, vector_t(3, 3)) };
        hash_map_vector_t x(boost::begin(a), boost::end(a));
        const void* addr = remote_address(x.find(2)->second);

        std::size_t c = x.capacity();
        x.reserve(2 * x.capacity());
        BOOST_CHECK(x.capacity() > c);
        BOOST_CHECK(x.size() == 3);
        BOOST_CHECK(addr = remote_address(x.find(2)->second));
    }
    
#if 0
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
        BOOST_CHECK(z.empty() && addr == remote_address(x[2]));
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
