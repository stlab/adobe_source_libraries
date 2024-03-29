/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_XML_TOKEN_HPP
#define ADOBE_XML_TOKEN_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/implementation/lex_shared_fwd.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

enum xml_lex_token_set_t {
    xml_token_open_tag_k = 0,
    xml_token_open_slash_tag_k,
    xml_token_close_tag_k,
    xml_token_slash_close_tag_k,
    xml_token_name_k,
    xml_token_att_value_k,
    xml_token_char_data_k,
    xml_token_equals_k,
    xml_token_reference_k,
    xml_token_comment_k,
    xml_token_processing_instruction_k,
    xml_token_eof_k
};

/**************************************************************************************************/

const char* token_to_string(xml_lex_token_set_t token);

/**************************************************************************************************/

template <>
inline xml_lex_token_set_t eof_token<xml_lex_token_set_t>() {
    return xml_token_eof_k;
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
