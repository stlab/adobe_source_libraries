/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_UTILITY_PAIR_HPP
#define ADOBE_UTILITY_PAIR_HPP

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <utility>

#include <boost/operators.hpp>

#include <adobe/empty.hpp>

#include <adobe/implementation/swap.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

/*!
\defgroup asl_pair pair
\ingroup utility
*/

//! \ingroup asl_pair
template <typename T1, typename T2 = T1>
struct aggregate_pair {
    typedef T1 first_type;
    typedef T2 second_type;

    T1 first;
    T2 second;

    friend inline bool operator==(const aggregate_pair& x, const aggregate_pair& y) {
        return x.first == y.first && x.second == y.second;
    }

    friend inline bool operator<(const aggregate_pair& x, const aggregate_pair& y) {
        return x.first < y.first || (!(y.first < x.first) && x.second < y.second);
    }

    friend inline bool operator!=(const aggregate_pair& x, const aggregate_pair& y) {
        return !(x == y);
    }

    friend inline bool operator>(const aggregate_pair& x, const aggregate_pair& y) { return y < x; }

    friend inline bool operator<=(const aggregate_pair& x, const aggregate_pair& y) {
        return !(y < x);
    }

    friend inline bool operator>=(const aggregate_pair& x, const aggregate_pair& y) {
        return !(x < y);
    }

    friend inline void swap(aggregate_pair& x, aggregate_pair& y) {
        swap(x.first, y.first);
        swap(x.second, y.second);
    }
};

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

// ADOBE_UTILITY_PAIR_HPP
#endif

/*************************************************************************************************/
