/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#include <iostream>

#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/empty.hpp>
#include <adobe/iomanip_asl_cel.hpp>
#include <adobe/iomanip_javascript.hpp>
#include <adobe/iomanip_pdf.hpp>
#include <adobe/iomanip_xml.hpp>
#include <adobe/name.hpp>
#include <adobe/string.hpp>

struct unknown_type_t {};

bool operator==(const unknown_type_t&, const unknown_type_t&) { return true; }

bool operator!=(const unknown_type_t&, const unknown_type_t&) { return false; }

int main() {
    using namespace adobe;

    dictionary_t test;
    dictionary_t::value_type one_dict_value("favorite_number"_name, any_regular_t(42));

    test["array_empty"_name] = any_regular_t(array_t());
    test["array_one"_name] = any_regular_t(array_t(1, any_regular_t(42)));
    test["array_many"_name] = any_regular_t(array_t(3, any_regular_t(42)));
    test["bool"_name] = any_regular_t(true);
    test["dictionary_empty"_name] = any_regular_t(dictionary_t());
    test["dictionary_one"_name] =
        any_regular_t(dictionary_t(&one_dict_value, boost::next(&one_dict_value)));
    test["double"_name] = any_regular_t(3.14159265);
    test["empty"_name] = any_regular_t(empty_t());
    test["integer"_name] = any_regular_t(42);
    test["name"_name] = any_regular_t("test_name"_name);
    test["string_safe"_name] = any_regular_t(std::string("Hello, world!"));
    test["string_unsafe"_name] = any_regular_t(std::string("\" ' \n"));
    test["unknown"_name] = any_regular_t(unknown_type_t());

    std::cout << "======== ASL CEL (Safe) ========" << std::endl << std::endl;
    std::cout << begin_asl_cel << test << end_asl_cel << std::endl << std::endl;

    std::cout << "======== ASL CEL (Unsafe) ========" << std::endl << std::endl;
    std::cout << begin_asl_cel_unsafe << test << end_asl_cel_unsafe << std::endl << std::endl;

    std::cout << "======== XML ========" << std::endl << std::endl;
    std::cout << begin_xml << test << end_xml << std::endl << std::endl;

    std::cout << "======== PDF ========" << std::endl << std::endl;
    std::cout << begin_pdf << test << end_pdf << std::endl << std::endl;

    std::cout << "======== JavaScript ========" << std::endl << std::endl;
    std::cout << begin_javascript << test << end_javascript << std::endl << std::endl;
}
