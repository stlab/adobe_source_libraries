/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/implementation/expression_parser.hpp>

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <sstream>

/**************************************************************************************************/

using namespace adobe;
using namespace std;

/**************************************************************************************************/

const line_position_t expression_position{__FILE__, __LINE__ + 1};
constexpr const char* expression = "1 + 2 * 3";

BOOST_AUTO_TEST_CASE(expression_parser_test) {
    istringstream expression_stream(expression);
    expression_parser exp(expression_stream, expression_position);
}
