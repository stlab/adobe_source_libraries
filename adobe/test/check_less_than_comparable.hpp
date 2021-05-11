/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/config.hpp>
#include <adobe/utility/ignore_unused.hpp>
#include <boost/test/unit_test.hpp>


namespace adobe {

/*!
\addtogroup testing
@{
*/

template <typename T, typename Op>
void check_transitive(const T& x, const T& y, const T& z, Op op) {
    ignore_unused(y);
    assert(op(x, y) && op(y, z));
    BOOST_CHECK_MESSAGE(op(x, z), "tranisitive");
}

template <typename T, typename Op>
void check_irreflexive(const T& x, Op op) {
    BOOST_CHECK_MESSAGE(!op(x, x), "irreflexive");
}

template <typename T, typename Op>
void check_antisymmetric(const T& x, const T& y, Op op) {
    BOOST_CHECK_MESSAGE(!(op(x, y) && op(y, x)), "anti-symmetric");
}


template <typename T, typename Op>
void check_less_than_comparable(const T& x, const T& y, const T& z, Op op) {
    check_irreflexive(x, op);
    check_antisymmetric(x, y, op);
    check_transitive(x, y, z, op);
}

//!@}
} // namespace adobe
