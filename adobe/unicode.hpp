/*
    Copyright 2012 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_UNICODE_HPP
#define ADOBE_UNICODE_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <cassert>
#include <stdexcept>
#include <iterator>

#include <boost/bind.hpp>
#include <boost/cstdint.hpp>
#include <boost/utility/enable_if.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

#ifndef ADOBE_NO_DOCUMENTATION

/**************************************************************************************************/

template <typename T>
struct is_utf8_type
{ enum { value = sizeof(T) == 1 }; };

/**************************************************************************************************/

template <typename T>
struct is_utf16_type
{ enum { value = sizeof(T) == 2 }; };

/**************************************************************************************************/

template <typename T>
struct is_utf32_type
{ enum { value = sizeof(T) == 4 }; };

/**************************************************************************************************/

template <typename I>
struct is_utf8_iterator_type
{ enum { value = is_utf8_type<typename std::iterator_traits<I>::value_type>::value }; };

/**************************************************************************************************/

template <typename I>
struct is_utf16_iterator_type
{ enum { value = is_utf16_type<typename std::iterator_traits<I>::value_type>::value }; };

/**************************************************************************************************/

template <typename I>
struct is_utf32_iterator_type
{ enum { value = is_utf32_type<typename std::iterator_traits<I>::value_type>::value }; };

/**************************************************************************************************/

