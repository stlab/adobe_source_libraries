/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <cmath>

#include <boost/test/unit_test.hpp>

#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/empty.hpp>
#include <adobe/name.hpp>
#include <adobe/serializable.hpp>

/**************************************************************************************************/

void serializable_test() {
    using namespace adobe;
    using namespace adobe::literals;

    std::cout << serializable_t(42) << '\n';
    std::cout << serializable_t(bool()) << '\n';
    std::cout << serializable_t(true) << '\n';
    std::cout << serializable_t(dictionary_t()) << '\n';
    std::cout << serializable_t(M_PI) << '\n';
    std::cout << serializable_t(empty_t()) << '\n';
    std::cout << serializable_t(name_t()) << '\n';
    std::cout << serializable_t(name_t("name_t")) << '\n';
    std::cout << serializable_t("const char*") << '\n';
    std::cout << serializable_t(std::string("std::string")) << '\n';
    std::cout << serializable_t(array_t()) << '\n';

    dictionary_t dict;

    dict["key"_name].assign("value");

    std::cout << serializable_t(std::cref(dict)) << '\n';

    array_t array;

    push_back(array, "value");

    std::cout << serializable_t(std::cref(array)) << '\n';
}

/**************************************************************************************************/

using namespace boost::unit_test;

test_suite* init_unit_test_suite(int, char * []) {
    framework::master_test_suite().add(BOOST_TEST_CASE(&serializable_test));

    return 0;
}

/**************************************************************************************************/
