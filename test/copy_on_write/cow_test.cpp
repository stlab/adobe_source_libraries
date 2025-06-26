/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/config.hpp>

#include <functional>
#include <iostream>
#include <utility>

#define BOOST_TEST_MAIN

#include <adobe/test/check_less_than_comparable.hpp>
#include <adobe/test/check_null.hpp>
#include <adobe/test/check_regular.hpp>
#include <boost/test/unit_test.hpp>

#include <adobe/copy_on_write.hpp>
#include <adobe/memory.hpp>

namespace {

template <typename R, typename T>
R make_value(const T& x) {
    return R(x);
}

template <>
std::string make_value(const long& x) {
    std::stringstream s;
    s << x;
    return std::string(s.str());
}

template <typename CowType>
void test_copy_on_write() {

    typename CowType::value_type (*mv)(const long&) =
        &make_value<typename CowType::value_type, long>;

    // Test default constructor
    {
        CowType value_0;
    }
    // Test basic concept requirements
    {
        CowType value_1(mv(1)); // allocation
        CowType value_2(mv(2)); // allocation
        CowType value_3(mv(3)); // allocation

        // regular
        adobe::check_regular(value_1);

        // operator<
        adobe::check_less_than_comparable(value_1, value_2, value_3, std::less<CowType>());

        // operator>
        adobe::check_less_than_comparable(value_3, value_2, value_1, std::greater<CowType>());

        CowType value_test(mv(1)); // allocation

        BOOST_CHECK_MESSAGE(value_1 == value_test, "equality of non-identical values");
        BOOST_CHECK_MESSAGE(value_2 != value_test, "equality of non-identical values");

        BOOST_CHECK(value_test.unique());

        value_test = value_2; // deallocation

        BOOST_CHECK(!value_test.unique());
        BOOST_CHECK(value_test.identity(value_2));
    }

    // Test basic move semantics
    {
        CowType value_1(mv(42)); // allocation
        CowType value_2(mv(21)); // allocation
        CowType value_move(std::move(value_1));

        value_move = std::move(value_2); // deallocation
    }

    // Test copy-assignment using null object_m
    {
        CowType foo(mv(1)); // allocation
        CowType bar(std::move(foo));

        foo = mv(2); // allocation
    }

    // Test copy-assignment using non-null object_m
    {
        CowType foo(mv(5)); // allocation
        CowType bar(foo);

        BOOST_CHECK(bar.identity(foo));

        bar = mv(6); // allocation

        BOOST_CHECK(bar.unique() && foo.unique());
    }

    // Test move-assignment using null object_m
    {
        CowType foo(mv(1)); // allocation
        CowType bar(std::move(foo));
        typename CowType::value_type value(mv(2));

        foo = std::move(value); // allocation
    }

    // Test move-assignment using unique instance
    {
        CowType foo(mv(1)); // allocation
        typename CowType::value_type value(mv(2));

        foo = std::move(value);
    }

    // Test move-assignment using new allocation
    {
        CowType foo(mv(1)); // allocation
        CowType bar(foo);
        typename CowType::value_type value(mv(2));

        foo = std::move(value); // allocation
    }

    // Test write() using unique instance
    {
        CowType foo(mv(1)); // allocation

        foo.write() = typename CowType::value_type(mv(2));
    }

    // Test write() using new allocation
    {
        CowType foo(mv(1)); // allocation
        CowType bar(foo);

        foo.write() = typename CowType::value_type(mv(2)); // allocation
    }

    // Test read()
    {
        CowType foo(mv(1)); // allocation

        BOOST_CHECK_MESSAGE(foo.read() == typename CowType::value_type(mv(1)), "read error");
        BOOST_CHECK_MESSAGE(static_cast<typename CowType::value_type>(foo) ==
                                typename CowType::value_type(mv(1)),
                            "read error");
        BOOST_CHECK_MESSAGE(*foo == typename CowType::value_type(mv(1)), "read error");
        BOOST_CHECK_MESSAGE(*(foo.operator->()) == typename CowType::value_type(mv(1)),
                            "read error");
    }

    // Test swap
    {
        CowType foo(mv(1)); // allocation
        CowType bar(mv(2)); // allocation

        swap(foo, bar);

        BOOST_CHECK_MESSAGE(foo.read() == typename CowType::value_type(mv(2)), "swap error");
        BOOST_CHECK_MESSAGE(bar.read() == typename CowType::value_type(mv(1)), "swap error");
    }
}
} // namespace

BOOST_AUTO_TEST_CASE(CowType_allocator_rtti) {
    using namespace adobe;

    {
        typedef copy_on_write<int> cow_t;

        std::cout << typeid(cow_t).name() << '\n';

        // BOOST_CHECK(!t.requires_std_rtti());
    }

    {
        typedef copy_on_write<int> cow_t;

        std::cout << typeid(cow_t).name() << '\n';

        // BOOST_CHECK(t.requires_std_rtti());
    }
}

BOOST_AUTO_TEST_CASE(copy_on_write) {
    test_copy_on_write<adobe::copy_on_write<int>>();
    test_copy_on_write<adobe::copy_on_write<std::string>>();
}
