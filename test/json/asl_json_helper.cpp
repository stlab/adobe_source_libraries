/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

// stdc++
#include <vector>
#include <iostream>

// boost
#include <boost/test/unit_test.hpp>

// asl
#include <adobe/any_regular.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/iomanip.hpp>
#include <adobe/iomanip_asl_cel.hpp>
#include <adobe/json.hpp>

/******************************************************************************/

struct helper_t {
    typedef adobe::any_regular_t    value_type;
    typedef std::string             string_type;
    typedef string_type             key_type;
    typedef adobe::dictionary_t     object_type;
    typedef adobe::array_t          array_type;
    typedef object_type::value_type pair_type;

    static json_type type(const value_type& x) {
        const std::type_info& type(x.type_info());

        if (type == typeid(object_type))      return json_type::object;
        else if (type == typeid(array_type))  return json_type::array;
        else if (type == typeid(string_type)) return json_type::string;
        else if (type == typeid(double))      return json_type::number;
        else if (type == typeid(bool))        return json_type::boolean;
        else if (type == typeid(adobe::empty_t)) return json_type::null;

        assert(false && "invalid type for serialization");
    }
    
    template <typename T>
    static const T& as(const value_type& x) { return x.cast<T>(); }

    static void move_append(object_type& obj, key_type& key, value_type& value) {
        // REVISIT (fbrereto) : dictionary needs move semantics
        obj[adobe::name_t(key.c_str())].assign(std::move(value));
    }
    static void append(string_type& str, const char* f, const char* l) {
        str.append(f, l);
    }
    static void move_append(array_type& array, value_type& value) {
        // REVISIT (fbrereto) : array needs move semantics
        array.push_back(std::move(value));
    }
};

/******************************************************************************/

BOOST_AUTO_TEST_CASE(asl_json_helper_smoke) {
    std::cout << "-=-=- asl_json_helper_smoke -=-=-\n";
    adobe::any_regular_t x = json_parser_t<helper_t>(u8R"raw(
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
    )raw").parse();
    
    json_generator<helper_t, std::ostream_iterator<char>>(std::ostream_iterator<char>(std::cout)).generate(x);

    // std::cout << adobe::begin_asl_cel << x << adobe::end_asl_cel << '\n';
}

/******************************************************************************/
