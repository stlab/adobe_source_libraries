/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_JSON_HELPER_HPP
#define ADOBE_JSON_HELPER_HPP

#include <adobe/any_regular.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/json.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/**
    \ingroup json

    \brief A utility class can be used with \ref json_parser and
           \ref json_generator to round trip JSON data through the common ASL
            data structures.
*/
struct asl_json_helper_t {
    /*
    We use std::string as key type instead of adobe::name_t because of the
    append, move, etc., capabilities of the former type. When we go to
    insert a key/value pair into the dictionary we convert the key to a name_t
    first. This is a better route to take than building out name_t to behave
    more like a string type, which it is not intended to be.
    */
    typedef adobe::any_regular_t value_type;
    typedef std::string string_type;
    typedef string_type key_type;
    typedef adobe::dictionary_t object_type;
    typedef adobe::array_t array_type;
    typedef object_type::value_type pair_type;

    static json_type type(const value_type& x) {
        const std::type_info& type(x.type_info());

        if (type == typeid(object_type))
            return json_type::object;
        else if (type == typeid(array_type))
            return json_type::array;
        else if (type == typeid(string_type))
            return json_type::string;
        else if (type == typeid(double))
            return json_type::number;
        else if (type == typeid(bool))
            return json_type::boolean;
        else if (type == typeid(adobe::empty_t))
            return json_type::null;

        ADOBE_ASSERT(false && "invalid type for serialization");
    }

    template <typename T>
    static const T& as(const value_type& x) {
        return x.cast<T>();
    }

    static void move_append(object_type& obj, key_type& key, value_type& value) {
        obj[adobe::name_t(key.c_str())] = std::move(value);
        key.clear();
    }
    static void append(string_type& str, const char* f, const char* l) { str.append(f, l); }
    static void move_append(array_type& array, value_type& value) {
        array.emplace_back(std::move(value));
    }
};

/**************************************************************************************************/
/**
    \ingroup json

    \brief A utility routine that takes in raw JSON data and produces a
           representation of that data using the common ASL data
           structures.

    \param data raw JSON data.

    \return Either a \ref array_t or a \ref `dictionary_t` representing the
            raw data parsed, wrapped in an `any_regular_t`.
*/
inline adobe::any_regular_t json_parse(const char* data) {
    return json_parser<asl_json_helper_t>(data).parse();
}

/**
    \ingroup json

    \brief A utility routine that takes in an any_regular_t and produces from it
           a well-formed representation of the structure in JSON.

    \note The only types that will be accepted during JSON generation are:
        - `array_t` (array)
        - `dictionary_t` (object)
        - `std::string` (string)
        - `double` (double or int)
        - `bool` (bool)
        - `empty_t` (null)

    \param x The encapsulated structure. This root structure must be either an
             `array_t` or a `dictionary_t`.
    \param out Output iterator to which the representative JSON will be copied.

    \return The output iterator passed in.
*/
template <typename O>
inline O json_generate(const adobe::any_regular_t& x, O out) {
    return json_generator<asl_json_helper_t, O>(out).generate(x);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
// ADOBE_JSON_HELPER_HPP
#endif

/**************************************************************************************************/
