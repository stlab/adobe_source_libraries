/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/implementation/xml_lex.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

xml_lex_t::xml_lex_t(uchar_ptr_t first, uchar_ptr_t last, const line_position_t& position) :
    _super(first, last, position), name_possible_m(false)
{
    _super::set_parse_token_proc(boost::bind(&xml_lex_t::parse_token, boost::ref(*this)));
}

/*************************************************************************************************/

xml_lex_t::xml_lex_t(const xml_lex_t& rhs) :
    _super(rhs), name_possible_m(rhs.name_possible_m)
{
    _super::set_parse_token_proc(boost::bind(&xml_lex_t::parse_token, boost::ref(*this)));
}

/*************************************************************************************************/

void xml_lex_t::parse_token()
{
    token_type result;

    if (!(  is_processing_instruction(result)
		||  is_comment(result)
		||  is_tag_open(result)
        ||  is_tag_close(result)
        ||  (name_possible_m &&
                (   is_name(result)
                ||  is_equals(result)
                ||  is_att_value(result
                )))
        ||  is_reference(result)
        ||  is_char_data(result)
        ))
    { _super::throw_parser_exception("Syntax Error"); }

    _super::put_token(result);
}

/*************************************************************************************************/

bool xml_lex_t::is_processing_instruction(token_type& result)
{
    char	c;
	
	init_token(result);
	
	// REVISIT eberdahl - Although this code "works" in the sense
	// that the processing instruction is extracted as an entire
	// token, it seems wrong to implement the lexer in this way. This
	// mechanism has the advantage of requiring a single token type
	// (the processing instruction) and makes the trivial parser's job
	// much easier. However, the mechanism is signifiantly different
	// from, say, lexing an element tag, in which the tag open and tag
	// close tokens are independent lexical tokens and the tokens
	// between the tag open and tag close tokens must be assembled by
	// the parser (i.e. instead of being assembled by the lexer).
	
    if (get_char(c) && c == '<' &&
		get_char(c) && c == '?')
    {
		while (get_char(c))
		{
			if (c == '?' &&
				peek_char(c) && c == '>')
			{
				_super::advance_lex();
				
				finalize_token(result, xml_token_processing_instruction_k);
				return true;
			}
		}
		
		_super::throw_parser_exception("Unterminated processing instruction");
	}
	
	reset_lex(result);
	
    return false;
}

/*************************************************************************************************/

bool xml_lex_t::is_comment(token_type& result)
{
    char	c;

	// REVISIT eberdahl - Although this code "works" in the sense
	// that the comment is extracted as an entire token, it seems
	// wrong to implement the lexer in this way. This mechanism has
	// the advantage of requiring a single token type (the comment)
	// and makes the trivial parser's job much easier. However, the
	// mechanism is signifiantly different from, say, lexing an
	// element tag, in which the tag open and tag close tokens are
	// independent lexical tokens and the tokens between the tag open
	// and tag close tokens must be assembled by the parser (i.e.
	// instead of being assembled by the lexer).
	
	init_token(result);

    if (get_char(c) && c == '<' &&
		get_char(c) && c == '!' &&
		get_char(c) && c == '-' &&
		get_char(c) && c == '-')
    {
		while (get_char(c))
		{
			if (c == '-' &&
				peek_char(c) && c == '-')
			{
				_super::advance_lex();

				if (get_char(c) && c == '>')
				{
					finalize_token(result, xml_token_comment_k);
					return true;
				}
				else
				{
					_super::throw_parser_exception("Illegal character sequence in comment");
				}
			}
		}

		_super::throw_parser_exception("Unterminated comment");
	}

	reset_lex(result);
	
    return false;
}

/*************************************************************************************************/

bool xml_lex_t::is_tag_open(token_type& result)
{
    char c;

    init_token(result);

    if (get_char(c) && c == '<')
    {
        if (peek_char(c) && c == '/')
        {
            _super::advance_lex();

            finalize_token(result, xml_token_open_slash_tag_k);
        }
        else
            finalize_token(result, xml_token_open_tag_k);

        name_possible_m = true;

        _super::set_skip_white_space(true);

        return true;
    }

    reset_lex(result);

    return false;
}

