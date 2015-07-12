/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

// stdc++
#include <cfloat>
#include <iostream>
#include <vector>

#define BOOST_TEST_MAIN

// boost
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunneeded-internal-declaration"
#include <boost/test/unit_test.hpp>
#pragma clang diagnostic pop

// asl
#include <adobe/algorithm/for_each.hpp>
#include <adobe/any_regular.hpp>
#include <adobe/string/to_string.hpp>

// double-conversion
#ifdef ADOBE_BUILT_WITH_CMAKE
#include <double-conversion/double-conversion.h>
#else
#include <double-conversion/src/double-conversion.h>
#endif

/******************************************************************************/

using namespace std;

/******************************************************************************/

BOOST_AUTO_TEST_CASE(to_string_smoke) {
    static_assert(DBL_MAX == 1.7976931348623157e+308, "Unexpected DBL_MAX");

    {
    char buf[32] = { 0 };
    adobe::to_string(atof("0.1"), &buf[0], false);
    assert(strcmp(&buf[0], "0.1") == 0);
    }

    {
    char buf[32] = { 0 };
    adobe::to_string(DBL_MAX, &buf[0], false);
    assert(atof(&buf[0]) != DBL_MAX);
    }

    {
    char buf[32] = { 0 };
    adobe::to_string(atof("0.1"), &buf[0], true);
    assert(strcmp(&buf[0], "0.1") != 0);
    }

    {
    char buf[32] = { 0 };
    adobe::to_string(DBL_MAX, &buf[0], true);
    assert(atof(&buf[0]) == DBL_MAX);
    }
}

/******************************************************************************/

struct test_t {
    double      value_m;
    const char* tag_m;
};

/******************************************************************************/

test_t suite_g[] = {
    { std::atof("0.000001e-308"), "atof(0.000001e-308)" },
    { std::atof("0.1"), "atof(0.1)" },
    { 1.1, "1.1" },
    { DBL_MIN, "DBL_MIN" },
    { DBL_MAX, "DBL_MAX" },
    { __DBL_DENORM_MIN__, "__DBL_DENORM_MIN__" }
};

/******************************************************************************/

template <bool Precise>
inline void asl_to_string(const test_t& test) {
    adobe::to_string(test.value_m, ostream_iterator<char>(std::cout), Precise);
}

inline void double_conversion_serialization(const test_t& test) {
    using namespace double_conversion;
    const DoubleToStringConverter& c = DoubleToStringConverter::EcmaScriptConverter();
    char buf[32] = { 0 };
    StringBuilder builder(buf, sizeof(buf));
    c.ToShortest(test.value_m, &builder);
    std::cout << builder.Finalize();
}

inline void any_regular_serialization(const test_t& test) {
    std::cout << adobe::any_regular_t(test.value_m);
}

/******************************************************************************/

template <typename Converter>
void test_suite(Converter convert, const char* label) {
    std::cout << '\n';
    std::cout << label << ":\n";
    for (const auto& test : suite_g) {
        std::cout << "  " << test.tag_m << ": ";
        convert(test);
        std::cout << '\n';
    }
}

/******************************************************************************/

BOOST_AUTO_TEST_CASE(serialization_test_suite) {
    test_suite(&asl_to_string<true>, "adobe::to_string precise");

    test_suite(&asl_to_string<false>, "adobe::to_string short");

    test_suite(&double_conversion_serialization, "double-conversion");

    test_suite(&any_regular_serialization, "adobe::any_regular_t");
}

/******************************************************************************/
