#include <iostream>

#include <boost/function.hpp>

#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/empty.hpp>
#include <adobe/iomanip_asl_cel.hpp>
#include <adobe/iomanip_javascript.hpp>
#include <adobe/iomanip_pdf.hpp>
#include <adobe/iomanip_xml.hpp>
#include <adobe/name.hpp>
#include <adobe/string.hpp>

struct unknown_type_t
{ };

bool operator==(const unknown_type_t&, const unknown_type_t&)
{ return true; }

bool operator!=(const unknown_type_t&, const unknown_type_t&)
{ return false; }

int main()
{
    using namespace adobe;

    dictionary_t             test;
    dictionary_t::value_type one_dict_value(static_name_t("favorite_number"), any_regular_t(42));

    test[static_name_t("array_empty")] = any_regular_t(array_t());
    test[static_name_t("array_one")] = any_regular_t(array_t(1, any_regular_t(42)));
    test[static_name_t("array_many")] = any_regular_t(array_t(3, any_regular_t(42)));
    test[static_name_t("bool")] = any_regular_t(true);
    test[static_name_t("dictionary_empty")] = any_regular_t(dictionary_t());
    test[static_name_t("dictionary_one")] = any_regular_t(dictionary_t(&one_dict_value, boost::next(&one_dict_value)));
    test[static_name_t("double")] = any_regular_t(3.14159265);
    test[static_name_t("empty")] = any_regular_t(empty_t());
    test[static_name_t("integer")] = any_regular_t(42);
    test[static_name_t("name")] = any_regular_t(static_name_t("test_name"));
    test[static_name_t("string_safe")] = any_regular_t(string_t("Hello, world!"));
    test[static_name_t("string_unsafe")] = any_regular_t(string_t("\" ' \n"));
    test[static_name_t("unknown")] = any_regular_t(unknown_type_t());

    std::cout << "======== ASL CEL (Safe) ========"<< std::endl << std::endl;
    std::cout << begin_asl_cel << test << end_asl_cel << std::endl << std::endl;

    std::cout << "======== ASL CEL (Unsafe) ========"<< std::endl << std::endl;
    std::cout << begin_asl_cel_unsafe << test << end_asl_cel_unsafe << std::endl << std::endl;

    std::cout << "======== XML ========"<< std::endl << std::endl;
    std::cout << begin_xml << test << end_xml << std::endl << std::endl;

    std::cout << "======== PDF ========"<< std::endl << std::endl;
    std::cout << begin_pdf << test << end_pdf << std::endl << std::endl;

    std::cout << "======== JavaScript ========"<< std::endl << std::endl;
    std::cout << begin_javascript << test << end_javascript << std::endl << std::endl;
}
