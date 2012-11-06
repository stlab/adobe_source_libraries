/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_UNICODE_HPP
#define ADOBE_UNICODE_HPP

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/algorithm/for_each.hpp>

#include <boost/cstdint.hpp>
#include <boost/utility/enable_if.hpp>

#include <vector>
#include <cassert>
#include <stdexcept>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

#if !defined(ADOBE_NO_DOCUMENTATION)

/*************************************************************************************************/

template <typename T>
struct is_utf8_type
{ enum { value = sizeof(T) == 1 }; };

/*************************************************************************************************/

template <typename T>
struct is_utf16_type
{ enum { value = sizeof(T) == 2 }; };

/*************************************************************************************************/

template <typename T>
struct is_utf32_type
{ enum { value = sizeof(T) == 4 }; };

/*************************************************************************************************/

template <typename I>
struct is_utf8_iterator_type
{ enum { value = is_utf8_type<typename std::iterator_traits<I>::value_type>::value }; };

/*************************************************************************************************/

template <typename I>
struct is_utf16_iterator_type
{ enum { value = is_utf16_type<typename std::iterator_traits<I>::value_type>::value }; };

/*************************************************************************************************/

template <typename I>
struct is_utf32_iterator_type
{ enum { value = is_utf32_type<typename std::iterator_traits<I>::value_type>::value }; };

/*************************************************************************************************/

