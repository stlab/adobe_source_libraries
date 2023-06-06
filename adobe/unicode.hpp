/*
    Copyright 2012 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_UNICODE_HPP
#define ADOBE_UNICODE_HPP

/**************************************************************************************************/

#include <cstdint>
#include <iterator>

#include <adobe/cassert.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

namespace detail {

/**************************************************************************************************/

template <std::size_t N>
struct unicode_size_type_ {};

template <typename I> // I models InputIterator
struct unicode_size_type {
    typedef unicode_size_type_<sizeof(typename std::iterator_traits<I>::value_type)> type;
};

/**************************************************************************************************/

const unsigned char to_utf32_pivot_1_k(128);
const unsigned char to_utf32_pivot_2_k(192);
const unsigned char to_utf32_pivot_3_k(224);
const unsigned char to_utf32_pivot_4_k(240);
const unsigned char to_utf32_pivot_5_k(248);

const std::uint32_t to_utf8_pivot_1_k(1UL << 7);
const std::uint32_t to_utf8_pivot_2_k(1UL << 11);
const std::uint32_t to_utf8_pivot_3_k(1UL << 16);
const std::uint32_t to_utf8_pivot_4_k(1UL << 21);

/**************************************************************************************************/

template <std::size_t NumBytes>
struct utf8_header_t {};

template <>
struct utf8_header_t<0> {
    static const char value = '\x80';
}; // nonheader
// template <>  struct utf8_header_t<1> { static const char value = '\x00'; }; // illegal
template <>
struct utf8_header_t<2> {
    static const char value = '\xC0';
};
template <>
struct utf8_header_t<3> {
    static const char value = '\xE0';
};
template <>
struct utf8_header_t<4> {
    static const char value = '\xF0';
};
// template <>  struct utf8_header_t<5> { static const char value = '\xF8'; }; // illegal
// template <>  struct utf8_header_t<6> { static const char value = '\xFC'; }; // illegal

/**************************************************************************************************/

template <char Mask, typename BinaryInteger>
inline char add_mask(BinaryInteger code) {
    return static_cast<char>(static_cast<char>(code) | Mask);
}

template <std::size_t NumBytes, bool Header, typename BinaryInteger>
inline char utf8_add_mask(BinaryInteger code) {
    return add_mask<utf8_header_t<Header ? NumBytes : 0>::value>(code);
}


// MM concept gcc-4.1.1 workaround
inline char utf8_add_mask_0_false(char code) { return utf8_add_mask<0, false>(code); }

/**************************************************************************************************/

template <char Mask, typename BinaryInteger>
inline char strip_mask(BinaryInteger code) {
    return static_cast<char>(code & ~Mask);
}

template <std::size_t NumBytes, bool Header, typename BinaryInteger>
inline char utf8_strip_mask(BinaryInteger code) {
    return strip_mask<utf8_header_t<Header ? NumBytes : 0>::value>(code);
}

/**************************************************************************************************/

template <std::size_t Position>
inline std::uint32_t promote_fragment(char fragment) {
    return std::uint32_t(fragment << ((Position - 1) * 6));
}

template <>
inline std::uint32_t promote_fragment<1>(char fragment) {
    return std::uint32_t(fragment);
}

template <>
inline std::uint32_t promote_fragment<0>(char); // unimplemented

/**************************************************************************************************/

template <std::size_t Position>
inline char demote_fragment(std::uint32_t fragment) {
    return char((fragment >> ((Position - 1) * 6)) & 0x0000003F);
}

template <>
inline char demote_fragment<1>(std::uint32_t fragment) {
    return char(fragment & 0x0000003F);
}

template <>
inline char demote_fragment<0>(std::uint32_t); // unimplemented

// MM concept gcc-4.1.1 workaround
inline char demote_fragment_1(std::uint32_t fragment) { return demote_fragment<1>(fragment); }


/**************************************************************************************************/

template <typename T, std::size_t ByteCount, bool Header = true>
struct demotion_engine_t {
    template <typename OutputIterator>
    inline OutputIterator operator()(std::uint32_t code, OutputIterator out) {
        *out = static_cast<T>(utf8_add_mask<ByteCount, Header>(demote_fragment<ByteCount>(code)));

        ++out;

        return demotion_engine_t<T, ByteCount - 1, false>()(code, out);
    }
};


template <typename T>
struct demotion_engine_t<T, 1, false> {
    template <typename OutputIterator>
    inline OutputIterator operator()(std::uint32_t code, OutputIterator out) {
        *out = static_cast<T>(utf8_add_mask_0_false(demote_fragment_1(code)));

        return ++out;
    }
};

/**************************************************************************************************/

template <std::size_t ByteCount, bool Header = true>
struct promotion_engine_t {
    template <typename InputIterator>
    inline std::uint32_t operator()(InputIterator& first, InputIterator last) {
        /*
            CodeWarrior 9.4 doesn't like this code composited into one line;
            GCC doesn't seem to have a problem.
        */

        char n = static_cast<char>(*first);
        char stripped(utf8_strip_mask<ByteCount, Header>(n));
        std::uint32_t shifted(promote_fragment<ByteCount>(stripped));

        ++first;

        if (first == last) {
            ADOBE_ASSERT(false && "unicode: UTF-8 to UTF-32 conversion ran out of input");
            return 0;
        }

        return shifted | promotion_engine_t<ByteCount - 1, false>()(first, last);
    }
};

template <>
struct promotion_engine_t<1, false> {
    template <typename InputIterator>
    inline std::uint32_t operator()(InputIterator& first, InputIterator) {
        std::uint32_t result(promote_fragment<1>(utf8_strip_mask<0, false>(*first)));

        ++first;

        return result;
    }
};

/**************************************************************************************************/

template <typename InputIterator, typename T>
InputIterator to_utf32(InputIterator first, InputIterator last, T& result, unicode_size_type_<2>) {
    std::uint16_t code = static_cast<std::uint16_t>(*first);
    ++first;

    if (code < 0xD800) {
        result = static_cast<T>(code);
    } else if (code < 0xDC00) {
        if (first == last) {
            ADOBE_ASSERT(false && "unicode: UTF-16 lead surrogate found without trail surrogate");
            return first;
        }

        std::uint16_t trail = static_cast<std::uint16_t>(*first);
        ++first;

        ADOBE_ASSERT((0xDC00 <= trail && trail <= 0xDFFF) &&
                     "unicode: UTF-16 lead surrogate found without trail surrogate");

        result = static_cast<T>(((code - 0xD800) << 10) + (trail - 0xDC00) + 0x10000);
    } else {
        ADOBE_ASSERT(!(code < 0xE000) &&
                     "unicode: UTF-16 trail surrogate found without lead surrogate");
        result = static_cast<T>(code);
    }

    return first;
}

/**************************************************************************************************/

template <typename InputIterator, typename T>
InputIterator to_utf32(InputIterator first, InputIterator last, T& result, unicode_size_type_<1>) {
    unsigned char n(static_cast<unsigned char>(*first));

    if (n < to_utf32_pivot_1_k) {
        result = static_cast<T>(n);
        ++first;
    } else if (n < to_utf32_pivot_3_k) {
        ADOBE_ASSERT(!(n < to_utf32_pivot_2_k) &&
                     "unicode: ill-defined UTF-8 (first byte is 10xxxxxx)");
        result = static_cast<T>(promotion_engine_t<2>()(first, last));
    } else if (n < to_utf32_pivot_4_k) {
        result = static_cast<T>(promotion_engine_t<3>()(first, last));
    } else if (n < to_utf32_pivot_5_k) {
        result = static_cast<T>(promotion_engine_t<4>()(first, last));
    } else {
        ADOBE_ASSERT(false && "unicode: ill-defined UTF-8 (first byte is 11111xxx)");
    }

    ADOBE_ASSERT(!(result > 0x0010FFFF) && "unicode: ill-defined UTF-8 (code point out of range)");

    ADOBE_ASSERT(!(0x0000D800 <= result && result <= 0x0000DFFF) &&
                 "unicode: ill-defined UTF-8 (surrogate code point)");

    return first;
}

/**************************************************************************************************/

template <typename InputIterator, typename T>
InputIterator to_utf32(InputIterator first, InputIterator, T& result, unicode_size_type_<4>) {
    result = static_cast<T>(*first);

    return ++first;
}

/**************************************************************************************************/
/*
        utf32 -> utf8
            - 1 source value
            - n output values
*/

template <typename T, typename O> // O models OutputIterator
O utf32_to_utf8(std::uint32_t code, O output) {
    if (code < to_utf8_pivot_1_k) // UTF-8 is 1 byte long
    {
        *output = static_cast<T>(code);
        ++output;
    } else if (code < to_utf8_pivot_2_k) // UTF-8 is 2 bytes long
        output = demotion_engine_t<T, 2>()(code, output);
    else if (code < to_utf8_pivot_3_k) // UTF-8 is 3 bytes long
        output = demotion_engine_t<T, 3>()(code, output);
    else if (code < to_utf8_pivot_4_k) // UTF-8 is 4 bytes long
        output = demotion_engine_t<T, 4>()(code, output);
    else
        ADOBE_ASSERT(false && "unicode: invalid code point (out of range)");

    return output;
}

/**************************************************************************************************/
/*
        utf32 -> utf16
            - 1 source value
            - n output values
*/

template <typename T, // output type for O
          typename N, // models Integer; sizeof(T) must equal 4; code must be valid utf32
          typename O>
// models OutputIterator
O utf32_to_utf16(N code, O output) {
    if (code < 0x10000) {
        *output = static_cast<T>(code);
    } else {
        *output = static_cast<T>(((code - 0x10000) >> 10) + 0xD800);

        ++output;

        *output = static_cast<T>(((code - 0x10000) & 0x03FF) + 0xDC00);
    }

    return ++output;
}

/**************************************************************************************************/
/*
        utf8 -> utf8
            - n source values
            - m output values
*/

template <typename T, typename I, // I models InputIterator
          typename O>
// O models OutputIterator
O to_utf8(I first, I last, O output, unicode_size_type_<1>) {
    return std::copy(first, last, output);
}

/**************************************************************************************************/
/*
        utf16 -> utf8
            - n source values
            - m output values
*/

template <typename T, typename I, // I models InputIterator
          typename O>
// O models OutputIterator
O to_utf8(I first, I last, O output, unicode_size_type_<2>) {
    while (first != last) {
        std::uint32_t tmp;

        first = to_utf32(first, last, tmp, unicode_size_type_<2>());

        output = utf32_to_utf8<T>(tmp, output);
    }

    return output;
}

/**************************************************************************************************/
/*
        utf32 -> utf8
            - n source values
            - m output values
*/

template <typename T, typename I, // I models InputIterator
          typename O>
// O models OutputIterator
O to_utf8(I first, I last, O output, unicode_size_type_<4>) {
    while (first != last) {
        output = utf32_to_utf8<T>(static_cast<std::uint32_t>(*first), output);
        ++first;
    }

    return output;
}

/**************************************************************************************************/
/*
        utf8 -> utf16
            - n source values
            - m output values
*/
template <typename T, typename I, // I models InputIterator
          typename O>
// O models OutputIterator
O to_utf16(I first, I last, O output, unicode_size_type_<1>) {
    while (first != last) {
        std::uint32_t result;

        first = to_utf32(first, last, result, unicode_size_type_<1>());

        output = utf32_to_utf16<T>(result, output);
    }

    return output;
}

/**************************************************************************************************/
/*
        utf16 -> utf16
            - n source values
            - n output values
*/
template <typename T, typename I, // I models InputIterator
          typename O>
// O models OutputIterator
O to_utf16(I first, I last, O output, unicode_size_type_<2>) {
    return std::copy(first, last, output);
}

/**************************************************************************************************/
/*
        utf32 -> utf16
            - n source values
            - m output values
*/
template <typename T, typename I, // I models InputIterator
          typename O>
// O models OutputIterator
O to_utf16(I first, I last, O output, unicode_size_type_<4>) {
    while (first != last) {
        output = utf32_to_utf16<T>(*first, output);
        ++first;
    }

    return output;
}

/**************************************************************************************************/

template <typename T, typename I, typename O>
O to_utf_(I f, I l, O o, unicode_size_type_<1>) {
    return to_utf8<T>(f, l, o, typename unicode_size_type<I>::type());
}

template <typename T, typename I, typename O>
O to_utf_(I f, I l, O o, unicode_size_type_<2>) {
    return to_utf16<T>(f, l, o, typename unicode_size_type<I>::type());
}

template <typename T, typename I, typename O>
O to_utf_(I f, I l, O o, unicode_size_type_<4>) {
    T result;

    while (f != l) {
        f = to_utf32(f, l, result, typename unicode_size_type<I>::type());

        *o++ = result;
    }

    return o;
}

/**************************************************************************************************/

template <int A, int B>
struct expand_utf_t;

template <>
struct expand_utf_t<8, 8> {
    static const int value = 1;
};
template <>
struct expand_utf_t<8, 16> {
    static const int value = 1;
};
template <>
struct expand_utf_t<8, 32> {
    static const int value = 1;
};

template <>
struct expand_utf_t<16, 8> {
    static const int value = 3;
};
template <>
struct expand_utf_t<16, 16> {
    static const int value = 1;
};
template <>
struct expand_utf_t<16, 32> {
    static const int value = 1;
};

template <>
struct expand_utf_t<32, 8> {
    static const int value = 4;
};
template <>
struct expand_utf_t<32, 16> {
    static const int value = 2;
};
template <>
struct expand_utf_t<32, 32> {
    static const int value = 1;
};

/**************************************************************************************************/

} // namespace detail

