/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

// stdc++
#include <vector>
#include <iostream>

#define BOOST_TEST_MAIN

// boost
#include <boost/any.hpp>
#include <boost/test/unit_test.hpp>

// asl
#include <adobe/algorithm/for_each.hpp>
#include <adobe/algorithm/sort.hpp>
#include <adobe/algorithm/lower_bound.hpp>
#include <adobe/json.hpp>

/**************************************************************************************************/

using namespace std;
using namespace boost;
using namespace adobe;

/**************************************************************************************************/

struct any_json_helper_t {
    typedef any                                 value_type;
    typedef string                              key_type;
    typedef string                              string_type;
    typedef unordered_map<key_type, value_type> object_type;
    typedef vector<value_type>                  array_type;
    typedef object_type::value_type             pair_type;

    static json_type type(const value_type& x) {
        // REVISIT (make this thread safe) - nuke json_type and add functions to table directly.
        bool inited = false;

        static struct type_table_t {
            const std::type_info* type_info_;
            json_type type_;
        } table[] = {
            { &typeid(object_type),  json_type::object },
            { &typeid(array_type),   json_type::array },
            { &typeid(string_type),  json_type::string },
            { &typeid(double),       json_type::number },
            { &typeid(bool),         json_type::boolean },
            { &typeid(void),         json_type::null }
        };
        static const auto projection = [](const type_table_t& x)->const std::type_info& { return *x.type_info_; };

        if (!inited) {
            adobe::sort(table, adobe::less(), projection);
            inited = true;
        }

        auto p = adobe::lower_bound(table, x.type(), adobe::less(), projection);
        assert(p != end(table) && "invalid type for serialization");

        return p->type_;
    }

    template <typename T>
    static const T& as(const value_type& x) { return any_cast<const T&>(x); }

    static void move_append(object_type& obj, key_type& key, value_type& value) {
        obj.emplace(std::move(key), std::move(value));
    }
    static void append(string_type& str, const char* f, const char* l) {
        str.append(f, l);
    }
    static void move_append(array_type& array, value_type& value) {
        array.push_back(std::move(value));
    }
};

/**************************************************************************************************/

BOOST_AUTO_TEST_CASE(any_json_helper_smoke) {
    std::cout << "-=-=- any_json_helper_smoke -=-=-\n";
    any x = json_parser<any_json_helper_t>(u8R"raw(
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

    json_generator<any_json_helper_t, ostream_iterator<char>>(ostream_iterator<char>(cout)).generate(x);

    cout << endl;
}

/******************************************************************************/