namespace implementation {

/*************************************************************************************************/

// REVISIT (fbrereto) : I don't need to INIT_ONCE these, do I?

const unsigned char     to_utf32_pivot_1_k(128);
const unsigned char     to_utf32_pivot_2_k(192);
const unsigned char     to_utf32_pivot_3_k(224);
const unsigned char     to_utf32_pivot_4_k(240);
const unsigned char     to_utf32_pivot_5_k(248);
const unsigned char     to_utf32_pivot_6_k(252);
const unsigned char     to_utf32_pivot_7_k(254);

const boost::uint32_t   to_utf8_pivot_1_k(1UL << 7);
const boost::uint32_t   to_utf8_pivot_2_k(1UL << 11);
const boost::uint32_t   to_utf8_pivot_3_k(1UL << 16);
const boost::uint32_t   to_utf8_pivot_4_k(1UL << 21);
const boost::uint32_t   to_utf8_pivot_5_k(1UL << 26);

const boost::uint16_t   to_utf16_surrogate_pivot_k(65535);
const boost::uint16_t   utf16_high_surrogate_front_k(0xd800);
const boost::uint16_t   utf16_high_surrogate_back_k(0xdbff);
const boost::uint16_t   utf16_low_surrogate_front_k(0xdc00);
const boost::uint16_t   utf16_low_surrogate_back_k(0xdfff);

/*************************************************************************************************/
/*
    NOTE (fbrereto) : The char(...) designations are required on windows, otherwise the MSVC
                      compiler complains in the utf8_add_mask routines with the following:

                      "warning C4309: 'specialization' : truncation of constant value"
*/
template <std::size_t NumBytes> struct utf8_header_t    { };
template <>                     struct utf8_header_t<0> { static const char value = '\x80'; }; // nonheader
//template <>                   struct utf8_header_t<1> { static const char value = '\x00'; }; // illegal
template <>                     struct utf8_header_t<2> { static const char value = '\xC0'; };
template <>                     struct utf8_header_t<3> { static const char value = '\xE0'; };
template <>                     struct utf8_header_t<4> { static const char value = '\xF0'; };
template <>                     struct utf8_header_t<5> { static const char value = '\xF8'; };
template <>                     struct utf8_header_t<6> { static const char value = '\xFC'; };

/*************************************************************************************************/

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

/*************************************************************************************************/

template<char Mask, typename BinaryInteger>
inline char strip_mask(BinaryInteger code)
{ return static_cast<char>(code & ~Mask); }

template <std::size_t NumBytes, bool Header, typename BinaryInteger>
inline char utf8_strip_mask(BinaryInteger code)
{ return strip_mask<utf8_header_t<Header ? NumBytes : 0>::value>(code); }

/*************************************************************************************************/

template <std::size_t Position>
inline boost::uint32_t promote_fragment(char fragment)
{ return boost::uint32_t(fragment << ((Position - 1) * 6)); }

template <>
inline boost::uint32_t promote_fragment<1>(char fragment)
{ return boost::uint32_t(fragment); }

template <>
inline boost::uint32_t promote_fragment<0>(char); // unimplemented

/*************************************************************************************************/

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


/*************************************************************************************************/

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

/*************************************************************************************************/

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

/*************************************************************************************************/

template <typename InputIterator, typename DestInteger>
typename boost::enable_if<is_utf16_iterator_type<InputIterator>, InputIterator>::type
    to_utf32 (InputIterator first, InputIterator last, DestInteger& result)
{
    if (first == last) return first;

    boost::uint16_t code(static_cast<boost::uint16_t>(*first));

    ++first;

    if (code >= implementation::utf16_high_surrogate_front_k &&
        code <= implementation::utf16_high_surrogate_back_k)
    {
        result = 0;

        if (first == last)
            throw std::runtime_error("unicode: utf16 high surrogate found without low surrogate"); 

        boost::uint16_t low(static_cast<boost::uint16_t>(*first));

        assert (low >= implementation::utf16_low_surrogate_front_k &&
                low <= implementation::utf16_low_surrogate_back_k);

        ++first;

        result = (code - implementation::utf16_high_surrogate_front_k) * 0x400 +
                 (low - implementation::utf16_low_surrogate_front_k) + 0x10000;
    }
    else if (code >= implementation::utf16_low_surrogate_front_k &&
             code <= implementation::utf16_low_surrogate_back_k)
        { throw std::runtime_error("unicode: utf16 low surrogate found without high surrogate"); }
    else
        { result = static_cast<DestInteger>(code); }

    return first;
}

/*************************************************************************************************/

template <typename InputIterator, typename DestInteger>
typename boost::enable_if<is_utf8_iterator_type<InputIterator>, InputIterator>::type
    to_utf32 (InputIterator first, InputIterator last, DestInteger& result)
{
    if (first == last)
        return first;

    unsigned char n(static_cast<unsigned char>(*first));

    if (n < implementation::to_utf32_pivot_1_k)
        { result = static_cast<DestInteger>(n); ++first; }
    else if (n < implementation::to_utf32_pivot_2_k)
        { throw std::runtime_error("unicode: ill-defined utf8 (< 192)"); }
    else if (n < implementation::to_utf32_pivot_3_k)
        result = implementation::promotion_engine_t<2>()(first, last);
    else if (n < implementation::to_utf32_pivot_4_k)
        result = implementation::promotion_engine_t<3>()(first, last);
    else if (n < implementation::to_utf32_pivot_5_k)
        result = implementation::promotion_engine_t<4>()(first, last);
    else if (n < implementation::to_utf32_pivot_6_k)
        result = implementation::promotion_engine_t<5>()(first, last);
    else if (n < implementation::to_utf32_pivot_7_k)
        result = implementation::promotion_engine_t<6>()(first, last);
    else
        { throw std::runtime_error("unicode: ill-defined utf8 (>= 254)"); }

    return first;
}

/*************************************************************************************************/

template <typename InputIterator, typename DestInteger>
typename boost::enable_if<is_utf32_iterator_type<InputIterator>, InputIterator>::type
    to_utf32 (InputIterator first, InputIterator last, DestInteger& result)
{
    if (first == last)
        return first;

    result = *first;

    return ++first;
}

/*************************************************************************************************/

} // namespace implementation

/*************************************************************************************************/

#endif

/*************************************************************************************************/
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
    if (code < implementation::to_utf8_pivot_1_k) // UTF-8 is 1 byte long
        { *output = static_cast<char>(code); ++output; }
    else if (code < implementation::to_utf8_pivot_2_k) // UTF-8 is 2 bytes long
        output = implementation::demotion_engine_t<2>()(code, output);
    else if (code < implementation::to_utf8_pivot_3_k) // UTF-8 is 3 bytes long
        output = implementation::demotion_engine_t<3>()(code, output);
    else if (code < implementation::to_utf8_pivot_4_k) // UTF-8 is 4 bytes long
        output = implementation::demotion_engine_t<4>()(code, output);
    else if (code < implementation::to_utf8_pivot_5_k) // UTF-8 is 5 bytes long
        output = implementation::demotion_engine_t<5>()(code, output);
    else // UTF-8 is 6 bytes long
        output = implementation::demotion_engine_t<6>()(code, output);

    return output;
}