/**************************************************************************************************/

/*!
\ingroup unicode

\tparam T is an 8, 16, or 32 bit integral type for a UTF code point
\tparam U is an 8, 16, or 32 bit integral type for a UTF code point

\result <code>expand_utf<T, U>::value</code> is the maximum expansion when converting a UTF
sequence from T to U determined by the following table:
        |source/result|UTF-8|UTF-16|UTF-32|
        |------------:|:---:|:----:|:----:|
        |UTF-8        |  1  |   1  |   1  |
        |UTF-16       |  3  |   1  |   1  |
        |UTF-32       |  4  |   2  |   1  |

\see copy_utf

*/

template <typename T, typename U>
struct expand_utf
    : std::integral_constant<std::size_t, detail::expand_utf_t<sizeof(T), sizeof(U)>::value> {};

/**************************************************************************************************/

/*!
\ingroup unicode

`copy_utf` copies the text from the range `[f, l)` from UTF-8, 16, or 32 to UTF-8, 16
or 32 and assigns the result to `*o`.

\tparam T must be 8, 16, or 32 bit integral type
\tparam I models InputIterator; value_type(I) must be 8, 16, or 32 bit integral type
\tparam O models OutputIterator; must accept T

\pre `[f, l)` is a valid range of UTF-8, 16, or 32 encode text
\pre `o` is not an iterator within the range `[f, l)`
\pre There is enough space to hold the text being copied. The maximum
      requirement on the output is that `[o, o + m(l - f))` is a valid range
      where `m` is <code>expand_utf<value_type(I), T>::value</code>

\note If the source contains an invalid or partial encoding then the output is undefined (debug
builds may assert). However, the code will not read beyond the specified source range or output
more than the maximum number of elements.

\return An output iterator pointing to the end of the encoded text.
*/

template <typename T, typename I, typename O>
O copy_utf(I f, I l, O o) {
    return detail::to_utf_<T>(f, l, o, detail::unicode_size_type_<sizeof(T)>());
}

/*!
\ingroup unicode

\tparam R is `ConvertibleToRange`

\see copy_utf
*/

template <typename T, typename R, typename O>
O copy_utf(const R& r, O o) {
    return copy_utf<T>(std::begin(r), std::end(r), o);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