/*************************************************************************************************/

bool xml_lex_t::is_tag_close(token_type& result)
{
    char c;

    init_token(result);

    if (!get_char(c)) return false;

    if (c == '>')
    {
        finalize_token(result, xml_token_close_tag_k);
    }
    else if (c == '/')
    {
        if (peek_char(c) && c == '>')
        {
            _super::advance_lex();

            finalize_token(result, xml_token_slash_close_tag_k);
        }
        else
        {
            reset_lex(result);

            return false;
        }
    }
    else
    {
        reset_lex(result);

        return false;
    }

    name_possible_m = false;

    _super::set_skip_white_space(false);

    return true;
}

/*************************************************************************************************/

bool xml_lex_t::is_name(token_type& result)
{
    init_token(result);

    if (!is_name_start_char()) return false;

    while (true)
    {
        if (is_name_char())
            _super::advance_lex();
        else
            break;
    }

    finalize_token(result, xml_token_name_k);

    return true;
}

/*************************************************************************************************/

bool xml_lex_t::is_equals(token_type& result)
{
    char c;

    init_token(result);

    if (peek_char(c) && c == '=')
    {
        _super::advance_lex();

        finalize_token(result, xml_token_equals_k);

        return true;
    }

    return false;
}

/*************************************************************************************************/

bool xml_lex_t::is_att_value(token_type& result)
{
    char c;

    if (!(peek_char(c) && (c == '"' || c == '\''))) return false;

    _super::advance_lex();

    init_token(result);

    char tick(c);

    while (peek_char(c))
    {
        if (c == '<' || c == '&')
            _super::throw_parser_exception("Unterminated Attribute Value");
        else if (c == tick)
            break;
        else
            _super::advance_lex();
    }

    finalize_token(result, xml_token_att_value_k);

    _super::advance_lex();

    return true;
}

/*************************************************************************************************/

bool xml_lex_t::is_reference(token_type& result)
{
    char c;

    if (!(peek_char(c) && c == '&')) return false;

    init_token(result);

    _super::advance_lex();

    while (get_char(c))
    {
        if (c == ';')
            break;
    }

    finalize_token(result, xml_token_reference_k);

    return true;
}

/*************************************************************************************************/

bool xml_lex_t::is_char_data(token_type& result)
{
    char c;

    init_token(result);

    while (peek_char(c))
    {
        if (c == '<' || c == '&')
            break;

        _super::advance_lex();
    }

    finalize_token(result, xml_token_char_data_k);

    return true;
}

/*************************************************************************************************/
//
// NameStartChar =  ":" | [A-Z] | "_" | [a-z] | [#xC0-#xD6] | [#xD8-#xF6] |
//                  [#xF8-#x2FF] | [#x370-#x37D] | [#x37F-#x1FFF] |
//                  [#x200C-#x200D] | [#x2070-#x218F] | [#x2C00-#x2FEF] |
//                  [#x3001-#xD7FF] | [#xF900-#xFDCF] | [#xFDF0-#xFFFD] | [#x10000-#xEFFFF]
//
// (latter tests removed by virtue of type size limitation)
//
bool xml_lex_t::is_name_start_char()
{
    char c;

    if (!peek_char(c)) return false;

    return  (c >= 'A' && c <= 'Z')                ||
            (c >= 'a' && c <= 'z')                ||
            c == ':'                            ||
            c == '_'                            ||
            (c >= char(0xC0) && c <= char(0xD6))  ||
            (c >= char(0xD8) && c <= char(0xF6))  ||
            (c >= char(0xF8) && c <= char(0xFF));
}

/*************************************************************************************************/
//
// NameChar = NameStartChar | "-" | "." | [0-9] | #xB7 | [#x0300-#x036F] | [#x203F-#x2040]
//
// (latter tests removed by virtue of type size limitation)
//
bool xml_lex_t::is_name_char()
{
    if (is_name_start_char()) return true;

    char c;

    if (!peek_char(c)) return false;

    return  c == '-'                ||
            c == '.'                ||
            (c >= '0' && c <= '9')    ||
            c == char(0xB7);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
