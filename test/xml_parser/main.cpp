/*
 Copyright 2006-2007 Adobe Systems Incorporated
 Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
 or a copy at http://stlab.adobe.com/licenses.html)
 */

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/xml_parser.hpp>
#include <adobe/xstring.hpp>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <exception>
#include <iostream>
#include <vector>

/****************************************************************************************************/

namespace
{

/****************************************************************************************************/

struct math_test_t
{
	math_test_t() : observed_m(0), expected_m(0) {}
	
	long	observed_m;
	long	expected_m;
};

/****************************************************************************************************/

const char* const test_document_1 =
					"<math-test>\n"
					"	<expression>\n"
					"		<operand value=\"15\"/>\n"
					"		<operand value=\"10\"/>\n"
					"		<add/>\n"
					"	</expression>\n"
					"	<result value=\"25\"/>\n"
					"</math-test>";

const char* const test_document_2 =
					"<math-test>/n"
					"	<expression>/n"
					"		<operand value=\"3\"/>/n"
					"		<operand value=\"5\"/>/n"
					"		<multiply/>/n"
					"	</expression>/n"
					"	<result value=\"15\"/>/n"
					"</math-test>/n";

const char* const test_document_3 = 
					"<?xml version=\"1.0\" encoding=\"utf8\" ?>\n"
					"<math-test>\n"
					"	<expression>\n"
					"		<operand value=\"2\"/>\n"
					"		<operand value=\"3\"/>\n"
					"		<expression>\n"
					"			<operand value=\"4\"/>\n"
					"			<operand value=\"5\"/>\n"
					"			<multiply/>\n"
					"		</expression>\n"
					"		<add/>\n"
					"		<multiply/>\n"
					"	</expression>\n"
					"	<result value=\"46\"/>\n"
					"</math-test>\n";

/****************************************************************************************************/

long to_long(const adobe::token_range_t& value);

long calculate_expression(const adobe::token_range_t& content);

adobe::token_range_t document_content(const adobe::token_range_t&	entire_element_range,
									  const adobe::token_range_t&	name,
									  const adobe::attribute_set_t&	attribute_set,
									  const adobe::token_range_t&	value,
									  bool&							passed);

adobe::token_range_t test_content(const adobe::token_range_t&	entire_element_range,
								  const adobe::token_range_t&	name,
								  const adobe::attribute_set_t&	attribute_set,
								  const adobe::token_range_t&	value,
								  math_test_t&					test);

adobe::token_range_t expression_content(const adobe::token_range_t&		entire_element_range,
										const adobe::token_range_t&		name,
										const adobe::attribute_set_t&	attribute_set,
										const adobe::token_range_t&		value,
										std::vector<long>&				stack);

bool run_test(const adobe::token_range_t& document, const adobe::line_position_t& line_position);

/****************************************************************************************************/

} // anonymous namespace

int main()
{
	bool success = true;

	try
	{
		adobe::token_range_t document;
		adobe::line_position_t line_position;
		bool test_passed;
		
		// REVISIT - this repetitive pattern screams out to be refactored
		
		document = adobe::static_token_range(test_document_1);
		line_position = adobe::line_position_t("test document 1");
		test_passed = run_test(document, line_position);
		std::cout << "XML Test 1 " << (test_passed ? "passed" : "failed") << std::endl;
		success = success && test_passed;

		document = adobe::static_token_range(test_document_2);
		line_position = adobe::line_position_t("test document 2");
		test_passed = run_test(document, line_position);
		std::cout << "XML Test 2 " << (test_passed ? "passed" : "failed") << std::endl;
		success = success && test_passed;

		document = adobe::static_token_range(test_document_3);
		line_position = adobe::line_position_t("test document 3");
		test_passed = run_test(document, line_position);
		std::cout << "XML Test 3 " << (test_passed ? "passed" : "failed") << std::endl;
		success = success && test_passed;
	}
	catch(const std::exception& e)
	{
		std::cout << "Caught exception: '" << e.what() << "'" << std::endl;
		success = false;
	}

	return (success ? 0 : 1);
}

