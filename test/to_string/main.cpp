/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

// stdc++
#include <cfloat>
#include <charconv>
#include <iostream>
#include <limits>
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

/******************************************************************************/

using namespace std;

/******************************************************************************/

BOOST_AUTO_TEST_CASE(to_string_smoke) {
    static_assert(DBL_MAX == 1.7976931348623157e+308, "Unexpected DBL_MAX");

    {
        char buf[32] = {0};
        adobe::to_string(atof("0.1"), &buf[0], false);
        BOOST_CHECK_EQUAL(strcmp(&buf[0], "0.1"), 0);
    }

    {
        char buf[32] = {0};
        adobe::to_string(DBL_MAX, &buf[0], false);
        BOOST_CHECK(atof(&buf[0]) != DBL_MAX);
    }

    {
        char buf[32] = {0};
        adobe::to_string(atof("0.1"), &buf[0], true);
        BOOST_CHECK(strcmp(&buf[0], "0.1") != 0);
    }

    {
        char buf[32] = {0};
        adobe::to_string(DBL_MAX, &buf[0], true);
        BOOST_CHECK_EQUAL(atof(&buf[0]), DBL_MAX);
    }
}

/******************************************************************************/

struct test_t {
    double value_m;
    const char* dconv_str_m;
};

/******************************************************************************/
/*
    I have updated the `tag_m` in this structure to `dconv_str_m`, and changed its values to be
    what `EcmaScriptConverter` would output for each value. This way we have some guidance on just
    how far off each serialization option is from the previous known-good implementation.
*/
test_t suite_g[] = {
    {std::atof("0.000001e-308"), "1e-314"},
    {std::atof("0.1"), "0.1"},
    {1.1, "1.1"},
    {0.000001, "0.000001"},
    {0.0000001, "1e-7"},
    {111111111111111111111.0, "111111111111111110000"},
    {100000000000000000000.0, "100000000000000000000"},
    {1111111111111111111111.0, "1.1111111111111111e+21"},
    {std::numeric_limits<double>::min(), "2.2250738585072014e-308"},
    {std::numeric_limits<double>::max(), "1.7976931348623157e+308"},
    {std::numeric_limits<double>::denorm_min(), "5e-324"},
    {std::numeric_limits<double>::infinity(), "Infinity"},
    {-std::numeric_limits<double>::infinity(), "-Infinity"},
    {std::numeric_limits<double>::quiet_NaN(), "NaN"},
    {std::numeric_limits<double>::signaling_NaN(), "NaN"},
};

/******************************************************************************/

template <bool Precise>
std::string asl_to_string_v1(const test_t& test) {
    if (!std::isfinite(test.value_m)) {
        return "<unsupported>";
    }

    std::string result;
    adobe::to_string(test.value_m, std::back_inserter(result), Precise);
    return result;
}

std::string asl_to_string(const test_t& test) {
    return adobe::to_string(test.value_m);
}

std::string std_to_chars(const test_t& test) {
#if defined(_LIBCPP_AVAILABILITY_HAS_NO_TO_CHARS_FLOATING_POINT)
    return "unavailable";
#else
    std::array<char, 64> str;
    if (auto [ptr, ec] = std::to_chars(str.data(), str.data() + str.size(), test.value_m); ec == std::errc()) {
        const auto sz = ptr - str.data();
        return std::string(str.data(), sz);
    } else {
        return std::make_error_code(ec).message();
    }
#endif
}

std::string any_regular_serialization(const test_t& test) {
    std::stringstream ss;
    ss << adobe::any_regular_t(test.value_m);
    return std::move(ss).str();
}

/******************************************************************************/

std::size_t test_suite(std::string (*convert)(const test_t& test), const char* label) {
    std::size_t match_count = 0;
    std::cout << '\n';
    std::cout << label << ":\n";
    for (const auto& test : suite_g) {
        const auto converted = convert(test);
        const bool match = converted == test.dconv_str_m;
        match_count += match;
        std::cout << (match ? "  MATCH `" : "  DIFFR `") << test.dconv_str_m << '`';
        if (!match) {
            std::cout << "; got `" << converted << '`';
        }
        std::cout << '\n';
    }
    std::cout << "  MATCH_COUNT: " << match_count << "\n";
    return match_count;
}

/******************************************************************************/

BOOST_AUTO_TEST_CASE(serialization_test_suite) {
#if ADOBE_HAS_TO_CHARS_FP()
    constexpr auto to_string_match_count_k = 10;
    constexpr auto to_chars_match_count_k = 6;
#else
    constexpr auto to_string_match_count_k = 9;
    constexpr auto to_chars_match_count_k = 0;
#endif

    BOOST_CHECK_EQUAL(test_suite(&asl_to_string_v1<true>, "adobe::to_string (precise)"), 3);

    BOOST_CHECK_EQUAL(test_suite(&asl_to_string_v1<false>, "adobe::to_string (short)"), 2);

    BOOST_CHECK_EQUAL(test_suite(&asl_to_string, "adobe::to_string (v2)"), to_string_match_count_k);

    BOOST_CHECK_EQUAL(test_suite(&std_to_chars, "std::to_chars"), to_chars_match_count_k);

    BOOST_CHECK_EQUAL(test_suite(&any_regular_serialization, "adobe::any_regular_t"), to_string_match_count_k);
}

/******************************************************************************/
