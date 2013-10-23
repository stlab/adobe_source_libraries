/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>

#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <adobe/enum_ops.hpp>


enum Number {
    num_neg_one=-1,
    num_0 = 0,
    num_1 = 1,
    num_2 = 2,
    num_3 = 3,
    num_4 = 4,
    num_5 = 5,
    num_6 = 6,
    num_7 = 7
};

ADOBE_DEFINE_BITSET_OPS(Number)

BOOST_AUTO_TEST_CASE(enum_bitset_ops) {
    Number x;

// or
    x = num_1 | num_2;
    BOOST_CHECK(x == 3);

// and
    x = num_3 & num_1;
    BOOST_CHECK(x == 1);

// xor
    x = num_2 ^ num_3;
    BOOST_CHECK(x == 1);

// not
    x = num_7 & (~ num_5);
    BOOST_CHECK(x == 2);

// or=
    x = num_1;
    x |= num_2;
    BOOST_CHECK(x == 3); 

// and=
    x = num_3;
    x &= num_1;
    BOOST_CHECK(x == 1);

// xor
    x = num_2;
    x ^= num_3;
    BOOST_CHECK(x == 1);
}


ADOBE_DEFINE_ARITHMETIC_OPS(Number)
BOOST_AUTO_TEST_CASE(enum_arith_ops) {
    Number x;

// +
    x = num_1 + num_2;
    BOOST_CHECK(x == 3);

// -
    x = num_3 - num_1;
    BOOST_CHECK(x == 2);

// - (unary)
    x = -num_neg_one;
    BOOST_CHECK(x == 1);     

// *
    x = num_2 * num_3;
    BOOST_CHECK(x == 6);

// /
    x = num_5 / num_2;
    BOOST_CHECK(x == 2); 

// %
    x = num_5 % num_2;
    BOOST_CHECK(x == 1); 

// +=
    x = num_1;
    x+= num_2;
    BOOST_CHECK(x == 3);

// -=
    x = num_3;
    x -= num_1;
    BOOST_CHECK(x == 2);

// *=
    x = num_2;
    x *= num_3;
    BOOST_CHECK(x == 6);

// /=
    x = num_5;
    x /= num_2;
    BOOST_CHECK(x == 2); 

// %=
    x = num_5;
    x %= num_2;
    BOOST_CHECK(x == 1); 
}
