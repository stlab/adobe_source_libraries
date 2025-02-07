/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

/*
REVISIT (sparent) : [Old comment - what does this mean?] I need to handle the pos_type correctly
with regards to state.
*/

/**************************************************************************************************/

#include <array>
#include <cassert>
#include <functional>
#include <iomanip>
#include <istream>
#include <optional>
#include <sstream>
#include <utility>

#include <boost/config.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/name.hpp>
#include <adobe/string.hpp>

#include <adobe/implementation/expression_parser.hpp>
#include <adobe/implementation/lex_stream.hpp>
#include <adobe/implementation/parser_shared.hpp>
#include <adobe/implementation/token.hpp>

#ifdef BOOST_MSVC
namespace std {
using ::isalnum;
using ::isalpha;
using ::isdigit;
using ::isspace;
} // namespace std
#endif

using namespace std;
using namespace adobe;

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

// Handles short-circuit productions of the form:
//  `<expression> { ("&&" | "||") <expression> }.`
template <class IsExpression, class IsOperator>
bool is_logical_expression(expression_parser& parser, array_t& expression_stack,
                           IsExpression is_expression, IsOperator is_operator,
                           const char* error_message) {
    if (!is_expression(expression_stack))
        return false;

    for (auto oper = is_operator(); oper; oper = is_operator()) {
        array_t operand2;

        if (!is_expression(operand2))
            parser.throw_exception(error_message);

        push_back(expression_stack, operand2);
        expression_stack.push_back(*oper);
    }

    return true;
}

/**************************************************************************************************/

// Handles non-short-circuit productions of the form:
//  `<expression> { <operator> <expression> }.`
template <class IsExpression, class IsOperator>
bool is_binary_expression(expression_parser& parser, array_t& expression_stack,
                          IsExpression is_expression, IsOperator is_operator,
                          const char* error_message) {
    if (!is_expression(expression_stack))
        return false;

    for (auto oper = is_operator(); oper; oper = is_operator()) {
        if (!is_expression(expression_stack))
            parser.throw_exception(error_message);

        expression_stack.push_back(*oper);
    }

    return true;
}


/**************************************************************************************************/

class name_span_t {
public:
    template <class T>
    name_span_t(const T& a) : first_m(a.data()), last_m(a.data() + a.size()) {}

    const static_name_t* begin() const { return first_m; }
    const static_name_t* end() const { return last_m; }

private:
    const static_name_t* first_m;
    const static_name_t* last_m;
};