namespace {

/****************************************************************************************************/

long to_long(const adobe::token_range_t& value)
{
	const std::string value_string(value.first, value.second);
	
	return boost::lexical_cast<long>(value_string);
}

long calculate_expression(const adobe::token_range_t& content)
{
	std::vector<long> value_stack;
	
	adobe::make_xml_parser(content.first, content.second,
						   adobe::line_position_t("expression"),
						   adobe::always_true<adobe::token_range_t>(),
						   boost::bind(expression_content, _1, _2, _3, _4,
									   boost::ref(value_stack)),
						   adobe::implementation::null_output_t())
		.parse_content();
	
	return value_stack.back();
}

adobe::token_range_t document_content(const adobe::token_range_t&	/*entire_element_range*/,
									  const adobe::token_range_t&	name,
									  const adobe::attribute_set_t&	/*attribute_set*/,
									  const adobe::token_range_t&	value,
									  bool&							passed)
{
	static const adobe::token_range_t math_test_token_k( adobe::static_token_range("math-test") );

	passed = false;
	
	if (adobe::token_range_equal(name, math_test_token_k))
	{
		math_test_t	test;

		adobe::make_xml_parser(value.first, value.second,
							   adobe::line_position_t("math-test"),
							   adobe::always_true<adobe::token_range_t>(),
							   boost::bind(test_content, _1, _2, _3, _4,
										   boost::ref(test)),
							   adobe::implementation::null_output_t())
			.parse_content();
		
		if (test.observed_m == test.expected_m)
			passed = true;
	}
		
	return adobe::token_range_t();
}

adobe::token_range_t test_content(const adobe::token_range_t&	/*entire_element_range*/,
								  const adobe::token_range_t&	name,
								  const adobe::attribute_set_t&	attribute_set,
								  const adobe::token_range_t&	value,
								  math_test_t&					test)
{
	static const adobe::token_range_t expression_token_k( adobe::static_token_range("expression") );
	static const adobe::token_range_t result_token_k( adobe::static_token_range("result") );
	static const adobe::token_range_t value_attr_k( adobe::static_token_range("value") );

	if (adobe::token_range_equal(name, expression_token_k))
	{		
		test.observed_m = calculate_expression(value);
	}
	else if (adobe::token_range_equal(name, result_token_k))
	{
		const adobe::token_range_t value_attr( attribute_set[value_attr_k] );
		
		if (0 == boost::size(value_attr))
			throw std::runtime_error("result element is missing value attribute");
		
		test.expected_m = to_long(value_attr);
	}
	else
	{
		throw std::runtime_error("unexpected element encountered in math-test");
	}
	
	return adobe::token_range_t();
}

adobe::token_range_t expression_content(const adobe::token_range_t&		/*entire_element_range*/,
										const adobe::token_range_t&		name,
										const adobe::attribute_set_t&	attribute_set,
										const adobe::token_range_t&		value,
										std::vector<long>&				stack)
{
	static const adobe::token_range_t expression_token_k( adobe::static_token_range("expression") );
	static const adobe::token_range_t operand_token_k( adobe::static_token_range("operand") );
	static const adobe::token_range_t add_token_k( adobe::static_token_range("add") );
	static const adobe::token_range_t multiply_token_k( adobe::static_token_range("multiply") );
	static const adobe::token_range_t value_attr_k( adobe::static_token_range("value") );

	if (adobe::token_range_equal(name, expression_token_k))
	{
		stack.push_back( calculate_expression(value) );
	}
	else if (adobe::token_range_equal(name, operand_token_k))
	{
		const adobe::token_range_t value_attr( attribute_set[value_attr_k] );
		
		if (0 == boost::size(value_attr))
			throw std::runtime_error("result element is missing value attribute");
		
		stack.push_back( to_long(value_attr) );
	}
	else if (adobe::token_range_equal(name, add_token_k))
	{
		const long operand_a = stack.back();
		stack.pop_back();
		
		const long operand_b = stack.back();
		stack.pop_back();
		
		stack.push_back(operand_a + operand_b);
	}
	else if (adobe::token_range_equal(name, multiply_token_k))
	{
		const long operand_a = stack.back();
		stack.pop_back();
		
		const long operand_b = stack.back();
		stack.pop_back();
		
		stack.push_back(operand_a * operand_b);
	}
	else
	{
		throw std::runtime_error("unexpected element encountered in expression");
	}
	
	return adobe::token_range_t();
}

bool run_test(const adobe::token_range_t& document, const adobe::line_position_t& line_position)
{
	bool passed = false;

	adobe::make_xml_parser(document.first, document.second,
						   line_position,
						   adobe::always_true<adobe::token_range_t>(),
						   boost::bind(document_content, _1, _2, _3, _4,
									   boost::ref(passed)),
						   adobe::implementation::null_output_t())
		.parse_document();

	return passed;
}

/****************************************************************************************************/

} // anonymous namespace
