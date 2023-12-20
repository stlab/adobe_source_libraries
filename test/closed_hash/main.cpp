/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#define BOOST_TEST_MAIN

// File being tested is included first
#include <adobe/closed_hash.hpp>

#include <cassert>
#include <iterator>

#include <boost/next_prior.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/test/unit_test.hpp>

#include <adobe/test/check_regular.hpp>

/**************************************************************************************************/

namespace adobe {

/*
    Precondition: x != T(), remote_address(T) is defined.
*/

template <typename T>
void test_movable(const T& x) {
    assert(x != T());
    // assert(is_movable<T>::value);

    adobe::check_regular(x);

    T y = x;
    // move construction (and RVO)
    const void* addr = remote_address(y);
    T z = std::move(y);
#ifndef __clang_analyzer__
    // Use after move
    BOOST_CHECK(y == T());
#endif
    BOOST_CHECK(z == x);
    BOOST_CHECK(remote_address(z) == addr);
    // move assignment
    y = std::move(z);
#ifndef __clang_analyzer__
    // Use after move
    BOOST_CHECK(z == T());
#endif
    BOOST_CHECK(y == x);
    BOOST_CHECK(remote_address(y) == addr);
}

template <typename T>
const void* remote_address(const std::vector<T>& x) {
    assert(!x.empty());
    return &*x.begin();
}

template <typename T>
const void* remote_address(const closed_hash_set<T>& x) {
    assert(!x.empty());
    return &*x.begin();
}

template <typename Key, typename Value>
const void* remote_address(const closed_hash_map<Key, Value>& x) {
    assert(!x.empty());
    return &*x.begin();
}

} // namespace adobe

