/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_JSON_HELPER_HPP
#define ADOBE_JSON_HELPER_HPP

#include <adobe/json.hpp>
#include <adobe/any_regular.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

struct asl_json_helper_t {
    /*
    We use std::string as key type instead of adobe::name_t because of the
    append, move, etc., capabilities of the former type. When we go to
    insert a key/value pair into the dictionary we convert the key to a name_t
    first. This is a better route to take than building out name_t to behave
    more like a string type, which it is not intended to be.
    */
    typedef adobe::any_regular_t    value_type;
    typedef std::string             string_type;
    typedef string_type             key_type;
    typedef adobe::dictionary_t     object_type;
    typedef adobe::array_t          array_type;
    typedef object_type::value_type pair_type;

    static json_type type(const value_type& x) {
        const std::type_info& type(x.type_info());

        if (type == typeid(object_type)) return json_type::object;
        else if (type == typeid(array_type)) return json_type::array;
        else if (type == typeid(string_type)) return json_type::string;
        else if (type == typeid(double)) return json_type::number;
        else if (type == typeid(bool)) return json_type::boolean;
        else if (type == typeid(adobe::empty_t)) return json_type::null;

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
    static void append(string_type& str, const char* f, const char* l) {
        str.append(f, l);
    }
    static void move_append(array_type& array, value_type& value) {
        array.emplace_back(std::move(value));
    }
};

/**************************************************************************************************/

inline adobe::any_regular_t json_parse(const char* data)
{
    return json_parser<asl_json_helper_t>(data).parse();
}

template <typename O>
inline O json_generate(const adobe::any_regular_t& x, O out)
{
    return json_generator<asl_json_helper_t, O>(out).generate(x);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
// ADOBE_JSON_HELPER_HPP
#endif

/**************************************************************************************************/