/*************************************************************************************************/
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

/*************************************************************************************************/
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

/*************************************************************************************************/
/*
        utf16 -> utf8
            - n source values
            - m output values
*/

template <  typename I, // I models InputIterator
            typename O> // O models OutputIterator
typename boost::enable_if<is_utf16_iterator_type<I>, O>::type
    to_utf8(I first, I last, O output)
{
    while (first != last)
    {
        boost::uint32_t result;

        first = implementation::to_utf32(first, last, result);

        output = value_to_utf8(result, output);
    }

    return output;
}

/*************************************************************************************************/
/*
        utf32 -> utf8
            - n source values
            - m output values
*/

template <  typename I, // I models InputIterator
            typename O> // O models OutputIterator
typename boost::enable_if<is_utf32_iterator_type<I>, O>::type
    to_utf8(I first, I last, O output)
{
    if (first == last) return output;

    typedef typename std::iterator_traits<I>::value_type value_type;

    adobe::for_each(first, last, boost::bind(&value_to_utf8<value_type, O>, _1, boost::ref(output)));

    return output;
}

/*************************************************************************************************/
/*
        utf8 -> utf8
            - n source values
            - m output values
*/

template <  typename I, // I models InputIterator
            typename O> // O models OutputIterator
typename boost::enable_if<is_utf8_iterator_type<I>, O>::type
    to_utf8(I first, I last, O output)
{
    return std::copy(first, last, output);
}

/*************************************************************************************************/
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
    if (code <= implementation::to_utf16_surrogate_pivot_k)
    {
        *output = static_cast<boost::uint16_t>(code);
    }
    else
    {
        *output = static_cast<boost::uint16_t>((code - 0x10000) / 0x400 + implementation::utf16_high_surrogate_front_k);

        ++output;

        *output = static_cast<boost::uint16_t>((code - 0x10000) % 0x400 + implementation::utf16_low_surrogate_front_k);
    }

    return ++output;
}

/*************************************************************************************************/
/*
        utf8 -> utf16
            - n source values
            - m output values
*/
template <  typename I, // I models InputIterator
            typename O> // O models OutputIterator
typename boost::enable_if<is_utf8_iterator_type<I>, O>::type
    to_utf16(I first, I last, O output)
{
    while (first != last)
    {
        boost::uint32_t result;

        first = implementation::to_utf32(first, last, result);

        output = value_to_utf16(result, output);
    }

    return output;
}

/*************************************************************************************************/
/*
        utf16 -> utf16
            - n source values
            - n output values
*/
template <  typename I, // I models InputIterator
            typename O> // O models OutputIterator
typename boost::enable_if<is_utf16_iterator_type<I>, O>::type
    to_utf16(I first, I last, O output)
{
    return std::copy(first, last, output);
}

/*************************************************************************************************/
/*
    Precondition: [ first, last ) must convert to exactly one UTF-16 character
*/

template <typename I>
inline typename boost::enable_if<is_utf8_iterator_type<I>, boost::uint16_t>::type
    to_utf16(I first, I last)
{
    boost::uint32_t result;

    implementation::to_utf32(first, last, result);

    return static_cast<boost::uint16_t>(result);
}

/*************************************************************************************************/
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
        first = implementation::to_utf32(first, last, result);

        *output = result;

        ++output;
    }

    return output;
}

/*************************************************************************************************/
/*
    Precondition: [ first, last ) must convert to exactly one UTF-32 character
*/

template <typename I> // I models InputIterator
inline boost::uint32_t to_utf32(I first, I last)
{
    boost::uint32_t result;

    implementation::to_utf32(first, last, result);

    return result;
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif
    
/*************************************************************************************************/
