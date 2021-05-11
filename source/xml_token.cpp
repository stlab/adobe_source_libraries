/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/implementation/xml_token.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

const char* token_to_string(xml_lex_token_set_t token) {
    switch (token) {
    case xml_token_open_tag_k:
        return "xml_token_open_tag_k";
        break;
    case xml_token_open_slash_tag_k:
        return "xml_token_open_slash_tag_k";
        break;
    case xml_token_close_tag_k:
        return "xml_token_close_tag_k";
        break;
    case xml_token_slash_close_tag_k:
        return "xml_token_slash_close_tag_k";
        break;
    case xml_token_name_k:
        return "xml_token_name_k";
        break;
    case xml_token_att_value_k:
        return "xml_token_att_value_k";
        break;
    case xml_token_char_data_k:
        return "xml_token_char_data_k";
        break;
    case xml_token_equals_k:
        return "xml_token_equals_k";
        break;
    case xml_token_reference_k:
        return "xml_token_reference_k";
        break;
    case xml_token_comment_k:
        return "xml_token_comment_k";
        break;
    case xml_token_processing_instruction_k:
        return "xml_token_processing_instruction_k";
        break;
    case xml_token_eof_k:
        return "xml_token_eof_k";
        break;
    }

    return "unknown token!";
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
