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

template <std::size_t BitCount>
void fnvtest(const data_t& data, adobe::fnvtype<BitCount> expected)
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
        data_t data{ 'H', 'e', 'l', 'l', 'o', ',', ' ', 'w', 'o', 'r', 'l', 'd', '!' };

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
        data_t             costarring{ 'c', 'o', 's', 't', 'a', 'r', 'r', 'i', 'n', 'g' };
        data_t             liquid{ 'l', 'i', 'q', 'u', 'i', 'd' };
        adobe::fnvtype<32> hash(0x5e4daa9d);

        fnvtest<32>(costarring, hash);
        fnvtest<32>(liquid, hash);
    }

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
