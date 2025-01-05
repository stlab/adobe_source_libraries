/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/implementation/lex_stream.hpp>

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "lex_stream_test.h"
#include <sstream>

/**************************************************************************************************/

using namespace adobe;
using namespace std;

/**************************************************************************************************/

void exception_test(const char* const expression,
                    const adobe::line_position_t& expression_position,
                    int expected_line_number,
                    std::streampos expected_character_position) {
    istringstream expression_stream(expression);
    lex_stream_t lex(expression_stream, expression_position);
    bool caught(false);
    try {
        auto token = lex.get();
    } catch (const stream_error_t& error) {
        cerr << error;
        caught = true;
        BOOST_REQUIRE_EQUAL(error.line_position_set().size(), 1u);
        const auto& error_position = error.line_position_set().front();
        BOOST_REQUIRE_EQUAL(error_position.line_number_m,
                            expected_line_number);
        BOOST_REQUIRE_LT(error_position.line_start_m, error_position.position_m);
        BOOST_REQUIRE_EQUAL(error_position.position_m - error_position.line_start_m + 1, expected_character_position);
    }
    BOOST_REQUIRE(caught);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(lex_stream_eof_in_string) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        "This is a string that never ends
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 9);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(lex_stream_eof_in_comment) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        /*
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 9);
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(lex_stream_unexpected_character) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        $
    )";

    exception_test(expression, expression_position, expression_position.line_number_m + 1, 9);
}

/**************************************************************************************************/
