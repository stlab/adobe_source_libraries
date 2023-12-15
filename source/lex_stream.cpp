/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/implementation/lex_stream.hpp>

#include <array>
#include <iostream>
#include <mutex>
#include <sstream>

#include <boost/bind/bind.hpp>

#include <adobe/circular_queue.hpp>
#include <adobe/implementation/lex_shared.hpp>
#include <adobe/implementation/token.hpp>
#include <adobe/istream.hpp>
#include <adobe/name.hpp>
#include <adobe/once.hpp>
#include <adobe/string.hpp>

/**************************************************************************************************/

using namespace std;
using namespace boost::placeholders;

/**************************************************************************************************/

#ifdef BOOST_MSVC
namespace std {
using ::isalnum;
using ::isalpha;
using ::isdigit;
using ::isspace;
} // namespace std
#endif

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

using keyword_table_t = std::array<adobe::name_t, 3>;

/**************************************************************************************************/

const keyword_table_t* keywords_g;
const char* compound_match_g;
const adobe::name_t* name_table_g;
const int* compound_index_g;
const int* simple_index_g;

/**************************************************************************************************/

void init_once() {
    static keyword_table_t keywords_s = {{adobe::empty_k, adobe::true_k, adobe::false_k}};

    /*
        Multi-character tokens. Col + Row = ASCII value of first character in the token. Value at
        that position in the table is the next character required in order to make a valid token.

        '#' is a special character that denotes the next value could be one of many possibilities,
        and are custom-handled within is_compound.
    */
    static const char compound_match_s[] = {
        /*           0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
         */
        /* 00 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '0', '0', '0', '0',
        /* 10 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '0', '0', '0', '0',
        /* 20 */ '0', '=', '0', '0', '0', '0', '&', '0',
        '0',          '0', '0', '0', '0', '>', '0', '0',
        /* 30 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '#', '=', '#', '0',
        /* 40 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '0', '0', '0', '0',
        /* 50 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '0', '0', '0', '0',
        /* 60 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '0', '0', '0', '0',
        /* 70 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '|', '0', '0', '0',
        /* 80 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '0', '0', '0', '0',
        /* 90 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '0', '0', '0', '0',
        /* A0 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '0', '0', '0', '0',
        /* B0 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '0', '0', '0', '0',
        /* C0 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '0', '0', '0', '0',
        /* D0 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '0', '0', '0', '0',
        /* E0 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '0', '0', '0', '0',
        /* F0 */ '0', '0', '0', '0', '0', '0', '0', '0',
        '0',          '0', '0', '0', '0', '0', '0', '0'};

    static const adobe::name_t name_table_s[] = {/* 00 */ adobe::empty_k,
                                                 /* 01 */ adobe::equal_k,
                                                 /* 02 */ adobe::and_k,
                                                 /* 03 */ adobe::or_k,
                                                 /* 04 */ adobe::less_equal_k,
                                                 /* 05 */ adobe::greater_equal_k,
                                                 /* 06 */ adobe::not_equal_k,
                                                 /* 07 */ adobe::add_k,
                                                 /* 08 */ adobe::subtract_k,
                                                 /* 09 */ adobe::multiply_k,
                                                 /* 0A */ adobe::divide_k,
                                                 /* 0B */ adobe::modulus_k,
                                                 /* 0C */ adobe::question_k,
                                                 /* 0D */ adobe::colon_k,
                                                 /* 0E */ adobe::semicolon_k,
                                                 /* 0F */ adobe::assign_k,
                                                 /* 10 */ adobe::not_k,
                                                 /* 11 */ adobe::open_brace_k,
                                                 /* 12 */ adobe::close_brace_k,
                                                 /* 13 */ adobe::less_k,
                                                 /* 14 */ adobe::greater_k,
                                                 /* 15 */ adobe::open_parenthesis_k,
                                                 /* 16 */ adobe::close_parenthesis_k,
                                                 /* 17 */ adobe::at_k,
                                                 /* 18 */ adobe::open_bracket_k,
                                                 /* 19 */ adobe::close_bracket_k,
                                                 /* 1A */ adobe::comma_k,
                                                 /* 1B */ adobe::dot_k,
                                                 /* 1C */ adobe::to_k,
                                                 /* 1D */ adobe::bitwise_and_k,
                                                 /* 1E */ adobe::bitwise_xor_k,
                                                 /* 1F */ adobe::bitwise_or_k,
                                                 /* 20 */ adobe::bitwise_rshift_k,
                                                 /* 21 */ adobe::bitwise_lshift_k,
                                                 /* 22 */ adobe::bitwise_negate_k};

    /*
        Multi-character tokens. Col + Row = ASCII value of first character in the token. Value at
        that position in the table is the hex value for the token in the name table that compound
        token represents.
    */
    static const int compound_index_s[] = {
        /*           0     1     2     3     4     5     6     7     8     9     A     B     C
           D     E     F    */
        /* 00 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* 10 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* 20 */ 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00,
        /* 30 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
        /* 40 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* 50 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* 60 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* 70 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
        /* 80 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* 90 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* A0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* B0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* C0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* D0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* E0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* F0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /*
        Single-character tokens. Col + Row = ASCII value. Value at that position in the table is the
        hex value for the token in the name table that ASCII character represents. Example:

            "~" -> ASCII 0x7E -> 0x22 -> adobe::bitwise_negate_k

        Given that this routine is executed once would it be more legible to rewrite the below a la:

            simple_index_s['~'] = adobe::bitwise_negate_k ?
    */
    static const int simple_index_s[] = {
        /*           0     1     2     3     4     5     6     7     8     9     A     B     C
           D     E     F    */
        /* 00 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* 10 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* 20 */ 0x00, 0x10, 0x00, 0x00, 0x00, 0x0B, 0x1D, 0x00,
        0x15,          0x16, 0x09, 0x07, 0x1A, 0x08, 0x1B, 0x0A,
        /* 30 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x0D, 0x0E, 0x13, 0x0F, 0x14, 0x0C,
        /* 40 */ 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* 50 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x18, 0x00, 0x19, 0x1E, 0x00,
        /* 60 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* 70 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x11, 0x1F, 0x12, 0x22, 0x00,
        /* 80 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* 90 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* A0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* B0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* C0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* D0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* E0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* F0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00,          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    adobe::sort(keywords_s);

    keywords_g = &keywords_s;
    compound_match_g = &compound_match_s[0];
    name_table_g = &name_table_s[0];
    compound_index_g = &compound_index_s[0];
    simple_index_g = &simple_index_s[0];
}

/**************************************************************************************************/

static once_flag once_flag;
void once_instance() { call_once(once_flag, &init_once); }

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/


/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

struct lex_stream_t::implementation_t : stream_lex_base_t<2, std::istream_iterator<char>> {
    typedef stream_lex_base_t<2, std::istream_iterator<char>> _super;

public:
    typedef std::istream::pos_type pos_type;

    implementation_t(std::istream& in, const line_position_t& position);

    void set_keyword_extension_lookup(const keyword_extension_lookup_proc_t& proc);

    void set_comment_bypass(bool bypass);

private:
    void parse_token(char c);

    bool is_comment(char c, stream_lex_token_t& result);
    bool is_string(char c, stream_lex_token_t& result);
    bool is_number(char c, stream_lex_token_t& result);
    bool is_compound(char c, stream_lex_token_t& result);
    bool is_simple(char c, stream_lex_token_t& result);
    bool is_identifier_or_keyword(char c, stream_lex_token_t& result);

    bool is_hex_number(char c, stream_lex_token_t& result);
    bool is_dec_number(char c, stream_lex_token_t& result);

    bool skip_space(char& c);

    keyword_extension_lookup_proc_t keyword_proc_m;
    bool comment_bypass_m;
};

/**************************************************************************************************/

lex_stream_t::lex_stream_t(std::istream& in, const line_position_t& position)
    : object_m(new lex_stream_t::implementation_t(in, position)) {
    once_instance();
}

#if !defined(ADOBE_NO_DOCUMENTATION)

lex_stream_t::lex_stream_t(const lex_stream_t& rhs)
    : object_m(new lex_stream_t::implementation_t(*rhs.object_m)) {
    once_instance();
}

lex_stream_t::~lex_stream_t() { delete object_m; }

lex_stream_t& lex_stream_t::operator=(const lex_stream_t& rhs) {
    *object_m = *rhs.object_m;
    return *this;
}

#endif // !defined(ADOBE_NO_DOCUMENTATION)

const stream_lex_token_t& lex_stream_t::get() { return object_m->get_token(); }

void lex_stream_t::putback() { object_m->putback_token(); }

const line_position_t& lex_stream_t::next_position() { return object_m->next_position(); }

void lex_stream_t::set_keyword_extension_lookup(const keyword_extension_lookup_proc_t& proc) {
    return object_m->set_keyword_extension_lookup(proc);
}

void lex_stream_t::set_comment_bypass(bool bypass) { return object_m->set_comment_bypass(bypass); }

/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/

lex_stream_t::implementation_t::implementation_t(std::istream& in, const line_position_t& position)
    : _super(std::istream_iterator<char>(in), std::istream_iterator<char>(), position),
      comment_bypass_m(false) {
    in.unsetf(std::ios_base::skipws);

    _super::set_parse_token_proc(
        boost::bind(&lex_stream_t::implementation_t::parse_token, boost::ref(*this), _1));
}

/**************************************************************************************************/

void lex_stream_t::implementation_t::set_keyword_extension_lookup(
    const keyword_extension_lookup_proc_t& proc) {
    keyword_proc_m = proc;
}

/**************************************************************************************************/

void lex_stream_t::implementation_t::set_comment_bypass(bool bypass) { comment_bypass_m = bypass; }

/**************************************************************************************************/

bool lex_stream_t::implementation_t::is_number(char c, stream_lex_token_t& result) {
    return is_hex_number(c, result) || is_dec_number(c, result);
}

/**************************************************************************************************/

bool lex_stream_t::implementation_t::is_hex_number(char c, stream_lex_token_t& result) {
    if (c != '0' || std::tolower(_super::peek_char()) != 'x')
        return false;

    _super::get_char(c); // get 'x' or 'X'

    if (!std::isxdigit(_super::peek_char())) {
        _super::putback_char(c); // put back 'x' or 'X'

        return false;
    }

    unsigned long long v(0);

    while (true) {
        if (!_super::get_char(c))
            break;

        if (!std::isxdigit(c)) {
            _super::putback_char(c);

            break;
        }

        v <<= 4;

        v += c >= '0' && c <= '9' ? static_cast<int>(c - '0')
                                  : static_cast<int>(10 + std::tolower(c) - 'a');
    }

    result = stream_lex_token_t(number_k, any_regular_t(static_cast<double>(v)));

    return true;
}

/**************************************************************************************************/

bool lex_stream_t::implementation_t::is_dec_number(char c, stream_lex_token_t& result) {
    if (!std::isdigit(c))
        return false;

    _super::putback_char(c);

    std::stringstream temp;
    temp.imbue(std::locale::classic());

    while (true) {
        if (!_super::get_char(c))
            break;

        if (!std::isdigit(c) && c != '.') {
            _super::putback_char(c);

            break;
        }

        temp << c;
    }

    double re(0);
    temp >> re;

    result = stream_lex_token_t(number_k, any_regular_t(re));
    return true;
}

/**************************************************************************************************/

bool lex_stream_t::implementation_t::is_identifier_or_keyword(char c, stream_lex_token_t& result) {
    if (!std::isalpha(c) && c != '_')
        return false;

    identifier_buffer_m.clear();

    do {
        if (std::isalnum(c) || c == '_') {
            identifier_buffer_m.push_back(c);
        } else {
            _super::putback_char(c);
            break;
        }
    } while (_super::get_char(c));

    identifier_buffer_m.push_back(0);

    name_t ident(&identifier_buffer_m.front());
    keyword_table_t::const_iterator iter(lower_bound(*keywords_g, ident));

    if ((iter != keywords_g->end() && *iter == ident) ||
        (keyword_proc_m && keyword_proc_m(ident))) {
        result = stream_lex_token_t(keyword_k, any_regular_t(ident));
    } else {
        result = stream_lex_token_t(identifier_k, any_regular_t(ident));
    }

    return true;
}

/**************************************************************************************************/

bool lex_stream_t::implementation_t::is_comment(char c, stream_lex_token_t& result) {
    if (c != '/')
        return false;

    std::istream::int_type peek_c(_super::peek_char());

    if (peek_c == EOF || (peek_c != '/' && peek_c != '*'))
        return false;

    (void)_super::get_char(c);

    identifier_buffer_m.clear();

    if (c == '/') {
        while (_super::get_char(c) && !is_line_end(c)) {
            identifier_buffer_m.push_back(c);
        }

        identifier_buffer_m.push_back(0);

        result = stream_lex_token_t(trail_comment_k,
                                    any_regular_t(std::string(&identifier_buffer_m[0])));
    } else // if (c == '*')
    {
        while (true) {
            if (!_super::get_char(c))
                throw_parser_exception("Unexpected EOF in comment.");

            if (c == '*') {
                peek_c = _super::peek_char();

                if (peek_c != EOF && peek_c == '/') {
                    _super::ignore_char();
                    break;
                }
            } else if (is_line_end(c)) {
                c = '\n';
            }

            identifier_buffer_m.push_back(c);
        }

        identifier_buffer_m.push_back(0);

        result =
            stream_lex_token_t(lead_comment_k, any_regular_t(std::string(&identifier_buffer_m[0])));
    }

    return true;
}

/**************************************************************************************************/

bool lex_stream_t::implementation_t::is_string(char c, stream_lex_token_t& result) {
    if (c != '\'' && c != '\"')
        return false;

    identifier_buffer_m.clear();

    while (true) {
        char end_char(c);

        while (_super::get_char(c) && c != end_char) {
            // REVISIT (sparent) : Handle quoted characters here.
            // Also handle invalid characters such as line endings.
            identifier_buffer_m.push_back(c);
        }

        if (c != end_char)
            throw_parser_exception("Unexpected EOF in string.");

        if (!skip_space(c))
            break;

        if (c != '\'' && c != '\"') {
            _super::putback_char(c);
            break;
        }
    }

    identifier_buffer_m.push_back(0);
    result = stream_lex_token_t(string_k, any_regular_t(std::string(&identifier_buffer_m[0])));

    return true;
}

/**************************************************************************************************/

/*
    is_compound() works by noting that in most of the current compound tokens the token is unique
    in the first character and only requires that the proper second character be present.
*/

bool lex_stream_t::implementation_t::is_compound(char c, stream_lex_token_t& result) {
    char next_char(compound_match_g[(unsigned char)c]);

    if (next_char == '0')
        return false;

    int actual_char(_super::peek_char());

    if (actual_char == EOF)
        return false;

    /*
    There are cases compound characters that conflict in the simple table lookup:
        - >= and >>
        - <= and << and <==
    We special case them here rather than adding an additional table.
    */
    if (next_char == '#') {
        name_t token;

        if (c == '>' && actual_char == '>')
            token = bitwise_rshift_k;
        if (c == '>' && actual_char == '=')
            token = greater_equal_k;
        if (c == '<' && actual_char == '<')
            token = bitwise_lshift_k;
        if (c == '<' && actual_char == '=') {
            _super::ignore_char();
            if (_super::peek_char() == '=')
                token = is_k;
            else {
                result = stream_lex_token_t(less_equal_k, any_regular_t());
                return true;
            }
        }

        if (token) {
            _super::ignore_char();
            result = stream_lex_token_t(token, any_regular_t());
            return true;
        }

        return false;
    }


    if (actual_char != next_char)
        return false;

    _super::ignore_char();
    result = stream_lex_token_t(name_table_g[compound_index_g[(unsigned char)c]], any_regular_t());
    return true;
}

/**************************************************************************************************/

bool lex_stream_t::implementation_t::is_simple(char c, stream_lex_token_t& result) {
    int index(simple_index_g[(unsigned char)c]);

    if (index == 0)
        return false;

    result = stream_lex_token_t(name_table_g[index], any_regular_t());
    return true;
}

/**************************************************************************************************/

bool lex_stream_t::implementation_t::skip_space(char& c) {
    skip_white_space();

    return get_char(c);
}

/**************************************************************************************************/

void lex_stream_t::implementation_t::parse_token(char c) {
    stream_lex_token_t result;

    bool found_comment = is_comment(c, result);

    if (!(is_number(c, result) || is_identifier_or_keyword(c, result) || found_comment ||
          is_string(c, result) || is_compound(c, result) || is_simple(c, result))) {
        throw_parser_exception("Syntax Error");
    }

    if (!found_comment || !comment_bypass_m)
        put_token(std::move(result));
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
