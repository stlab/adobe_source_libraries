/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_JSON_HPP
#define ADOBE_JSON_HPP

#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef ADOBE_BUILT_WITH_CMAKE
#include <double-conversion/double-conversion.h>
#else
#include <double-conversion/src/double-conversion.h>
#endif

#include <adobe/cassert.hpp>
#include <adobe/string/to_string.hpp>

/**************************************************************************************************/
/**
    \defgroup json JSON Utilities
*/
namespace adobe {

/**************************************************************************************************/
/**
    \ingroup json

    \brief A utility class that parses raw JSON data and uses a helper class to
           construct the desired representations of what is parsed.

    The helper class, `T` has some requirements:

     - typedef value_type
       - A type able to hold a descriminated union of string_type, double,
         object_type, array_type, void (default constructed), and bool.
     - typedef object_type
     - typedef array_type
     - typedef string_type
     - typedef key_type
     - `T::move_append(object_type, key_type&, value_type&);`
     - `T::move_append(array_type, value_type&);`
     - `T::append(string_type, const char* f, const char* l);`
*/
template <typename T>
class json_parser {
    constexpr static double kNaN = std::numeric_limits<double>::quiet_NaN();
  public:
    typedef typename T::object_type object_type;
    typedef typename T::array_type array_type;
    typedef typename T::value_type value_type;
    typedef typename T::string_type string_type;
    typedef typename T::key_type key_type;
    
    /*
    We want both empty and error values to be infinity as it will be an invalid
    JSON value and is something we can check against to assert validity. We also
    allow 'junk' as it is the end of this token and on to the next one.
    */
    explicit json_parser(const char* p) :
        p_(p),
        s2d_(double_conversion::StringToDoubleConverter::ALLOW_TRAILING_JUNK,
             kNaN, kNaN, nullptr, nullptr)
        { }

    value_type parse() {
        value_type result;
        require(is_object(result) || is_array(result), "object or array");
        return result;
    }

  private:    
    bool is_object(value_type& t) {
        if (!is_structural_char('{')) return false;
        object_type object;
        key_type string;
        if (is_string(string)) {
            require(is_structural_char(':'), ":");
            value_type value;
            require(is_value(value), "value");
            T::move_append(object, string, value);

            while (is_structural_char(',')) {
                require(is_string(string), "string");
                require(is_structural_char(':'), ":");
                require(is_value(value), "value");
                T::move_append(object, string, value);
            }
        }
        require(is_structural_char('}'), "}");
        t = value_type(std::move(object));
        return true;
    }
    
    bool is_array(value_type& t) {
        if (!is_structural_char('[')) return false;
        array_type array;
        value_type value;
        if (is_value(value)) {
            T::move_append(array, value);
            while (is_structural_char(',')) {
                require(is_value(value), "value");
                T::move_append(array, value);
            }
        }
        require(is_structural_char(']'), "]");
        t = value_type(std::move(array));
        return true;
    }
    
    // REVISIT (sparent) : should use peek to avoid steping in/out of multiple functions.
    
    bool is_value(value_type& t) {
        return is_string(t)
            || is_number(t)
            || is_object(t)
            || is_array(t)
            || is_bool(t)
            || is_null(t);
    }
    
    // requires at least one character
    
    bool is_sequence(const char* p) {
        if (!is_char(*p)) return false;
        ++p;
        while (is_char(*p)) ++p;
        require(*p == 0, "valid constant");
        return true;
    }
    
    bool is_bool(value_type& t) {
        if (is_sequence("true")) { t = value_type(true); return true; }
        if (is_sequence("false")) { t = value_type(false); return true; }
        return false;
    }
    
    bool is_null(value_type& t) {
        if (!is_sequence("null")) return false;
        t = value_type();
        return true;
    }
    
    template <typename S> // S is string_type or key_type
    bool is_string(S& s) {
        if (!is_char('"')) return false;
        while (!is_char('"')) {
            const char* f = p_;
            while (!('\x00' <= *p_ && *p_ < '\x20') && *p_ != '"' && *p_ != '\\') ++p_;
            require(!('\x00' <= *p_ && *p_ < '\x20'), "valid character");
            T::append(s, f, p_);
            if (!is_char('\\')) continue;
            char c = escape()[static_cast<unsigned char>(*p_++)];
            require(c, "valid escape character");
            if (c == 'u') {
                std::uint32_t utf = get_four_digits();
                // handle surrogate leading
                if (0xD800 <= utf && utf <= 0xDBFF) {
                    require(is_char('u'), "trail surrogate");
                    std::uint32_t trailing = get_four_digits();
                    require(0xDC00 <= trailing && trailing <= 0xDFFF, "trail surrogate");
                    utf = ((utf - 0xD800) << 10) + (trailing - 0xDC00);
                }
                char buffer[4];
                auto end = utf8(utf, buffer);
                T::append(s, buffer, end);
            }
            else T::append(s, &c, &c + 1);
        }
        return  true;
    }
    
    bool is_string(value_type& t) {
        string_type string;
        bool result = is_string(string);
        if (result)
            t = value_type(move(string));
        return result;
    }

