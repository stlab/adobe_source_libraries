/**************************************************************************************************/

/*

    ADOBE CONFIDENTIAL
    ___________________

    Copyright 2001 - 2004 Adobe Systems Incorporated
    All Rights Reserved.

    NOTICE: All information contained herein is, and remains the property of
    Adobe Systems Incorporated and its suppliers, if any. The intellectual and
    technical concepts contained herein are proprietary to Adobe Systems Incorporated
    and its suppliers and may be covered by U.S. and Foreign Patents, patents
    in process, and are protected by trade secret or copyright law. Dissemination
    of this information or reproduction of this material is strictly forbidden
    unless prior written permission is obtained from Adobe Systems Incorporated.

*/

/**************************************************************************************************/

// start_of_example

#include <adobe/enum_ops.hpp>
#include <boost/detail/lightweight_test.hpp>

enum Foo { foo_4 = 1 << 2, foo_8 = 1 << 3 };

enum class Views : int { None = 0, Text = 1 << 0, Icon = 1 << 1, Preview = 1 << 2 };

auto stlab_enable_bitmask_enum(E) -> std::true_type;

ADOBE_DEFINE_BITSET_OPS(Foo)

int main() {
    Foo a(foo_4);
    Foo b(foo_8);
    Foo c(a | b);

    BOOST_TEST(a == 4L);
    BOOST_TEST(b == 8L);
    BOOST_TEST(c == 12L);

    Views x;
    x = Views::Text | Views::Icon;
    BOOST_TEST((x & Views::Text) == Views::Text);
    BOOST_TEST((x & Views::Preview) != Views::Preview);

    return boost::report_errors();
}

// end_of_example
