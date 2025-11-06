/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/virtual_machine.hpp>

#include <adobe/array.hpp>
#include <adobe/expression_parser.hpp>

#include <sstream>
#include <string>

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace adobe;

BOOST_AUTO_TEST_CASE(index_operator) {
    const line_position_t expression_position{__FILE__, __LINE__ + 1};
    constexpr const char* expression = R"(
        ["a", "correct", "c"][1]
    )";
    istringstream stream{expression};
    expression_parser exp(stream, expression_position);
    array_t instructions;
    exp.require_expression(instructions);

    virtual_machine_t vm;
    vm.evaluate(instructions);
    BOOST_TEST(cast<string>(vm.back()) == "correct");
}
