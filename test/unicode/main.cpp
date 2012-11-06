/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/algorithm/copy.hpp>
#include <adobe/unicode.hpp>

#include <boost/cstdint.hpp>
#include <boost/next_prior.hpp>

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

typedef std::vector<boost::uint32_t> utf32_buffer_t;
typedef std::vector<boost::uint16_t> utf16_buffer_t;
typedef std::vector<boost::uint8_t>  utf8_buffer_t;

/****************************************************************************************************/

void roundtrip_test(boost::uint32_t code_point, bool print = true)
{
    /*
        We need to flex all parts of the unicode API given the code point that is passed in.
        This includes:
            - utf32 -> utf8 -> utf32
            - utf32 -> utf16 -> utf32
            - utf16 -> utf8 -> utf16
    */

    utf16_buffer_t  utf16;
    utf8_buffer_t   utf8;

    adobe::value_to_utf16(code_point, std::back_inserter(utf16));
    adobe::value_to_utf8(code_point, std::back_inserter(utf8));

    utf32_buffer_t  utf16_roundtrip;
    utf32_buffer_t  utf8_roundtrip;

    if (print)
    {
        std::cout << "Flexing API for code point 0x" << std::hex << code_point << "..." << std::endl;
        std::cout << "   utf16 encoding is";
        for (utf16_buffer_t::iterator first(utf16.begin()), last(utf16.end()); first != last; ++first)
            std::cout << " 0x" << static_cast<boost::uint32_t>(*first);
        std::cout << std::endl;
        std::cout << "    utf8 encoding is";
        for (utf8_buffer_t::iterator first(utf8.begin()), last(utf8.end()); first != last; ++first)
            std::cout << " 0x" << static_cast<boost::uint32_t>(*first);
        std::cout << std::endl;
        std::cout << std::dec;
    }

    adobe::to_utf32(utf16.begin(), utf16.end(), std::back_inserter(utf16_roundtrip));
    adobe::to_utf32(utf8.begin(), utf8.end(), std::back_inserter(utf8_roundtrip));

    if (utf16_roundtrip[0] != code_point)
        throw std::runtime_error("In utf32 -> utf16 -> utf32: code point mismatch");
    else if (utf8_roundtrip[0] != code_point)
        throw std::runtime_error("In utf32 -> utf8 -> utf32: code point mismatch");

    utf8_buffer_t   utf8_2;
    utf16_buffer_t  utf16_2;

    adobe::to_utf8(utf16.begin(), utf16.end(), std::back_inserter(utf8_2));
    adobe::to_utf16(utf8.begin(), utf8.end(), std::back_inserter(utf16_2));

    utf32_buffer_t  utf16_roundtrip_2;

    adobe::to_utf32(utf16_2.begin(), utf16_2.end(), std::back_inserter(utf16_roundtrip_2));
    
    if (utf16_roundtrip_2[0] != code_point)
        throw std::runtime_error("In utf16 -> utf8 -> utf16: code point mismatch");

    if (print)
    {
        std::cout << "   code point 0x" << std::hex << code_point << std::dec << " (";
        adobe::copy(utf8, std::ostream_iterator<char>(std::cout));
        std::cout << ") roundtripped successfully." << std::endl;
    }
}

/****************************************************************************************************/

bool valid_code_point(boost::uint32_t cp)
{
    return  !(cp >= 0xd800 && cp <= 0xdbff) &&
            !(cp >= 0xdc00 && cp <= 0xdfff);
}

/****************************************************************************************************/

void basic_roundtrip_test()
{
    std::cout << "Singleton Roundtrip Tests" << std::endl;

    roundtrip_test(0x03B4);     // GREEK SMALL LETTER DELTA
    roundtrip_test(0x10137);    // AEGEAN WEIGHT BASE UNIT
    roundtrip_test(0xc5);       // LATIN CAPITAL LETTER A WITH RING ABOVE
    roundtrip_test(0x212b);     // ANGSTROM SIGN
    roundtrip_test(0xF0000);    // Hypothetical private-use glyph
}

/****************************************************************************************************/

void full_roundtrip_test()
{
    std::cout << "Performing complete range roundtrip test..." << std::ends;

    boost::uint32_t first(0x0);
    boost::uint32_t last(0x10ffff);

    try
    {
        for (; first != last; ++first)
            if (valid_code_point(first))
                roundtrip_test(first, false);
    }
    catch (...)
    {
        std::cerr   << std::endl << "While processing value " << std::hex
                    << first << std::dec << "..." << std::endl;
        throw;
    }

    std::cout << "passed!" << std::endl;
}

/****************************************************************************************************/

void bug_test_from_07_27_2008()
{
    utf8_buffer_t   result;
    boost::uint32_t v(0x10abcd);

    adobe::to_utf8(&v, boost::next(&v), back_inserter(result));    

    std::cout << std::hex;

    for (utf8_buffer_t::iterator first(result.begin()), last(result.end()); first != last; ++first)
        std::cout << " 0x" << static_cast<boost::uint32_t>(*first);

    std::cout << std::dec;
}

/****************************************************************************************************/

} // namespace

/*************************************************************************************************/

#if defined(BOOST_MSVC) && defined(BOOST_THREAD_USE_LIB)
extern "C" void tss_cleanup_implemented()
{ }
#endif

/*************************************************************************************************/

int main(int argc, char* argv[])
try
{
    basic_roundtrip_test();

    if (argc > 1 && argv[1] == std::string("--full"))
        full_roundtrip_test();
    else
        std::cout << "Complete roundtrip range test was skipped." << std::endl
                  << "Run this binary with argument --full to "
                  << "test conversion of all Unicode code points." << std::endl;

    bug_test_from_07_27_2008();

    return 0;
}
catch (const std::exception& error)
{
    std::cerr << "Exception: " << error.what() << "\n";

    return 1;
}
catch (...)
{
    std::cerr << "Unknown Exception\n";

    return 1;
}
