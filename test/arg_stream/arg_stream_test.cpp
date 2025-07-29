/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

#include <adobe/config.hpp>

// boost
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

// asl
#include <adobe/arg_stream.hpp>

// std
#include <string>

BOOST_AUTO_TEST_CASE(arg_stream_single_basic_functionality) {
    // Test basic construction and single use
    adobe::arg_stream::single<int> stream(42);
    
    // Should not be at EOF initially
    BOOST_CHECK(!stream.eof());
    
    // Get the value once
    int value = stream.get_next_arg<int>();
    BOOST_CHECK_EQUAL(value, 42);
    
    // Should be at EOF after single use
    BOOST_CHECK(stream.eof());
    
    // Should throw no_more_args if we try to get another arg
    BOOST_CHECK_THROW(stream.get_next_arg<int>(), adobe::arg_stream::no_more_args);
}

BOOST_AUTO_TEST_CASE(arg_stream_single_multiple_count) {
    // Test with repeat count
    adobe::arg_stream::single<std::string> stream("hello", 3);
    
    // Should not be at EOF initially
    BOOST_CHECK(!stream.eof());
    
    // Get the value three times
    for (int i = 0; i < 3; ++i) {
        BOOST_CHECK(!stream.eof());
        std::string value = stream.get_next_arg<std::string>();
        BOOST_CHECK_EQUAL(value, "hello");
    }
    
    // Should be at EOF after three uses
    BOOST_CHECK(stream.eof());
    
    // Should throw no_more_args if we try to get another arg
    BOOST_CHECK_THROW(stream.get_next_arg<std::string>(), adobe::arg_stream::no_more_args);
}

BOOST_AUTO_TEST_CASE(arg_stream_single_type_conversion) {
    // Test implicit type conversions
    adobe::arg_stream::single<int> stream(42);
    
    // Convert int to double
    double d_value = stream.get_next_arg<double>();
    BOOST_CHECK_EQUAL(d_value, 42.0);
}

BOOST_AUTO_TEST_CASE(arg_stream_single_zero_count) {
    // Test with zero repeat count
    adobe::arg_stream::single<int> stream(42, 0);
    
    // Should be at EOF immediately
    BOOST_CHECK(stream.eof());
    
    // Should throw no_more_args immediately
    BOOST_CHECK_THROW(stream.get_next_arg<int>(), adobe::arg_stream::no_more_args);
}

BOOST_AUTO_TEST_CASE(arg_stream_single_copy_semantics) {
    // Test that the value is properly stored and accessed
    std::string original = "test_string";
    adobe::arg_stream::single<std::string> stream(original);
    
    std::string retrieved = stream.get_next_arg<std::string>();
    BOOST_CHECK_EQUAL(retrieved, original);
    
    // Original should be unchanged
    BOOST_CHECK_EQUAL(original, "test_string");
}

BOOST_AUTO_TEST_CASE(arg_stream_single_reference_type) {
    // Test with reference types
    int original = 100;
    adobe::arg_stream::single<int> stream(original, 2);
    
    // Modify original
    original = 200;
    
    // Stream should still have the original value (100) since it stores by value
    int first = stream.get_next_arg<int>();
    int second = stream.get_next_arg<int>();
    BOOST_CHECK_EQUAL(first, 100);
    BOOST_CHECK_EQUAL(second, 100);
}

// Test functions for call tests
int add_two_ints(int a, int b) {
    return a + b;
}

int add_three_ints(int a, int b, int c) {
    return a + b + c;
}

double multiply_double_int(double d, int i) {
    return d * i;
}

void void_function(int value) {
    // Just consume the value, nothing to return
}

std::string concat_strings(const std::string& a, const std::string& b) {
    return a + b;
}

// Test class for member function tests
class TestClass {
public:
    int value;
    TestClass(int v) : value(v) {}
    
    int add_to_value(int x) {
        return value + x;
    }
    
    int multiply_values(int x, int y) {
        return value * x * y;
    }
};

BOOST_AUTO_TEST_CASE(arg_stream_call_basic_function) {
    // Test calling a simple function with two arguments
    adobe::arg_stream::single<int> stream1(10, 2);
    
    int result = adobe::arg_stream::call(add_two_ints, stream1);
    BOOST_CHECK_EQUAL(result, 20); // 10 + 10
}

BOOST_AUTO_TEST_CASE(arg_stream_call_mixed_types) {
    // Test calling a function with mixed argument types
    adobe::arg_stream::single<double> double_stream(2.5);
    adobe::arg_stream::single<int> int_stream(4);
    
    auto chained = adobe::arg_stream::make_chain(double_stream, int_stream);
    
    double result = adobe::arg_stream::call(multiply_double_int, chained);
    BOOST_CHECK_EQUAL(result, 10.0); // 2.5 * 4
}

BOOST_AUTO_TEST_CASE(arg_stream_call_void_function) {
    // Test calling a void function
    adobe::arg_stream::single<int> stream(42);
    
    // Should not throw and should consume the argument
    adobe::arg_stream::call(void_function, stream);
    
    // Stream should be exhausted
    BOOST_CHECK(stream.eof());
}

BOOST_AUTO_TEST_CASE(arg_stream_call_string_function) {
    // Test calling a function with string arguments
    adobe::arg_stream::single<std::string> stream1("Hello");
    adobe::arg_stream::single<std::string> stream2(" World");
    
    auto chained = adobe::arg_stream::make_chain(stream1, stream2);
    
    std::string result = adobe::arg_stream::call(concat_strings, chained);
    BOOST_CHECK_EQUAL(result, "Hello World");
}

BOOST_AUTO_TEST_CASE(arg_stream_call_member_function) {
    // Test calling member functions
    TestClass obj(5);
    adobe::arg_stream::single<int> stream(10);
    
    int result = adobe::arg_stream::call(&obj, &TestClass::add_to_value, stream);
    BOOST_CHECK_EQUAL(result, 15); // 5 + 10
}

BOOST_AUTO_TEST_CASE(arg_stream_call_member_function_multiple_args) {
    // Test calling member function with multiple arguments
    TestClass obj(2);
    adobe::arg_stream::single<int> stream1(3);
    adobe::arg_stream::single<int> stream2(4);
    
    auto chained = adobe::arg_stream::make_chain(stream1, stream2);
    
    int result = adobe::arg_stream::call(&obj, &TestClass::multiply_values, chained);
    BOOST_CHECK_EQUAL(result, 24); // 2 * 3 * 4
}

BOOST_AUTO_TEST_CASE(arg_stream_call_insufficient_args) {
    // Test that call throws when there are insufficient arguments
    adobe::arg_stream::single<int> stream(10, 1); // Only one argument available
    
    // Should throw no_more_args when trying to call a function requiring two arguments
    BOOST_CHECK_THROW(adobe::arg_stream::call(add_two_ints, stream), adobe::arg_stream::no_more_args);
}