BOOST_AUTO_TEST_CASE(closed_hash) {
    using namespace adobe;

    typedef adobe::closed_hash_set<int> hash_set_t;
    typedef adobe::closed_hash_map<int, double> hash_map_t;

    typedef std::vector<int> vector_t;
    typedef adobe::closed_hash_map<int, vector_t> hash_map_vector_t;

    {
        int a[] = {0, 1, 2, 3, 4, 5};
        adobe::test_movable(hash_set_t(std::begin(a), std::end(a)));
    }
    {
        std::pair<int, double> a[] = {std::make_pair(0, 0.0), std::make_pair(1, 1.1),
                                      std::make_pair(2, 2.2)};
        adobe::test_movable(hash_map_t(std::begin(a), std::end(a)));
    }

    { // empty properties
        hash_set_t x;
        BOOST_CHECK(x.begin() == x.end());
        BOOST_CHECK(x.empty());
        BOOST_CHECK(x.capacity() == 0);
        BOOST_CHECK(x.size() == 0);
    }

    { // empty properties
        hash_map_t x;
        BOOST_CHECK(x.begin() == x.end());
        BOOST_CHECK(x.empty());
        BOOST_CHECK(x.capacity() == 0);
        BOOST_CHECK(x.size() == 0);
    }

    { // non-empty properties
        int a[] = {0, 1, 2};
        hash_set_t x(std::begin(a), std::end(a));
        BOOST_CHECK(x.begin() != x.end());
        BOOST_CHECK(!x.empty());
        BOOST_CHECK(std::next(x.begin(), 3) == x.end());
        BOOST_CHECK(x.capacity() != 0);
        BOOST_CHECK(x.size() == 3);
    }

    { // non-empty properties
        std::pair<int, double> a[] = {std::make_pair(0, 0.0), std::make_pair(1, 1.1),
                                      std::make_pair(2, 2.2)};
        hash_map_t x(std::begin(a), std::end(a));
        BOOST_CHECK(x.begin() != x.end());
        BOOST_CHECK(!x.empty());
        BOOST_CHECK(std::next(x.begin(), 3) == x.end());
        BOOST_CHECK(x.capacity() != 0);
        BOOST_CHECK(x.size() == 3);
    }

    { // iterators & indexing
        int a[] = {0, 1, 2};
        const hash_set_t x(std::begin(a), std::end(a));
        hash_set_t y = x;
        BOOST_CHECK(std::next(x.begin(), 3) == x.end());
        BOOST_CHECK(std::next(x.rbegin(), 3) == x.rend());
        BOOST_CHECK(std::next(y.rbegin(), 3) == y.rend());
    }

    { // iterators & indexing
        std::pair<int, double> a[] = {std::make_pair(0, 0.0), std::make_pair(1, 1.1),
                                      std::make_pair(2, 2.2)};
        const hash_map_t x(std::begin(a), std::end(a));
        hash_map_t y = x;
        BOOST_CHECK(std::next(x.begin(), 3) == x.end());
        BOOST_CHECK(std::next(x.rbegin(), 3) == x.rend());
        BOOST_CHECK(std::next(y.rbegin(), 3) == y.rend());
        BOOST_CHECK(y[1] == 1.1);
        y[1] = 5.5;
        BOOST_CHECK(y[1] == 5.5);
    }

    { // reserve
        std::pair<int, vector_t> a[] = {std::make_pair(1, vector_t(1, 1)),
                                        std::make_pair(2, vector_t(2, 2)),
                                        std::make_pair(3, vector_t(3, 3))};
        hash_map_vector_t x(std::begin(a), std::end(a));
        const void* addr = remote_address(x.find(2)->second);

        std::size_t c = x.capacity();
        x.reserve(2 * x.capacity());
        BOOST_CHECK(x.capacity() > c);
        BOOST_CHECK(x.size() == 3);
        BOOST_CHECK(addr == remote_address(x.find(2)->second));
    }

#if 0
    {   // push_back
        vector_t x(3, 7);
        const void* addr = remote_address(x);
        vector_vector_t y;
        y.push_back(x);
        BOOST_CHECK(y.front() == x);
        y.push_back(std::move(x));
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
        vector_t x(std::begin(a), std::end(a));
        int b[] = { 0, 1, 7, 2, 3, 4, 5 };
        vector_t y(std::begin(b), std::end(b));
        x.insert(x.begin() + 2, 7);
        BOOST_CHECK(x == y);
    }
    {   // insert (move case)
        vector_t a[] = { vector_t(1, 1), vector_t(2, 2), vector_t(3, 3) };
        vector_vector_t x(std::begin(a), std::end(a));
        vector_t b[] = { vector_t(1, 1), vector_t(2, 2), vector_t(7, 7), vector_t(3, 3) };
        vector_vector_t y(std::begin(b), std::end(b));
        vector_t z(7, 7);
        const void* addr = remote_address(z);
        x.insert(x.begin() + 2, std::move(z));
        BOOST_CHECK(x == y);
        BOOST_CHECK(z.empty() && addr == remote_address(x[2]));
    }
    {   // insert range
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(std::begin(a), std::end(a));
        int b[] = { 7, 8, 9 };
        int c[] = { 0, 1, 7, 8, 9, 2, 3, 4, 5 };
        vector_t y(std::begin(c), std::end(c));

        x.insert(x.begin() + 2, std::begin(b), std::end(b));
        BOOST_CHECK(x == y);
    }
    {   // insert fill
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(std::begin(a), std::end(a));
        int b[] = { 0, 1, 7, 7, 7, 2, 3, 4, 5 };
        vector_t y(std::begin(b), std::end(b));
        x.insert(x.begin() + 2, 3, 7);
        BOOST_CHECK(x == y);
    }
    {   // erase
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(std::begin(a), std::end(a));
        int b[] = { 0, 1, 3, 4, 5 };
        vector_t y(std::begin(b), std::end(b));
        x.erase(x.begin() + 2);
        BOOST_CHECK(x == y);
    }
    {   // erase range
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(std::begin(a), std::end(a));
        int b[] = { 0, 1, 5 };
        vector_t y(std::begin(b), std::end(b));
        x.erase(x.begin() + 2, x.begin() + 5);
        BOOST_CHECK(x == y);
    }
    {   // clear
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(std::begin(a), std::end(a));
        x.clear();
        BOOST_CHECK(x.size() == 0);
    }
    {   // resize smaller
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(std::begin(a), std::end(a));
        int b[] = { 0, 1, 2 };
        vector_t y(std::begin(b), std::end(b));
        x.resize(3);
        BOOST_CHECK(x == y);
    }
    {   // resize larger
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(std::begin(a), std::end(a));
        int b[] = { 0, 1, 2, 3, 4, 5, 0, 0, 0 };
        vector_t y(std::begin(b), std::end(b));
        x.resize(x.size() + 3);
        BOOST_CHECK(x == y);
    }
    {   // resize fill
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(std::begin(a), std::end(a));
        int b[] = { 0, 1, 2, 3, 4, 5, 7, 7, 7 };
        vector_t y(std::begin(b), std::end(b));
        x.resize(x.size() + 3, 7);
        BOOST_CHECK(x == y);
    }
    {   // comparison operators
        int a[] = { 0, 1, 2, 3, 4, 5 };
        vector_t x(std::begin(a), std::end(a));
        int b[] = { 0, 1, 2, 5, 6 };
        vector_t y(std::begin(b), std::end(b));
        BOOST_CHECK(x != y);
        BOOST_CHECK(x < y);
        BOOST_CHECK(y > x);
        BOOST_CHECK(x <= y);
        BOOST_CHECK(y >= x);
    }
#endif
}
