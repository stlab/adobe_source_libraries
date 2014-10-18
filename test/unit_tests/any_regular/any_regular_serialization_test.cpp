/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#include <adobe/config.hpp>

#include <functional>
#include <utility>

#include <boost/test/unit_test.hpp>

#include <adobe/any_regular.hpp>

/*************************************************************************************************/

struct foo_t { int x; };
bool operator==(const foo_t& x, const foo_t& y) { return x.x == y.x; };
bool operator!=(const foo_t& x, const foo_t& y) { return !(x == y); }

// foo_t has operator<< defined. As such the following routine will be called
// when foo_t is wrapped in an any_regular_t.
void operator<<(std::ostream& s, const foo_t& x) {
    s << x.x;
}

// bar_t has no operator<< defined. As such it will not be serialized when bar_t
// is wrapped in an any_regular_t.
struct bar_t { };
bool operator==(const bar_t& x, const bar_t& y) { return true; };
bool operator!=(const bar_t& x, const bar_t& y) { return !(x == y); }

void any_regular_serialization_test() {
    using adobe::any_regular_t;

    std::cout << "foo_t: " << any_regular_t(foo_t{42}) << '\n';
    std::cout << "bar_t: " << any_regular_t(bar_t()) << '\n';
}

using namespace boost::unit_test;

test_suite* init_unit_test_suite(int, char * []) {
    framework::master_test_suite().add(BOOST_TEST_CASE(&any_regular_serialization_test));

    return 0;
}
