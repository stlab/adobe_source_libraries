/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/config.hpp>

#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <adobe/enum_ops.hpp>


enum Number {
    num_neg_one = -1,
    num_0 = 0,
    num_1 = 1,
    num_2 = 2,
    num_3 = 3,
    num_4 = 4,
    num_5 = 5,
    num_6 = 6,
    num_7 = 7
};

enum class Views : int { None = 0, Text = 1 << 0, Icon = 1 << 1, Preview = 1 << 2 };

enum class Num : int {
    num_0 = 0,
    num_1 = 1,
    num_2 = 2,
    num_3 = 3,
    num_4 = 4,
    num_5 = 5,
    num_6 = 6,
    num_7 = 7
};

auto adobe_enable_bitmask_enum(Views) -> std::true_type;

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
    x = num_7 & (~num_5);
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

BOOST_AUTO_TEST_CASE(enumclass_bitset_ops) {
    // the above proves our general cases
    // this just sanity checks enum classes
    Views x;
    x = Views::Text | Views::Icon;
    BOOST_CHECK((x & Views::Text) == Views::Text);
    BOOST_CHECK((x & Views::Preview) != Views::Preview);
}


ADOBE_DEFINE_ARITHMETIC_OPS(Number)
auto adobe_enable_arithmetic_enum(Num) -> std::true_type;

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
    x = num_2 * 3;
    BOOST_CHECK(x == 6);

    // /
    x = num_5 / num_2;
    BOOST_CHECK(x == 2);

    // %
    x = num_5 % num_2;
    BOOST_CHECK(x == 1);

    // +=
    x = num_1;
    x += num_2;
    BOOST_CHECK(x == 3);

    // -=
    x = num_3;
    x -= num_1;
    BOOST_CHECK(x == 2);

    // *=
    x = num_2;
    x *= 3;
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

BOOST_AUTO_TEST_CASE(enumclass_arith_ops) {
    // the above proves our general cases
    // this just sanity checks enum classes
    Num x;
    x = Num::num_1;
    BOOST_CHECK((x + Num::num_3) == Num::num_4);
}
