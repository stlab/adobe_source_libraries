/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

// stdc++
#include <iostream>
#include <iterator>
#include <vector>

// boost
#include <boost/test/unit_test.hpp>

// asl
#include <adobe/json_helper.hpp>

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(asl_json_floating_point_parsing) {
    struct test_t {
        const char* str;
        double expected;
    };

    test_t tests[] = {
        {"[42]", 42},
        {"[12.536]", 12.536},
        {"[-20.5]", -20.5},
        {"[-1.375e+112]", -1.375e+112},
        {"[3.1415926535897932384626433832795028841971693993751058209]", M_PI},
    };

    for (const auto& test : tests) {
        adobe::any_regular_t x = adobe::json_parse(test.str);
        double d = x.cast<adobe::array_t>()[0].cast<double>();
        BOOST_CHECK_CLOSE(d, test.expected, 0.000001);
    }
}

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(asl_json_helper_smoke) {
    std::cout << "-=-=- asl_json_helper_smoke -=-=-\n";
    adobe::any_regular_t x = adobe::json_parse(u8R"raw(
        [
            42,
            12.536,
            -20.5,
            -1.375e+112,
            3.1415926535897932384626433832795028841971693993751058209,
            null,
            {
                "Country": "US",
                "State": "CA",
                "Address": "",
                "Longitude": -122.3959,
                "Zip": "94107",
                "City": "SAN FRANCISCO",
                "Latitude": 37.7668,
                "precision": "zip"
            },
            {
                "precision": "zip",
                "Latitude": 37.371991,
                "City": "SUNNYVALE",
                "Zip": "94085",
                "Longitude": -122.02602,
                "Address": "",
                "State": "CA",
                "Country": "US"
            }
        ]
    )raw");

    adobe::json_generate(x, std::ostream_iterator<char>(std::cout));
}

/**************************************************************************************************/
