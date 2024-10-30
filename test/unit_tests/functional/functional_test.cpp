/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/functional.hpp> // file be tested included first

#include <utility>

#include <adobe/enum_ops.hpp>

#define BOOST_TEST_MAIN

// boost
#include <boost/test/unit_test.hpp>

/**************************************************************************************************/

using namespace std;
using namespace adobe;

namespace {

/// An enumeration of member function qualifiers as bit fields to identify or mask an overload.
enum member_function_qualifiers {
    _none = 0,
    _const_lvalue = 1 << 0,
    _lvalue = 1 << 1,
    _rvalue = 1 << 2,
    _all = _const_lvalue | _lvalue | _rvalue
};

auto adobe_enable_bitmask_enum(member_function_qualifiers) -> std::true_type;

/// An overload of a binary function that returns a tuple of its arguments and the qualifiers used.
/// Only the members specified by the noexcept_qualifier mask are noexcept.
template <member_function_qualifiers noexcept_qualifier>
struct test_binary_function {
    auto operator()(int x, int y) const& noexcept((noexcept_qualifier & _const_lvalue) != _none) {
        return tuple(x, y, _const_lvalue);
    }
    auto operator()(int x, int y) & noexcept((noexcept_qualifier & _lvalue) != _none) {
        return tuple(x, y, _lvalue);
    }
    auto operator()(int x, int y) && noexcept((noexcept_qualifier & _rvalue) != _none) {
        return tuple(x, y, _rvalue);
    }
};

} // namespace


BOOST_AUTO_TEST_CASE(functional_transpose) {

    // Test that the transpose functional object correctly transposes the arguments of a binary
    // function. It also tests that the correct overload is selected and that the noexcept
    // qualifier is correctly propagated.

    transpose lvalue{test_binary_function < _all ^ _lvalue > {}};
    const transpose const_lvalue{test_binary_function < _all ^ _const_lvalue > {}};
    transpose lvalue_noexcept{test_binary_function<_lvalue>{}};
    const transpose const_lvalue_noexcept{test_binary_function<_const_lvalue>{}};

    BOOST_TEST((lvalue(1, 2) == tuple(2, 1, _lvalue)));
    BOOST_TEST(!noexcept(lvalue(1, 2)));

    BOOST_TEST((const_lvalue(1, 2) == tuple(2, 1, _const_lvalue)));
    BOOST_TEST(!noexcept(const_lvalue(1, 2)));

    BOOST_TEST((lvalue_noexcept(1, 2) == tuple(2, 1, _lvalue)));
    BOOST_TEST(noexcept(lvalue_noexcept(1, 2)));

    BOOST_TEST((const_lvalue_noexcept(1, 2) == tuple(2, 1, _const_lvalue)));
    BOOST_TEST(noexcept(const_lvalue_noexcept(1, 2)));

    BOOST_TEST(
        (transpose(test_binary_function < _all ^ _rvalue > {})(1, 2) == tuple(2, 1, _rvalue)));
    BOOST_TEST(!noexcept(transpose(test_binary_function < _all ^ _rvalue > {})(1, 2)));

    BOOST_TEST((transpose(test_binary_function<_rvalue>{})(1, 2) == tuple(2, 1, _rvalue)));
    BOOST_TEST(noexcept(transpose(test_binary_function<_rvalue>{})(1, 2)));
}
