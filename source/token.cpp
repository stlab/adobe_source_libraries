/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/implementation/token.hpp>
#include <adobe/name.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

static_name_t ifelse_k               = ".ifelse"_name;
static_name_t number_k               = "number"_name;

static_name_t identifier_k           = "identifier"_name;
static_name_t string_k               = "string"_name;
static_name_t lead_comment_k         = "lead_comment"_name;
static_name_t trail_comment_k        = "trail_comment"_name;

static_name_t semicolon_k            = "semicolon"_name;
static_name_t comma_k                = "comma"_name;
static_name_t assign_k               = "assign"_name;
static_name_t question_k             = "question"_name;
static_name_t colon_k                = "colon"_name;
static_name_t open_brace_k           = "open_brace"_name;
static_name_t close_brace_k          = "close_brace"_name;
static_name_t open_parenthesis_k     = "open_parenthesis"_name;
static_name_t close_parenthesis_k    = "close_parenthesis"_name;
static_name_t dot_k                  = "dot"_name;
static_name_t open_bracket_k         = "open_bracket"_name;
static_name_t close_bracket_k        = "close_bracket"_name;
static_name_t at_k                   = ".at"_name;
static_name_t is_k                   = ".is"_name;
static_name_t to_k                   = ".to"_name;

static_name_t add_k                  = ".add"_name;
static_name_t subtract_k             = ".subtract"_name;
static_name_t multiply_k             = ".multiply"_name;
static_name_t divide_k               = ".divide"_name;
static_name_t modulus_k              = ".modulus"_name;

static_name_t not_k                  = ".not"_name;
static_name_t unary_negate_k         = ".unary_negate"_name;

static_name_t less_k                 = ".less"_name;
static_name_t greater_k              = ".greater"_name;

static_name_t and_k                  = ".and"_name;
static_name_t or_k                   = ".or"_name;
static_name_t less_equal_k           = ".less_equal"_name;
static_name_t greater_equal_k        = ".greater_equal"_name;
static_name_t not_equal_k            = ".not_equal"_name;
static_name_t equal_k                = ".equal"_name;

static_name_t keyword_k              = "keyword"_name;

static_name_t empty_k                = "empty"_name;
static_name_t true_k                 = "true"_name;
static_name_t false_k                = "false"_name;

static_name_t function_k             = ".function"_name;
static_name_t variable_k             = ".variable"_name;
static_name_t index_k                = ".index"_name;
static_name_t array_k                = ".array"_name;
static_name_t dictionary_k           = ".dictionary"_name;

static_name_t bitwise_and_k          = ".bitwise_and"_name;
static_name_t bitwise_xor_k          = ".bitwise_xor"_name;
static_name_t bitwise_or_k           = ".bitwise_or"_name;
static_name_t bitwise_rshift_k       = ".bitwise_rshift"_name;
static_name_t bitwise_lshift_k       = ".bitwise_lshift"_name;
static_name_t bitwise_negate_k       = ".bitwise_negate"_name;

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
