/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_TOKEN_HPP
#define ADOBE_TOKEN_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/name.hpp>

/**************************************************************************************************/

/*
REVISIT (sparent) : This code is an implementation detail and should be moved into an implementation
namespace but at this moment that would require too many changes.
*/

namespace adobe {

/**************************************************************************************************/


// Operators
constexpr static_name_t add_k = ".add"_name;
constexpr static_name_t and_k = ".and"_name;
constexpr static_name_t array_k = ".array"_name;
constexpr static_name_t at_k = ".at"_name;
constexpr static_name_t bitwise_and_k = ".bitwise_and"_name;
constexpr static_name_t bitwise_lshift_k = ".bitwise_lshift"_name;
constexpr static_name_t bitwise_negate_k = ".bitwise_negate"_name;
constexpr static_name_t bitwise_or_k = ".bitwise_or"_name;
constexpr static_name_t bitwise_rshift_k = ".bitwise_rshift"_name;
constexpr static_name_t bitwise_xor_k = ".bitwise_xor"_name;
constexpr static_name_t dictionary_k = ".dictionary"_name;
constexpr static_name_t divide_k = ".divide"_name;
constexpr static_name_t equal_k = ".equal"_name;
constexpr static_name_t function_k = ".function"_name;
constexpr static_name_t greater_equal_k = ".greater_equal"_name;
constexpr static_name_t greater_k = ".greater"_name;
constexpr static_name_t ifelse_k = ".ifelse"_name;
constexpr static_name_t index_k = ".index"_name;
constexpr static_name_t is_k = ".is"_name;
constexpr static_name_t less_equal_k = ".less_equal"_name;
constexpr static_name_t less_k = ".less"_name;
constexpr static_name_t modulus_k = ".modulus"_name;
constexpr static_name_t multiply_k = ".multiply"_name;
constexpr static_name_t not_equal_k = ".not_equal"_name;
constexpr static_name_t not_k = ".not"_name;
constexpr static_name_t or_k = ".or"_name;
constexpr static_name_t subtract_k = ".subtract"_name;
constexpr static_name_t to_k = ".to"_name;
constexpr static_name_t unary_negate_k = ".unary_negate"_name;
constexpr static_name_t variable_k = ".variable"_name;

// tokens
constexpr static_name_t assign_k = "assign"_name;
constexpr static_name_t close_brace_k = "close_brace"_name;
constexpr static_name_t close_bracket_k = "close_bracket"_name;
constexpr static_name_t close_parenthesis_k = "close_parenthesis"_name;
constexpr static_name_t colon_k = "colon"_name;
constexpr static_name_t comma_k = "comma"_name;
constexpr static_name_t dot_k = "dot"_name;
constexpr static_name_t empty_k = "empty"_name;
constexpr static_name_t false_k = "false"_name;
constexpr static_name_t identifier_k = "identifier"_name;
constexpr static_name_t keyword_k = "keyword"_name;
constexpr static_name_t lead_comment_k = "lead_comment"_name;
constexpr static_name_t number_k = "number"_name;
constexpr static_name_t open_brace_k = "open_brace"_name;
constexpr static_name_t open_bracket_k = "open_bracket"_name;
constexpr static_name_t open_parenthesis_k = "open_parenthesis"_name;
constexpr static_name_t question_k = "question"_name;
constexpr static_name_t semicolon_k = "semicolon"_name;
constexpr static_name_t string_k = "string"_name;
constexpr static_name_t trail_comment_k = "trail_comment"_name;
constexpr static_name_t true_k = "true"_name;

/// Given a name that represents a token, return the string representation of that token.
/// Precondition: `token` is a valid token name.
auto token_to_string(name_t token) -> const char*;

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
