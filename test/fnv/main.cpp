/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

// stdc++
#include <vector>
#include <iostream>

#define BOOST_TEST_MAIN

// boost
#include <boost/test/unit_test.hpp>

// asl
#include <adobe/fnv.hpp>

/******************************************************************************/

typedef std::vector<std::uint8_t> data_t;

/******************************************************************************/

template <typename FNVType>
bool print_expected_result(const FNVType& expected, const FNVType& result)
{
    std::cout << std::hex;

    bool match(expected == result);

    BOOST_CHECK_EQUAL(expected, result);

    if (match)
        {
        std::cout << "  Test OK (0x" << std::hex << expected << std::dec << ")\n";
        }
    else
        {
        std::cout << std::hex;
        std::cout << "  Expected: 0x" << expected << '\n';
        std::cout << "       Got: 0x" << result << '\n';
        std::cout << std::dec;
        }

    return match;
}

/******************************************************************************/

template <typename FNVType>
void test_expected_result(const FNVType& expected, const FNVType& result)
{
    if (print_expected_result(expected, result))
        return;

    throw std::runtime_error("Test failure.");
}

/******************************************************************************/

template <std::size_t Bits, typename Container>
void fnvtest(const Container& data, const adobe::fnvtype<Bits>& expected)
{
    adobe::fnvtype<Bits> result(adobe::fnv1a<Bits>(data));

    std::cout << "Testing fnv1a<" << Bits << "> with { ";

    bool first(true);

    for (auto& datum : data)
    {
        std::cout << (!first ? ", " : "") << "0x" << std::hex << static_cast<int>(datum) << std::dec;

        first = false;
    }

    std::cout << " }\n";

    test_expected_result(expected, result);
}

/******************************************************************************/

template <std::size_t Bits>
void anybit_test(const std::string& data, const adobe::fnvtype<Bits>& expected)
{
    std::cout << "  fnv1a<" << Bits << ">(" << data << "): ";

    test_expected_result(expected, adobe::fnv1a<Bits>(data));
}

/******************************************************************************/
#ifndef ADOBE_FNV_NO_BIGINTS
void multiprecision_test(const std::string&          data,
                         const adobe::fnvtype<32>&   expected32,
                         const adobe::fnvtype<64>&   expected64,
                         const adobe::fnvtype<128>&  expected128,
                         const adobe::fnvtype<256>&  expected256,
                         const adobe::fnvtype<512>&  expected512,
                         const adobe::fnvtype<1024>& expected1024)
{
    std::cout << "Data: '" << data << "'\n";

    test_expected_result(expected32, adobe::fnv1a<32>(data));
    test_expected_result(expected64, adobe::fnv1a<64>(data));
    test_expected_result(expected128, adobe::fnv1a<128>(data));
    test_expected_result(expected256, adobe::fnv1a<256>(data));
    test_expected_result(expected512, adobe::fnv1a<512>(data));
    test_expected_result(expected1024, adobe::fnv1a<1024>(data));
}
#endif

/******************************************************************************/

BOOST_AUTO_TEST_CASE(fnv_smoke)
{
    {
        std::string data("Hello, world!");

        fnvtest<32>(data, 0xed90f094);
        fnvtest<64>(data, 0x38d1334144987bf4);
    }

    {
        std::string data("Hello, world");

        fnvtest<32>(data, 0x94d8f9bd);
        fnvtest<64>(data, 0xdd7b24779de0921d);
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

    // Variable-power hashing
    {
        std::string data("Hello, world!");

        std::cout << "Any bitsize tests:\n";

        anybit_test<1>(data, 0x0);
        anybit_test<2>(data, 0x0);
        anybit_test<3>(data, 0x4);
        anybit_test<4>(data, 0x4);
        anybit_test<5>(data, 0x14);
        anybit_test<6>(data, 0x14);
        anybit_test<7>(data, 0x14);
        anybit_test<8>(data, 0x94);
        anybit_test<9>(data, 0x94);
        anybit_test<10>(data, 0x94);
        anybit_test<11>(data, 0x94);
        anybit_test<12>(data, 0x94);
        anybit_test<13>(data, 0x1094);
        anybit_test<14>(data, 0x3094);
        anybit_test<15>(data, 0x7094);
        anybit_test<16>(data, 0xf094);
        anybit_test<17>(data, 0xf094);
        anybit_test<18>(data, 0xf094);
        anybit_test<19>(data, 0xf094);
        anybit_test<20>(data, 0xf094);
        anybit_test<21>(data, 0x10f094);
        anybit_test<22>(data, 0x10f094);
        anybit_test<23>(data, 0x10f094);
        anybit_test<24>(data, 0x90f094);
        anybit_test<25>(data, 0x190f094);
        anybit_test<26>(data, 0x190f094);
        anybit_test<27>(data, 0x590f094);
        anybit_test<28>(data, 0xd90f094);
        anybit_test<29>(data, 0xd90f094);
        anybit_test<30>(data, 0x2d90f094);
        anybit_test<31>(data, 0x6d90f094);
        anybit_test<32>(data, 0xed90f094);
    }

#ifndef ADOBE_FNV_NO_BIGINTS
    {
        std::string lorem_ipsum("Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");

        using namespace boost::multiprecision::literals;

        multiprecision_test(lorem_ipsum,
                            0xb7c3ecf4,
                            0x72133ab33927ecf4,
                            0xEB7EAD9EC700E913540ACEA679A27A84_cppui128,
                            0x6D997241EA842EC01283322953A22260993F51FB1E630357867B55247114F214_cppui256,
                            0xE8E3DA6524D4BAE0A9B746BBFB0B516027C5B0B3196A5961EC83989B8C7FCAB133806377A38D28592AA1CD14DA63A2254920C0B8AC0C0CA9D25B290A7A7071C4_cppui512,
                            0xF6EBCA1943D3EB1C35C6924A431B455AD358D674A2571470A98EBA4CDDF85A6E6D8FBE6160B10D89E2B27C675A88A760419A364C30E39F4C2772D0BE14C7D65F6A2E7C0BC7445238D71A378BC1A7A8ED9356107EA52C6877DCB60BBBB0DAAA9386586EC801C3976E07CB8C9F2CE581C0093FF955609735D1BFA695FE307C4208_cppui1024);
    }

    {
        std::string hello_world("Hello, world!");

        using namespace boost::multiprecision::literals;

        multiprecision_test(hello_world,
                            0xed90f094,
                            0x38d1334144987bf4,
                            0xE40C10E67A044A1309ECED64DE4517EC_cppui128,
                            0x14FE3B5900FCEAB1A07C5BAB79DCCC19151D21940CB097A4B3A84F935C12FE44_cppui256,
                            0x444D84D3FD55F5FEEDD095032BEE94618C826168766ACA6C4E59A04F405690CE1FC3D4B0BECAC88C1A91B556BAF626F4F2A7B96764372282953FA07FCBE08240_cppui512,
                            0x4B09771CE1B9B55BD6BBEE8F1627F263EE4E34EE145945CE4063E90BAF408FF181E577CB8ABBB9D68DB95F00000000000000000000000000000000000000000000000000000000000000000000000005992342635328451A9F29E0998A6B548F75911852E3829CFF1313A138C944EE69819D8FB9EA6033182AD1215DCC8E9A6E_cppui1024);
    }
#endif
}

/******************************************************************************/