    bool is_number(value_type& t) {
        const char* p = p_;
        bool neg = is_char('-');
        if (!is_int()) {
            require(!neg, "digit");
            return false;
        }
        frac();
        exp();

        int count = 0;
        double value = s2d_.StringToDouble(p, static_cast<int>(p_ - p), &count);

        require(std::isfinite(value), "finite number");
        ADOBE_ASSERT(count == p_ - p && "StringToDouble() failure");

        t = value_type(value);
        return true;
    }
    
    void frac() {
        if (!is_char('.')) return;
        require(is_digit(), "digit");
        
        while (is_digit()) ;
    }
    
    void exp() {
        if (!is_char('e') && !is_char('E')) return;
        is_char('-') || is_char('+');
        require(is_digit(), "exponent digit");
        while (is_digit()) ;
    }
    
    bool is_digit() {
        if (!('0' <= *p_ && *p_ <= '9')) return false;
        ++p_;
        return true;
    }

    bool is_int() {
        if (is_char('0')) return true;
        if (!is_digit()) return false;
        while (is_digit()) ;
        return true;
    }

    std::uint32_t get_four_digits() {
        std::uint32_t result;
        char digit = hex_digit()[static_cast<unsigned char>(*p_++)];
        if (digit == '\xFF') goto fail;
        result = digit * 0x1000;
        digit = hex_digit()[static_cast<unsigned char>(*p_++)];
        if (digit == '\xFF') goto fail;
        result += digit * 0x0100;
        digit = hex_digit()[static_cast<unsigned char>(*p_++)];
        if (digit == '\xFF') goto fail;
        result += digit * 0x0010;
        digit = hex_digit()[static_cast<unsigned char>(*p_++)];
        if (digit == '\xFF') goto fail;
        result += digit;
        return result;
    fail:
        require(false, "four hex digits");
        return 0;
    }
    
    /**
        \todo (sparent) : should report error with offset, line number and
                          line offset, and better phrasing.
    */
    void require(bool x, const char* failure) {
        using namespace std;

        if (!x) throw logic_error(failure + string(" is required"));
    }
    
    /// \todo (sparent) : table is overkill, replace with switch and agnostic line ending counter.
    void skip_white_space() { while (ws()[static_cast<unsigned char>(*p_)]) { ++p_; } }
    
    bool is_char(char x) {
        if (*p_ != x) return false;
        ++p_;
        return true;
    }
    
    bool is_structural_char(char x) {
        skip_white_space();
        if (!is_char(x)) return false;
        skip_white_space();
        return true;
    }
    
    /// \pre f points to at least 4 characters.
    char* utf8(std::uint32_t x, char* f) {
        // Should assert non-surrogate
        std::size_t bytes_to_write(0);

        if (x < 0x80) bytes_to_write = 1;
        else if (x < 0x800) bytes_to_write = 2;
        else if (x < 0x10000) bytes_to_write = 3;
        else if (x < 0x0010FFFF) bytes_to_write = 4;
        else require(false, "valid utf-32 character");
        
        const std::uint32_t mark = 0x80;
        const std::uint32_t mask = 0xBF;
        
        static const unsigned char first_mark[] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
        
        f += bytes_to_write;
        
        switch (bytes_to_write) { /* note: everything falls through. */
            case 4: *--f = static_cast<char>((x | mark) & mask); x >>= 6;
            case 3: *--f = static_cast<char>((x | mark) & mask); x >>= 6;
            case 2: *--f = static_cast<char>((x | mark) & mask); x >>= 6;
            case 1: *--f = static_cast<char> (x | first_mark[bytes_to_write]);
        }
       
        return f + bytes_to_write;
    }
    
    const char*                                p_;
    double_conversion::StringToDoubleConverter s2d_;

    typedef char table_t_[256];
    
    static const table_t_& hex_digit() {
        static const char hex_digit_[] = {
        /*        0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F */
        /* 0 */ '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* 1 */ '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* 2 */ '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* 3 */ '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* 4 */ '\xFF', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* 5 */ '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* 6 */ '\xFF', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* 7 */ '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* 8 */ '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* 9 */ '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* A */ '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* B */ '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* C */ '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* D */ '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* E */ '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
        /* F */ '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF'
        };
        return hex_digit_;
    }
    
    static const table_t_& escape() {
        static const char escape_[] = {
        /*        0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F */
        /* 0 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', 
        /* 1 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* 2 */ '\x00', '\x00', '\x22', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x2F',
        /* 3 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* 4 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* 5 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x5C', '\x00', '\x00', '\x00',
        /* 6 */ '\x00', '\x00', '\x08', '\x00', '\x00', '\x00', '\x0C', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x0A', '\x00',
        /* 7 */ '\x00', '\x00', '\x0D', '\x00', '\x09', '\x75', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* 8 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* 9 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* A */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* B */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* C */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* D */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* E */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* F */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00'
        };
        return escape_;
    }
    