namespace detail {

/**************************************************************************************************/

template <std::size_t N> struct unicode_size_type_ { };

template <typename I> // I models InputIterator
struct unicode_size_type {
    typedef unicode_size_type_<sizeof(typename std::iterator_traits<I>::value_type)> type;
};

/**************************************************************************************************/

const unsigned char     to_utf32_pivot_1_k(128);
const unsigned char     to_utf32_pivot_2_k(192);
const unsigned char     to_utf32_pivot_3_k(224);
const unsigned char     to_utf32_pivot_4_k(240);
const unsigned char     to_utf32_pivot_5_k(248);

const boost::uint32_t   to_utf8_pivot_1_k(1UL << 7);
const boost::uint32_t   to_utf8_pivot_2_k(1UL << 11);
const boost::uint32_t   to_utf8_pivot_3_k(1UL << 16);
const boost::uint32_t   to_utf8_pivot_4_k(1UL << 21);

const boost::uint16_t   to_utf16_surrogate_pivot_k(65535);
const boost::uint16_t   utf16_high_surrogate_front_k(0xd800);
const boost::uint16_t   utf16_high_surrogate_back_k(0xdbff);
const boost::uint16_t   utf16_low_surrogate_front_k(0xdc00);
const boost::uint16_t   utf16_low_surrogate_back_k(0xdfff);

/**************************************************************************************************/

template <std::size_t NumBytes> struct utf8_header_t    { };

template <>     struct utf8_header_t<0> { static const char value = '\x80'; }; // nonheader
// template <>  struct utf8_header_t<1> { static const char value = '\x00'; }; // illegal
template <>     struct utf8_header_t<2> { static const char value = '\xC0'; };
template <>     struct utf8_header_t<3> { static const char value = '\xE0'; };
template <>     struct utf8_header_t<4> { static const char value = '\xF0'; };
// template <>  struct utf8_header_t<5> { static const char value = '\xF8'; }; // illegal
// template <>  struct utf8_header_t<6> { static const char value = '\xFC'; }; // illegal

/**************************************************************************************************/

template <char Mask, typename BinaryInteger>
inline char add_mask(BinaryInteger code)
{ return static_cast<char>(code | Mask); }

template <std::size_t NumBytes, bool Header, typename BinaryInteger>
inline char utf8_add_mask(BinaryInteger code)
{ return add_mask<utf8_header_t<Header ? NumBytes : 0>::value>(code); }


//MM concept gcc-4.1.1 workaround
inline char utf8_add_mask_0_false(boost::uint32_t code)
{
    return utf8_add_mask<0,false>(code);
}

/**************************************************************************************************/

template<char Mask, typename BinaryInteger>
inline char strip_mask(BinaryInteger code)
{ return static_cast<char>(code & ~Mask); }

template <std::size_t NumBytes, bool Header, typename BinaryInteger>
inline char utf8_strip_mask(BinaryInteger code)
{ return strip_mask<utf8_header_t<Header ? NumBytes : 0>::value>(code); }

/**************************************************************************************************/

template <std::size_t Position>
inline boost::uint32_t promote_fragment(char fragment)
{ return boost::uint32_t(fragment << ((Position - 1) * 6)); }

template <>
inline boost::uint32_t promote_fragment<1>(char fragment)
{ return boost::uint32_t(fragment); }

template <>
inline boost::uint32_t promote_fragment<0>(char); // unimplemented

/**************************************************************************************************/

template <std::size_t Position>
inline char demote_fragment(boost::uint32_t fragment)
{ return char((fragment >> ((Position - 1) * 6)) & 0x0000003F); }

template <>
inline char demote_fragment<1>(boost::uint32_t fragment)
{ return char(fragment & 0x0000003F); }

template <>
inline char demote_fragment<0>(boost::uint32_t); // unimplemented

//MM concept gcc-4.1.1 workaround 
inline char demote_fragment_1(boost::uint32_t fragment)
{
    return demote_fragment<1>(fragment);
}


/**************************************************************************************************/

template <std::size_t ByteCount, bool Header = true>
struct demotion_engine_t
{
    template <typename OutputIterator>
    inline OutputIterator operator () (boost::uint32_t code, OutputIterator i)
    {
        *i = utf8_add_mask<ByteCount, Header>(demote_fragment<ByteCount>(code));

        ++i;

        return demotion_engine_t<ByteCount - 1, false>()(code, i);
    }
};


template <>
struct demotion_engine_t<1, false>
{
    template <typename OutputIterator>
    inline OutputIterator operator () (boost::uint32_t code, OutputIterator i)
    {
        *i = utf8_add_mask_0_false(demote_fragment_1(code));

        return ++i;
    }
};

/**************************************************************************************************/

template <std::size_t ByteCount, bool Header = true>
struct promotion_engine_t
{
    template <typename InputIterator>
    inline boost::uint32_t operator () (InputIterator& first, InputIterator last)
    {
        /*
            CodeWarrior 9.4 doesn't like this code composited into one line;
            GCC doesn't seem to have a problem.
        */

        char            n(*first);
        char            stripped(utf8_strip_mask<ByteCount, Header>(n));
        boost::uint32_t shifted(promote_fragment<ByteCount>(stripped));

        ++first;

        if (first == last)
            throw std::runtime_error("unicode: utf32 conversion ran out of input");

        return shifted | promotion_engine_t<ByteCount - 1, false>()(first, last);
    }
};

template <>
struct promotion_engine_t<1, false>
{
    template <typename InputIterator>
    inline boost::uint32_t operator () (InputIterator& first, InputIterator)
    {
        boost::uint32_t result(promote_fragment<1>(utf8_strip_mask<0, false>(*first)));

        ++first;

        return result;
    }
};

/**************************************************************************************************/

template <typename InputIterator, typename DestInteger>
InputIterator to_utf32 (InputIterator first, InputIterator last, DestInteger& result, unicode_size_type_<2>)
{
    if (first == last) return first;

    boost::uint16_t code(static_cast<boost::uint16_t>(*first));

    ++first;

    if (code >= detail::utf16_high_surrogate_front_k &&
        code <= detail::utf16_high_surrogate_back_k)
    {
        result = 0;

        if (first == last)
            throw std::runtime_error("unicode: UTF-16 high surrogate found without low surrogate"); 

        boost::uint16_t low(static_cast<boost::uint16_t>(*first));

        assert (low >= detail::utf16_low_surrogate_front_k &&
                low <= detail::utf16_low_surrogate_back_k);

        ++first;

        result = (code - detail::utf16_high_surrogate_front_k) * 0x400 +
                 (low - detail::utf16_low_surrogate_front_k) + 0x10000;
    }
    else if (code >= detail::utf16_low_surrogate_front_k &&
             code <= detail::utf16_low_surrogate_back_k)
        { throw std::runtime_error("unicode: UTF-16 low surrogate found without high surrogate"); }
    else
        { result = static_cast<DestInteger>(code); }

    return first;
}

/**************************************************************************************************/

template <typename InputIterator, typename DestInteger>
InputIterator to_utf32 (InputIterator first, InputIterator last, DestInteger& result, unicode_size_type_<1>)
{
    if (first == last)
        return first;

    unsigned char n(static_cast<unsigned char>(*first));

    if (n < detail::to_utf32_pivot_1_k)
        { result = static_cast<DestInteger>(n); ++first; }
    else if (n < detail::to_utf32_pivot_2_k)
        { throw std::runtime_error("unicode: ill-defined UTF-8 (first byte is 10xxxxxx)"); }
    else if (n < detail::to_utf32_pivot_3_k)
        result = detail::promotion_engine_t<2>()(first, last);
    else if (n < detail::to_utf32_pivot_4_k)
        result = detail::promotion_engine_t<3>()(first, last);
    else if (n < detail::to_utf32_pivot_5_k)
        result = detail::promotion_engine_t<4>()(first, last);
    else 
        { throw std::runtime_error("unicode: ill-defined UTF-8 (first byte is 11111xxx)"); }
    
    if (result > 0x0010FFFF)
        { throw std::runtime_error("unicode: ill-defined UTF-8 (code point out of range)"); }
    
    if (0x0000D800 <= result && result <= 0x0000DFFF)
        { throw std::runtime_error("unicode: ill-defined UTF-8 (surrogate code point)"); }

    return first;
}

/**************************************************************************************************/

template <typename InputIterator, typename DestInteger>
InputIterator to_utf32 (InputIterator first, InputIterator last, DestInteger& result, unicode_size_type_<4>)
{
    if (first == last)
        return first;

    result = *first;

    return ++first;
}

/**************************************************************************************************/
/*
        utf32 -> utf8
            - 1 source value
            - n output values
*/

template <  typename T, // T models Integer; T must be a valid UTF32-encoded code point
            typename O> // O models OutputIterator
typename boost::enable_if<is_utf32_type<T>, O>::type
    value_to_utf8(T code, O output)
{
    if (code < detail::to_utf8_pivot_1_k) // UTF-8 is 1 byte long
        { *output = static_cast<char>(code); ++output; }
    else if (code < detail::to_utf8_pivot_2_k) // UTF-8 is 2 bytes long
        output = detail::demotion_engine_t<2>()(code, output);
    else if (code < detail::to_utf8_pivot_3_k) // UTF-8 is 3 bytes long
        output = detail::demotion_engine_t<3>()(code, output);
    else if (code < detail::to_utf8_pivot_4_k) // UTF-8 is 4 bytes long
        output = detail::demotion_engine_t<4>()(code, output);
    else throw std::runtime_error("unicode: invalid code point (out of range)");

    return output;
}

/**************************************************************************************************/
/*
        utf16 -> utf8
            - 1 source value
            - n output values
*/

template <  typename T, // T models Integer; T must be a valid UTF16-encoded code point
            typename O> // O models OutputIterator
typename boost::enable_if<is_utf16_type<T>, O>::type
    value_to_utf8(T code, O output)
{
    return value_to_utf8(static_cast<boost::uint32_t>(code), output);
}

/**************************************************************************************************/
/*
        utf8 -> utf8
            - 1 source value
            - 1 output value
*/

template <  typename T, // T models Integer; T must be a valid UTF8-encoded code point
            typename O> // O models OutputIterator
typename boost::enable_if<is_utf8_type<T>, O>::type
    value_to_utf8(T code, O output)
{
    *output++ = code;

    return output;
}

/**************************************************************************************************/
/*
        utf32 -> utf16
            - 1 source value
            - n output values
*/

template <  typename T, // T models Integer; sizeof(T) must equal 4; code must be valid utf32
            typename O> // O models OutputIterator
typename boost::enable_if<is_utf32_type<T>, O>::type
    value_to_utf16(T code, O output)
{
    if (code <= detail::to_utf16_surrogate_pivot_k)
    {
        *output = static_cast<boost::uint16_t>(code);
    }
    else
    {
        *output = static_cast<boost::uint16_t>((code - 0x10000) / 0x400 + detail::utf16_high_surrogate_front_k);

        ++output;

        *output = static_cast<boost::uint16_t>((code - 0x10000) % 0x400 + detail::utf16_low_surrogate_front_k);
    }

    return ++output;
}

/**************************************************************************************************/
/*
        utf8 -> utf8
            - n source values
            - m output values
*/

template <  typename I, // I models InputIterator
            typename O> // O models OutputIterator
O to_utf8(I first, I last, O output, unicode_size_type_<1>)
{
    return std::copy(first, last, output);
}

/**************************************************************************************************/
/*
        utf16 -> utf8
            - n source values
            - m output values
*/

template <  typename I, // I models InputIterator
            typename O> // O models OutputIterator
O to_utf8(I first, I last, O output, unicode_size_type_<2>)
{
    while (first != last)
    {
        boost::uint32_t result;

        first = detail::to_utf32(first, last, result, unicode_size_type_<2>());

        output = detail::value_to_utf8(result, output);
    }

    return output;
}

/**************************************************************************************************/
/*
        utf32 -> utf8
            - n source values
            - m output values
*/

template <  typename I, // I models InputIterator
            typename O> // O models OutputIterator
O to_utf8(I first, I last, O output, unicode_size_type_<4>)
{
    if (first == last) return output;

    typedef typename std::iterator_traits<I>::value_type value_type;

    std::for_each(first, last, boost::bind(&detail::value_to_utf8<value_type, O>, _1, boost::ref(output)));

    return output;
}

/**************************************************************************************************/
/*
        utf8 -> utf16
            - n source values
            - m output values
*/
template <  typename I, // I models InputIterator
            typename O> // O models OutputIterator
O to_utf16(I first, I last, O output, unicode_size_type_<1>)
{
    while (first != last)
    {
        boost::uint32_t result;

        first = detail::to_utf32(first, last, result, detail::unicode_size_type_<1>());

        output = detail::value_to_utf16(result, output);
    }

    return output;
}

/**************************************************************************************************/
/*
        utf16 -> utf16
            - n source values
            - n output values
*/
template <  typename I, // I models InputIterator
            typename O> // O models OutputIterator
O to_utf16(I first, I last, O output, unicode_size_type_<2>)
{
    return std::copy(first, last, output);
}

/**************************************************************************************************/
/*
        utf32 -> utf16
            - n source values
            - m output values
*/
template <  typename I, // I models InputIterator
            typename O> // O models OutputIterator
O to_utf16(I first, I last, O output, unicode_size_type_<4>)
{
    while (first != last)
    {
        boost::uint32_t result;

        first = detail::to_utf32(first, last, result, detail::unicode_size_type_<1>());

        *output++ = result;
    }

    return output;
}

/**************************************************************************************************/

} // namespace detail

/**************************************************************************************************/

#endif

/**************************************************************************************************/

template <  typename I, // models InputIterator
            typename O> // models OutputIterator
O to_utf8(I f, I l, O o) {
    return detail::to_utf8(f, l, o, typename detail::unicode_size_type<I>::type());
}

/**************************************************************************************************/

template <  typename I, // models InputIterator
            typename O> // models OutputIterator
O to_utf16(I f, I l, O o) {
    return detail::to_utf16(f, l, o, typename detail::unicode_size_type<I>::type());
}

/**************************************************************************************************/
/*
        utf16 -> utf32
            - n source values
            - m output values

        utf8 -> utf32
            - n source values
            - m output values
*/

template <  typename I, // I models InputIterator
            typename O> // O models OutputIterator
O to_utf32(I first, I last, O output)
{
    boost::uint32_t result;

    while (first != last)
    {
        first = detail::to_utf32(first, last, result, typename detail::unicode_size_type<I>::type());

        *output++ = result;
    }

    return output;
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
