/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/implementation/token.hpp>

#include <adobe/algorithm/binary_search.hpp>
#include <adobe/algorithm/sorted.hpp>
#include <adobe/cassert.hpp>
#include <adobe/name.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

auto token_to_string(name_t token) -> const char* {
    using table_t = std::pair<static_name_t, const char* const>;
    constexpr table_t token_table[] = {// Operators
                                       {add_k, "+"},
                                       {and_k, "&&"},
                                       {array_k, "array"},
                                       {at_k, "@"},
                                       {bitwise_and_k, "&"},
                                       {bitwise_lshift_k, "<<"},
                                       {bitwise_negate_k, "~"},
                                       {bitwise_or_k, "|"},
                                       {bitwise_rshift_k, ">>"},
                                       {bitwise_xor_k, "^"},
                                       {dictionary_k, "dictionary"},
                                       {divide_k, "/"},
                                       {equal_k, "=="},
                                       {function_k, "function"},
                                       {greater_k, ">"},
                                       {greater_equal_k, ">="},
                                       {ifelse_k, "?:"},
                                       {index_k, "[]"},
                                       {is_k, "<=="},
                                       {less_k, "<"},
                                       {less_equal_k, "<="},
                                       {modulus_k, "%"},
                                       {multiply_k, "*"},
                                       {not_k, "!"},
                                       {not_equal_k, "!="},
                                       {or_k, "||"},
                                       {subtract_k, "-"},
                                       {to_k, "->"},
                                       {unary_negate_k, "-"},
                                       {variable_k, "variable"},
                                       // Tokens
                                       {assign_k, "="},
                                       {close_brace_k, "}"},
                                       {close_bracket_k, "]"},
                                       {close_parenthesis_k, ")"},
                                       {colon_k, ":"},
                                       {comma_k, ","},
                                       {dot_k, "."},
                                       {empty_k, "empty"},
                                       {false_k, "false"},
                                       {identifier_k, "identifier"},
                                       {keyword_k, "keyword"},
                                       {lead_comment_k, "lead_comment"},
                                       {number_k, "number"},
                                       {open_brace_k, "{"},
                                       {open_bracket_k, "["},
                                       {open_parenthesis_k, "("},
                                       {question_k, "?"},
                                       {semicolon_k, ";"},
                                       {string_k, "string"},
                                       {trail_comment_k, "trail_comment"},
                                       {true_k, "true"}};

#ifndef NDEBUG
    static int x = [&] {
        ADOBE_ASSERT(is_sorted(token_table, less{}, &table_t::first) &&
                     "token_table must be sorted");
        return 0;
    }();
#endif

    auto p = binary_search(token_table, token, less{}, &table_t::first);
    ADOBE_ASSERT(p != std::end(token_table));
    return p->second;
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
