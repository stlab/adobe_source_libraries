/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_EXPRESSION_PARSER_HPP
#define ADOBE_EXPRESSION_PARSER_HPP

#include <adobe/config.hpp>

#include <adobe/array_fwd.hpp>
#include <adobe/istream.hpp>
#include <adobe/dictionary_fwd.hpp>
#include <adobe/implementation/lex_stream_fwd.hpp>

#include <boost/noncopyable.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

/*! \page expression_language Expression Language

Underlying the language for Adam and Eve is a common, simple, expression language. The language
supports 

\code

expression                  = or_expression ["?" expression ":" expression].

or_expression               = and_expression { "||" and_expression }.
and_expression              = bitwise_or_expression { "&&" bitwise_or_expression }.
bitwise_or_expression       = bitwise_xor_expression { "|" bitwise_xor_expression }.
bitwise_xor_expression      = bitwise_and_expression { "^" bitwise_and_expression }.
bitwise_and_expression      = equality_expression { "&" equality_expression }.
equality_expression         = relational_expression { ("==" | "!=") relational_expression }.
relational_expression       = bitshift_expression { ("<" | ">" | "<=" | ">=") bitshift_expression }.
bitshift_expression         = additive_expression { ("<<" | ">>") additive_expression }.
additive_expression         = multiplicative_expression { ("+" | "-") multiplicative_expression }.
multiplicative_expression   = unary_expression { ("*" | "/" | "%") unary_expression }.

unary_expression            = postfix_expression | (unary_operator unary_expression).
unary_operator              = "+" | "-" | "!" | "~".

postfix_expression          = primary_expression { ("[" expression "]") | ("." identifier) }.

primary_expression          = name | number | boolean | string | "empty" | array | dictionary
                                | variable_or_fuction | ( "(" expression ")" ).

variable_or_function        = identifier ["(" [argument_expression_list] ")"].

array                       = "[" [argument_list] "]".
dictionary                  = "{" named_argument_list "}".

argument_expression_list    = named_argument_list | argument_list.

argument_list               = expression { "," expression }.
named_argument_list         = named_argument { "," named_argument }.
named_argument              = identifier ":" expression.

name                        = "@" (identifier | keyword).
boolean                     = "true" | "false".

\endcode

*/

/*************************************************************************************************/

class expression_parser : public boost::noncopyable
{
 public:
        
    expression_parser(std::istream& in, const line_position_t& position);
        
    ~expression_parser();
    
    const line_position_t& next_position();
    
    void set_keyword_extension_lookup(const keyword_extension_lookup_proc_t& proc);

//  expression = or_expression ["?" expression ":" expression].
    bool is_expression(array_t&);
    void require_expression(array_t&);
    
//  or_expression = and_expression { "||" and_expression }.
    bool is_or_expression(array_t&);

//  and_expression = bitwise_or_expression { "&&" bitwise_or_expression }.
    bool is_and_expression(array_t&);
    
//  bitwise_or_expression = bitwise_xor_expression { "|" bitwise_xor_expression }.
    bool is_bitwise_or_expression(array_t&);
    
//  bitwise_xor_expression = bitwise_and_expression { "^" bitwise_and_expression }.
    bool is_bitwise_xor_expression(array_t&);
    
//  bitwise_and_expression = equality_expression { "&" equality_expression }.
    bool is_bitwise_and_expression(array_t&);
    
//  equality_expression = relational_expression { ("==" | "!=") relational_expression }.
    bool is_equality_expression(array_t&);

//  relational_expression = bitshift_expression { relational_operator bitshift_expression }.
    bool is_relational_expression(array_t&);
    
//  bitshift_expression = additive_expression { ("<<" | ">>") additive_expression }.
    bool is_bitshift_expression(array_t&);
    
//  additive_expression = multiplicative_expression { ("+" | "-") multiplicative_expression }.
    bool is_additive_expression(array_t&);
    
    bool is_additive_operator(name_t&);
    
//  multiplicative_expression = unary_expression { ("*" | "/" | "%" | "div") unary_expression }.
    bool is_multiplicative_expression(array_t&);
    
    bool is_multiplicative_operator(name_t&);

//  unary_expression = postfix_expression | (unary_operator unary_expression).
    bool is_unary_expression(array_t&);
    
//  unary_operator = "+" | "-" | "!" | "~".
    bool is_unary_operator(name_t&);
    
//  postfix_expression = primary_expression { ("[" expression "]") | ("." identifier) }.
    bool is_postfix_expression(array_t&);
    
//  primary_expression = name | number | boolean | string | "empty" | array | dictionary
//      | variable_or_fuction | ( "(" expression ")" ).
    bool is_primary_expression(array_t&);
    
//  variable_or_fuctiontion = identifier ["(" [argument_expression_list] ")"].
    bool is_variable_or_function(array_t&);
    
//  argument_expression_list = named_argument_list | argument_list.
    bool is_argument_expression_list(array_t&);
    
//  array = "[" [argument_list] "]".
    bool is_array(array_t&);
    
//  dictionary = "{" named_argument_list "}".
    bool is_dictionary(array_t&);
    
//  argument_list = expression { "," expression }.
    bool is_argument_list(array_t&);
    
//  named_argument_list = named_argument { "," named_argument }.
    bool is_named_argument_list(array_t&);
    
//  named_argument = ident ":" expression.
    bool is_named_argument(array_t&);
        
//  name = "@" (identifier | keyword).
    bool is_name(any_regular_t&);
    
//  boolean = "true" | "false".
    bool is_boolean(any_regular_t&);
        
//  relational_operator = "<" | ">" | "<=" | ">=".
    bool is_relational_operator(name_t&);
    
//  bitshift_operator = "<<" | ">>".
    bool is_bitshift_operator(name_t&);
    
//  lexical tokens:

    bool is_identifier(name_t&);
    bool is_lead_comment(string_t&);
    bool is_trail_comment(string_t&);
    
/*
    REVISIT (sparent) : We should provide a protected call to get the token stream and allow
    subclasses to access it directly - but for now we'll stick with the law of Demiter.
*/

 protected:
    const stream_lex_token_t& get_token();
    void putback();

    bool is_token (name_t tokenName, any_regular_t& tokenValue);
    bool is_token (name_t tokenName);
    void require_token (name_t tokenName, any_regular_t& tokenValue);
    void require_token (name_t tokenName);
    bool is_keyword (name_t keywordName);
    void require_keyword (name_t keywordName);

    void throw_exception (const char* errorString);
    void throw_exception (const name_t& found, const name_t& expected);

private:
    class implementation;
    implementation*     object;
};

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
