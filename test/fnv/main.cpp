/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

#include <vector>
#include <iostream>

#include <adobe/fnv.hpp>

/******************************************************************************/

typedef std::vector<std::uint8_t> data_t;

/******************************************************************************/

template <std::size_t BitCount, typename Container>
void fnvtest(const Container& data, adobe::fnvtype<BitCount> expected)
{
    adobe::fnvtype<BitCount> result(adobe::fnv1a<BitCount>(data));

    std::cout << "Testing fnv1a<" << BitCount << "> with { ";

    bool first(true);

    for (auto& datum : data)
    {
        std::cout << (!first ? ", " : "") << "0x" << std::hex << static_cast<int>(datum) << std::dec;

        first = false;
    }

    std::cout << " }\n";
    std::cout << std::hex;
    std::cout << "  Expected: 0x" << expected << '\n';
    std::cout << "       Got: 0x" << result << '\n';
    std::cout << std::dec;

    if (result == expected)
        return;

    throw std::runtime_error("Test failure.");
}

/******************************************************************************/

int main()
try
{
    {
        std::string data("Hello, world!");

        fnvtest<32>(data, 0xed90f094);
        fnvtest<64>(data, 0x38d1334144987bf4);
    }

    // Known case where FNV1a<32> hashes to zero and a 64 bit check
    // to make sure we are not returning zero for everything.
    {
        data_t data{ 0xcc, 0x24, 0x31, 0xc4 };

        fnvtest<32>(data, 0);
        fnvtest<64>(data, 0xd5e06ee11503d060);
    }

    // Known case where FNV1a<64> hashes to zero and a 32 bit check
    // to make sure we are not returning zero for everything.
    {
        data_t data{ 0xd5, 0x6b, 0xb9, 0x53, 0x42, 0x87, 0x08, 0x36 };

        fnvtest<32>(data, 0x4cc8df80);
        fnvtest<64>(data, 0);
    }

    // Known FNV1a<32> collision.
    {
        std::string        costarring("costarring");
        std::string        liquid("liquid");
        adobe::fnvtype<32> hash(0x5e4daa9d);

        fnvtest<32>(costarring, hash);
        fnvtest<32>(liquid, hash);
    }

#ifndef ADOBE_FNV_NO_BIGINTS
    {
        std::string lorem_ipsum("Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");

        std::cout << "Multiprecision support tests:\n"
                  << "  data: '" << lorem_ipsum << "'\n"
                  << std::hex
                  << "    32: " << adobe::fnv1a<32>(lorem_ipsum)   << '\n'
                  << "    64: " << adobe::fnv1a<64>(lorem_ipsum)   << '\n'
                  << "   128: " << adobe::fnv1a<128>(lorem_ipsum)  << '\n'
                  << "   256: " << adobe::fnv1a<256>(lorem_ipsum)  << '\n'
                  << "   512: " << adobe::fnv1a<512>(lorem_ipsum)  << '\n'
                  << "  1024: " << adobe::fnv1a<1024>(lorem_ipsum) << '\n'
                  << std::dec;
    }
#endif

    std::cout << "All tests passed.\n";

    return 0;
}
catch (const std::exception& error)
{
    std::cout << "Error: " << error.what() << '\n';

    return 1;
}
catch (...)
{
    std::cout << "Error: unknown\n";

    return 1;
}

/******************************************************************************/
