/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/expression_parser.hpp>

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <sstream>

/**************************************************************************************************/

using namespace adobe;
using namespace std;

/**************************************************************************************************/

void exception_test(const char* const expression, const adobe::line_position_t& expression_position,
                    int expected_line_number, std::streampos expected_character_position) {
    istringstream expression_stream(expression);
    expression_parser exp(expression_stream, expression_position);
    bool caught(false);
    try {
        array_t instructions;
        exp.require_expression(instructions);
    } catch (const stream_error_t& error) {
        cerr << error;
        caught = true;
        BOOST_REQUIRE_EQUAL(error.line_position_set().size(), 1u);
        const auto& error_position = error.line_position_set().front();
        BOOST_REQUIRE_EQUAL(error_position.line_number_m, expected_line_number);
        BOOST_REQUIRE_LT(error_position.line_start_m, error_position.position_m);
        BOOST_REQUIRE_EQUAL(error_position.position_m - error_position.line_start_m + 1,
                            expected_character_position);
    }
    BOOST_REQUIRE(caught);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_configure_output) { cerr << "CTEST_FULL_OUTPUT\n\n"; }

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_and_expression) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        true || /* No Expression */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 17);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_bitwise_or_expression) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        true && /* No Expression */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 17);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_bitwise_xor_expression) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        42 | /* No Expression */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 14);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_bitwise_and_expression) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        42 ^ /* No Expression */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 14);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_equality_expression) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        42 & /* No Expression */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 14);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_relational_expression) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        42 == /* No Expression */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 15);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_bitshift_expression) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        42 <= /* No Expression */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 15);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_additive_expression) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        42 << /* No Expression */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 15);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_multiplicative_expression) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        42 + /* No Expression */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 14);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_unary_expression) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        42 * /* No Expression */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 14);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_unary_expression_recursive) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        - /* No Expression */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 11);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_index_expression) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        name[ /* No Expression */ ]
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 15);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_close_bracket) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        name[ 42 /* No Close Bracket */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 18);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_identifier_after_dot) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        name. /* No Identifier */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 15);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_close_parenthesis) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        function( /* No Close Parenthesis */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 19);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_expression_after_comma_in_argument_list) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        function( 42, /* No Expression */ )
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 23);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_named_argument_after_comma_in_named_argument_list) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        function( name: 42, /* No Named Argument */ )
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 29);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_expression_in_named_argument) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        function( name: /* No Expression */ )
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 25);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_expression_in_primary_parenthesis) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        ( /* No Expression */ )
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 11);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_closing_parenthesis_in_primary_parenthesis) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        ( 42 /* No Closing Parenthesis */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 14);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_closing_brace_in_dictionary) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        { /* No Closing Brace */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 11);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_closing_bracket_in_array) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        [ /* No Closing Bracket */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 11);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_identifier_after_at) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        @ /* No Identifier */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 11);
}


/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(expression_parser_require_expression) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        /* No Expression */
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 9);
}

/**************************************************************************************************/