optional<name_t> match_token(expression_parser& parser, name_span_t span) {
    for (const auto& token : span) {
        if (parser.is_token(token)) {
            return token;
        }
    }
    return nullopt;
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

class expression_parser::implementation {
public:
    implementation(std::istream& in, const line_position_t& position)
        : token_stream_m(in, position) {}

    void set_keyword_extension_lookup(const keyword_extension_lookup_proc_t& proc) {
        token_stream_m.set_keyword_extension_lookup(proc);
    }

    void set_comment_bypass(bool bypass) { token_stream_m.set_comment_bypass(bypass); }

    lex_stream_t token_stream_m;
};

/**************************************************************************************************/

expression_parser::expression_parser(std::istream& in, const line_position_t& position)
    : object(new implementation(in, position)) {}

expression_parser::~expression_parser() { delete object; }

/**************************************************************************************************/

/* REVISIT (sparent) : Should this be const? And is there a way to specify the class to throw? */

void expression_parser::throw_exception(const char* errorString) {
    throw_parser_exception(errorString, next_position());
}

/**************************************************************************************************/

/* REVISIT (sparent) : Should this be const? And is there a way to specify the class to throw? */

void expression_parser::throw_exception(const name_t& found, const name_t& expected) {
    throw_parser_exception(found, expected, next_position());
}

/**************************************************************************************************/

const line_position_t& expression_parser::next_position() {
    return object->token_stream_m.next_position();
}

/**************************************************************************************************/

void expression_parser::set_keyword_extension_lookup(const keyword_extension_lookup_proc_t& proc) {
    object->set_keyword_extension_lookup(proc);
}

/**************************************************************************************************/

void expression_parser::set_comment_bypass(bool bypass) { object->set_comment_bypass(bypass); }

/**************************************************************************************************/

//  expression = or_expression ["?" expression ":" expression].
bool expression_parser::is_expression(array_t& expression_stack) {
    if (!is_or_expression(expression_stack))
        return false;

    if (!is_token(question_k))
        return true;

    array_t operand2;
    array_t operand3;

    require_expression(operand2);
    require_token(colon_k);
    require_expression(operand3);

    push_back(expression_stack, operand2);
    push_back(expression_stack, operand3);
    expression_stack.push_back(any_regular_t(ifelse_k));

    return true;
}

/**************************************************************************************************/

void expression_parser::require_expression(array_t& expression_stack) {
    if (!is_expression(expression_stack)) {
        throw_exception("`expression` required.");
    }
}

/**************************************************************************************************/

//  or_expression = and_expression { "||" and_expression }.
bool expression_parser::is_or_expression(array_t& expression_stack) {
    return is_logical_expression(
        *this, expression_stack, [&](auto& stack) { return is_and_expression(stack); },
        [&] { return match_token(*this, array{or_k}); }, "`and_expression` required.");
}

/**************************************************************************************************/

//  and_expression = bitwise_or_expression { "&&" bitwise_or_expression }.
bool expression_parser::is_and_expression(array_t& expression_stack) {
    return is_logical_expression(
        *this, expression_stack, [&](auto& stack) { return is_bitwise_or_expression(stack); },
        [&] { return is_token(and_k) ? optional{and_k} : nullopt; },
        "`bitwise_or_expression` required.");
}

/**************************************************************************************************/

//  bitwise_or_expression = bitwise_xor_expression { "|" bitwise_xor_expression }.
bool expression_parser::is_bitwise_or_expression(array_t& expression_stack) {
    return is_binary_expression(
        *this, expression_stack, [&](auto& stack) { return is_bitwise_xor_expression(stack); },
        [&] { return is_token(bitwise_or_k) ? optional{bitwise_or_k} : nullopt; },
        "`bitwise_xor_expression` required.");
}

/**************************************************************************************************/

//  bitwise_xor_expression = bitwise_and_expression { "^" bitwise_and_expression }.
bool expression_parser::is_bitwise_xor_expression(array_t& expression_stack) {
    return is_binary_expression(
        *this, expression_stack, [&](auto& stack) { return is_bitwise_and_expression(stack); },
        [&] { return is_token(bitwise_xor_k) ? optional{bitwise_xor_k} : nullopt; },
        "`bitwise_and_expression` required.");
}

/**************************************************************************************************/

//  bitwise_and_expression = equality_expression { "&" equality_expression }.
bool expression_parser::is_bitwise_and_expression(array_t& expression_stack) {
    return is_binary_expression(
        *this, expression_stack, [&](auto& stack) { return is_equality_expression(stack); },
        [&] { return is_token(bitwise_and_k) ? optional{bitwise_and_k} : nullopt; },
        "`equality_expression` required.");
}

/**************************************************************************************************/

//  equality_expression = relational_expression { ("==" | "!=") relational_expression }.
bool expression_parser::is_equality_expression(array_t& expression_stack) {
    return is_binary_expression(
        *this, expression_stack, [&](auto& stack) { return is_relational_expression(stack); },
        [&] { return match_token(*this, array{equal_k, not_equal_k}); },
        "`relational_expression` required.");
}

/**************************************************************************************************/

//  relational_expression = bitshift_expression { ("<" | ">" | "<=" | ">=") bitshift_expression }.
bool expression_parser::is_relational_expression(array_t& expression_stack) {
    return is_binary_expression(
        *this, expression_stack, [&](auto& stack) { return is_bitshift_expression(stack); },
        [&] { return match_token(*this, array{less_k, greater_k, less_equal_k, greater_equal_k}); },
        "`bitshift_expression` required.");
}

/**************************************************************************************************/

//  bitshift_expression = additive_expression { ("<<" | ">>") additive_expression }.
bool expression_parser::is_bitshift_expression(array_t& expression_stack) {
    return is_binary_expression(
        *this, expression_stack, [&](auto& stack) { return is_additive_expression(stack); },
        [&] { return match_token(*this, array{bitwise_lshift_k, bitwise_rshift_k}); },
        "`additive_expression` required.");
}

/**************************************************************************************************/

//  additive_expression = multiplicative_expression { additive_operator multiplicative_expression }.
bool expression_parser::is_additive_expression(array_t& expression_stack) {
    return is_binary_expression(
        *this, expression_stack, [&](auto& stack) { return is_multiplicative_expression(stack); },
        [&] { return match_token(*this, array{add_k, subtract_k}); },
        "`multiplicative_expression` required.");
}

/**************************************************************************************************/

//  multiplicative_expression = unary_expression { ("*" | "/" | "%") unary_expression }.
bool expression_parser::is_multiplicative_expression(array_t& expression_stack) {
    return is_binary_expression(
        *this, expression_stack, [&](auto& stack) { return is_unary_expression(stack); },
        [&] { return match_token(*this, array{multiply_k, divide_k, modulus_k}); },
        "`unary_expression` required.");
}

/**************************************************************************************************/

//  unary_expression = postfix_expression | (unary_operator unary_expression).

bool expression_parser::is_unary_expression(array_t& expression_stack) {
    if (is_postfix_expression(expression_stack))
        return true;

    name_t operator_l;

    if (is_unary_operator(operator_l)) {
        if (!is_unary_expression(expression_stack))
            throw_exception("`unary_expression` required.");

        if (operator_l != add_k)
            expression_stack.push_back(any_regular_t(operator_l));

        return true;
    }

    return false;
}

/**************************************************************************************************/

//  postfix_expression          = primary_expression { ("[" expression "]") | ("." identifier)
//                                  | "(" [argument_expression_list] ")"}.

bool expression_parser::is_postfix_expression(array_t& expression_stack) {
    if (!is_primary_expression(expression_stack))
        return false;

    while (true) {
        if (is_token(open_bracket_k)) {
            require_expression(expression_stack);
            require_token(close_bracket_k);
            expression_stack.push_back(index_k);
        } else if (is_token(dot_k)) {
            any_regular_t result;
            require_token(identifier_k, result);
            expression_stack.push_back(std::move(result));
            expression_stack.push_back(index_k);
        } else if (is_token(open_parenthesis_k)) {
            // If there are no parameters then set the parameters to an empty array.
            if (!is_argument_expression_list(expression_stack))
                push_back(expression_stack, adobe::array_t());
            require_token(close_parenthesis_k);
            expression_stack.push_back(function_k);
        } else
            break;
    }

    return true;
}

/**************************************************************************************************/

//  argument_expression_list = named_argument_list | argument_list.
bool expression_parser::is_argument_expression_list(array_t& expression_stack) {
    if (is_named_argument_list(expression_stack) || is_argument_list(expression_stack))
        return true;

    return false;
}

/**************************************************************************************************/

//  argument_list = expression { "," expression }.
bool expression_parser::is_argument_list(array_t& expression_stack) {
    if (!is_expression(expression_stack))
        return false;

    std::size_t count = 1;

    while (is_token(comma_k)) {
        require_expression(expression_stack);
        ++count;
    }

    expression_stack.push_back(any_regular_t(double(count)));
    expression_stack.push_back(any_regular_t(array_k));

    return true;
}

/**************************************************************************************************/

//  named_argument_list = named_argument { "," named_argument }.
bool expression_parser::is_named_argument_list(array_t& expression_stack) {
    if (!is_named_argument(expression_stack))
        return false;

    std::size_t count = 1;

    while (is_token(comma_k)) {
        if (!is_named_argument(expression_stack))
            throw_exception("Named argument required.");

        ++count;
    }

    expression_stack.push_back(any_regular_t(double(count)));
    expression_stack.push_back(any_regular_t(dictionary_k));

    return true;
}

/**************************************************************************************************/

//  named_argument = identifier ":" expression.
bool expression_parser::is_named_argument(array_t& expression_stack) {
    /* NOTE (sparent) : This is the one point in the grammar where we need the LL(2) parser. */

    name_t ident;

    if (!is_identifier(ident))
        return false;

    if (!is_token(colon_k)) {
        putback(); // the identifier

        return false;
    }

    expression_stack.push_back(any_regular_t(ident));
    require_expression(expression_stack);

    return true;
}

/**************************************************************************************************/

// primary_expression          = name | number | boolean | string | "empty" | array | dictionary
//                                | identifier | ( "(" expression ")" ).

bool expression_parser::is_primary_expression(array_t& expression_stack) {
    any_regular_t result; // empty result used if is_keyword(empty_k)
    name_t name_result;

    if (is_name(result) || is_token(number_k, result) || is_boolean(result) ||
        is_token(string_k, result) || is_keyword(empty_k)) {
        expression_stack.push_back(std::move(result));
        return true;
    } else if (is_array(expression_stack))
        return true;
    else if (is_dictionary(expression_stack))
        return true;
    else if (is_identifier(name_result)) {
        expression_stack.push_back(name_result);
        expression_stack.push_back(any_regular_t(variable_k));
        return true;
    } else if (is_token(open_parenthesis_k)) {
        require_expression(expression_stack);
        require_token(close_parenthesis_k);
        return true;
    }

    return false;
}

/**************************************************************************************************/

bool expression_parser::is_dictionary(array_t& expression_stack) {
    if (!is_token(open_brace_k))
        return false;

    if (!is_named_argument_list(expression_stack))
        push_back(expression_stack, adobe::dictionary_t());

    require_token(close_brace_k);

    return true;
}

/**************************************************************************************************/

bool expression_parser::is_array(array_t& expression_stack) {
    if (!is_token(open_bracket_k))
        return false;

    if (!is_argument_list(expression_stack))
        push_back(expression_stack, adobe::array_t());

    require_token(close_bracket_k);

    return true;
}

/**************************************************************************************************/

bool expression_parser::is_name(any_regular_t& result) {
    if (!is_token(at_k))
        return false;

    if (!is_token(keyword_k, result) && !is_token(identifier_k, result))
        throw_exception("`identifier` or `keyword` required.");

    return true;
}

/**************************************************************************************************/

bool expression_parser::is_boolean(any_regular_t& result) {
    if (is_keyword(true_k)) {
        result = any_regular_t(true);
        return true;
    } else if (is_keyword(false_k)) {
        result = any_regular_t(false);
        return true;
    }

    return false;
}


/**************************************************************************************************/

//  unary_operator = "+" | "-" | "!" | "~".
bool expression_parser::is_unary_operator(name_t& name_result) {
    const stream_lex_token_t& result(get_token());

    name_t name = result.first;
    if (name == subtract_k) {
        name_result = unary_negate_k;
        return true;
    } else if (name == not_k || name == add_k || name == bitwise_negate_k) {
        name_result = name;
        return true;
    }
    putback();
    return false;
}

/**************************************************************************************************/

/*
    REVISIT (sparent) : There should be a turn the simple lexical calls into templates
*/

bool expression_parser::is_identifier(name_t& name_result) {
    const stream_lex_token_t& result(get_token());

    if (result.first == identifier_k) {
        name_result = result.second.cast<adobe::name_t>();
        return true;
    }

    putback();
    return false;
}

bool expression_parser::is_lead_comment(string& string_result) {
    const stream_lex_token_t& result(get_token());

    if (result.first == lead_comment_k) {
        string_result = result.second.cast<string>();
        return true;
    }

    putback();
    return false;
}

bool expression_parser::is_trail_comment(string& string_result) {
    const stream_lex_token_t& result(get_token());

    if (result.first == trail_comment_k) {
        string_result = result.second.cast<string>();
        return true;
    }

    putback();
    return false;
}

/**************************************************************************************************/

bool expression_parser::is_token(name_t tokenName, any_regular_t& tokenValue) {
    const stream_lex_token_t& result(get_token());
    if (result.first == tokenName) {
        tokenValue = std::move(result.second);
        return true;
    }
    putback();
    return false;
}

/**************************************************************************************************/

bool expression_parser::is_token(name_t tokenName) {
    const stream_lex_token_t& result(get_token());
    if (result.first == tokenName) {
        return true;
    }
    putback();
    return false;
}

/**************************************************************************************************/

bool expression_parser::is_keyword(name_t keyword_name) {
    const stream_lex_token_t& result(get_token());
    if (result.first == keyword_k && result.second.cast<name_t>() == keyword_name)
        return true;
    putback();
    return false;
}

/**************************************************************************************************/

const stream_lex_token_t& expression_parser::get_token() { return object->token_stream_m.get(); }

/**************************************************************************************************/

void expression_parser::putback() { object->token_stream_m.putback(); }

/**************************************************************************************************/

void expression_parser::require_token(name_t tokenName, any_regular_t& tokenValue) {
    const stream_lex_token_t& result(get_token());
    if (result.first != tokenName) {
        putback();
        throw_exception(tokenName, result.first);
    }

    tokenValue = result.second;
}

/**************************************************************************************************/

void expression_parser::require_keyword(name_t keyword_name) {
    const stream_lex_token_t& result(get_token());
    if (result.first != keyword_k) {
        putback();
        throw_parser_exception(keyword_name.c_str(), token_to_string(result.first),
                               next_position());
    }
    if (result.second.cast<name_t>() != keyword_name) {
        putback();
        throw_exception(keyword_name, result.second.cast<name_t>());
    }
}

/**************************************************************************************************/

void expression_parser::require_token(name_t tokenName) {
    const stream_lex_token_t& result(get_token());
    if (result.first == tokenName)
        return;

    putback();
    throw_exception(tokenName, result.first);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
