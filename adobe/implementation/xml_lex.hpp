/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_XML_LEX_HPP
#define ADOBE_XML_LEX_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/implementation/xml_lex_fwd.hpp>
#include <adobe/implementation/xml_token.hpp>
#include <adobe/implementation/lex_shared.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

class xml_lex_t : lex_base_t<2, xml_lex_token_set_t>
{
    typedef lex_base_t<2, xml_lex_token_set_t>  _super;

public:
    typedef _super::token_type                  token_type;

    xml_lex_t(uchar_ptr_t first, uchar_ptr_t last, const line_position_t& position);

    xml_lex_t(const xml_lex_t& rhs);

    const token_type&       get()
        { return _super::get_token(); } 

    void                    putback()
        { _super::putback_token(); } 

    const line_position_t&  next_position()
        { return _super::next_position(); } 

    void                    set_skip_white_space(bool skip)
		{ return _super::set_skip_white_space(skip); }
	
#if !defined(ADOBE_NO_DOCUMENTATION)
private:
    typedef std::istream::pos_type pos_type;

    void parse_token();

    bool is_processing_instruction(token_type& result);
    bool is_comment(token_type& result);
    bool is_tag_open(token_type& result);
    bool is_tag_close(token_type& result);
    bool is_name(token_type& result);
    bool is_equals(token_type& result);
    bool is_att_value(token_type& result);
    bool is_char_data(token_type& result);
    bool is_reference(token_type& result);

    bool is_name_start_char();
    bool is_name_char();

    bool name_possible_m;   // names only possible within tags
#endif // !defined(ADOBE_NO_DOCUMENTATION)
};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