    static const table_t_& ws() {
        static const char ws_[] = {
        /*        0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F */
        /* 0 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x01', '\x01', '\x00', '\x00', '\x01', '\x00', '\x00',
        /* 1 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* 2 */ '\x01', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* 3 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* 4 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* 5 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* 6 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* 7 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* 8 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* 9 */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* A */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* B */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* C */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* D */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* E */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
        /* F */ '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00'
        };
        return ws_;
    }
};

/**************************************************************************************************/

enum class json_type {
    object, array, string, number, boolean, null
};

/**************************************************************************************************/
/**
    \ingroup json

    \brief A utility class that uses a helper class to access a provided data
           structure and output well-formed JSON.

    \todo (sparent): Add the following options: (pretty, precise, ascii, ordered).
                     Default are (compact, ?decimal, readible?, utf8, unordered.)
*/
template <typename T,
          typename O>
class json_generator {
  public:
    typedef typename T::object_type object_type;
    typedef typename T::pair_type pair_type; // type returned for *begin(object_type)
    typedef typename T::array_type array_type;
    typedef typename T::value_type value_type;
    typedef typename T::string_type string_type;
    typedef typename T::key_type key_type;
    
    json_generator(O out) : out_(out) { };
    
    O generate(const value_type& value, std::size_t indent = 0) {
        switch (T::type(value)) {
        case json_type::object:
        case json_type::array:
            generate_(value, indent);
            break;
        default:
            require(false, "object or array");
        }
        return out_;
    }

  private:
    void require(bool x, const char* message) {
        if (!x) throw std::logic_error(message);
    }
    
    void generate_(const value_type& value, std::size_t indent) {
        switch (T::type(value)) {
        case json_type::object:
            generate_(as<object_type>(value), indent);
            break;
        case json_type::array:
            generate_(as<array_type>(value), indent);
            break;
        case json_type::string:
            generate_string(as<string_type>(value));
            break;
        case json_type::number:
            generate_(as<double>(value));
            break;
        case json_type::boolean:
            generate_(as<bool>(value));
            break;
        case json_type::null:
            generate_();
            break;
        default:
            require(false, "valid type");
            break;
        }
    }
    
    void generate_() {
        static const char null_[] = "null";
        out_ = std::copy(std::begin(null_), std::end(null_) - 1, out_);
    }
    
    void generate_(bool x) {
        static const char true_[] = "true";
        static const char false_[] = "false";
        if (x) out_ = std::copy(std::begin(true_), std::end(true_) - 1, out_);
        else out_ = std::copy(std::begin(false_), std::end(false_) - 1, out_);
    }
    
    void generate_(double x) {
        require(!std::isnan(x) && !std::isinf(x), "valid double");
        out_ = adobe::to_string(x, out_);
    }
    
    template<typename U>
    static const U& as(const value_type& x) { return T::template as<U>(x); }
    
    void indent(std::size_t n) {
        for(std::size_t n_ = 0; n_ != n; ++n_) *out_++ = '\t';
    }
    void space() { *out_++ = ' ';}
    void endl() { *out_++ = '\n'; }
    
    template <typename S>
    void generate_string(const S& x) {
        static const char hex_digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

        *out_++ = '"';
        // REVISIT (sparent) : do escape handling here
        for (const auto& e : x) {
            if (('\x00' <= e && e <= '\x1F') || e == '"' || e == '\\') {
                *out_++ = '\\';
                switch (e) {
                case '"':   *out_++ = '"'; break;
                case '\\':  *out_++ = '\\'; break;
                case '\b':  *out_++ = 'b'; break;
                case '\f':  *out_++ = 'f'; break;
                case '\n':  *out_++ = 'n'; break;
                case '\r':  *out_++ = 'r'; break;
                case '\t':  *out_++ = 't'; break;
                default:
                    *out_++ = 'u';
                    *out_++ = '0';
                    *out_++ = '0';
                    *out_++ = hex_digits[static_cast<unsigned char>((e >> 4) & '\x0F')];
                    *out_++ = hex_digits[static_cast<unsigned char>(e & '\x0F')];
                    break;
                }
            } else *out_++ = e;
        }
        *out_++ = '"';
    }
    
    void generate_(const pair_type& value, std::size_t n) {
        generate_string(value.first); *out_++ = ':'; space(); generate_(value.second, n);
    }
        
    template <typename I> // I models forward iterator
    bool list(I f, I l, std::size_t n) {
        if (f == l) return false;
        I next = f; ++next;
        
        bool is_one = next == l;
        
        if (is_one) { space(); }
        else { endl(); n += 1; indent(n); }
        
        generate_(*f, n);
        
        if (is_one) { return false; }
        
        f = next;
        while (f != l) {
            *out_++ = ','; endl();
            indent(n); generate_(*f, n);
            ++f;
        }
        endl();
        return true;
    }
    
    void generate_(const object_type& value, std::size_t n) {
        *out_++ = '{';
        if (list(begin(value), end(value), n)) indent(n);
        else space();
        *out_++ = '}';
    }
    
    void generate_(const array_type& value, std::size_t n) {
        *out_++ = '[';
        if (list(begin(value), end(value), n)) indent(n);
        else space();
        *out_++ = ']';
    }
    
    O out_;
};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
// ADOBE_JSON_HPP
#endif

/**************************************************************************************************/
