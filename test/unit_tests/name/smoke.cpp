/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

// config
#include <adobe/config.hpp>

// stdc++
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <vector>

#define BOOST_TEST_MAIN

// boost
#include <boost/test/unit_test.hpp>

// *clears throat* Excuse me:
//
//     AAAAAAAHHHHHHH!!!!
//
// I swore to myself I would never do this, however this is the quickest and
// DIRTIEST way of gaining access to th internals of both static_name_t and
// name_t so we can verify they are behaving as we expect. This should NEVER
// be in ANY other code than the unit testing.
//
#define private public

// asl
#include <adobe/name.hpp>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

inline std::ostream& operator"" _dump (const char* str, std::size_t n)
{
    return std::cout << "dump: {\n"
                     << "   str: '" << str << "'\n"
                     << "     n: " << n << '\n'
                     << "  hash: 0x" << std::hex << adobe::detail::name_hash(str, n) << std::dec << '\n'
                     << "};\n";
}

template <std::size_t N>
void dumpy(const char (&str)[N])
{
    static_assert(sizeof(std::size_t) == 8, "std::size_t size mismatch.");

    std::cout << &str << '\n'
              << N << '\n'
              << std::hex << adobe::detail::name_hash(str, N-1) << std::dec << '\n'
              ;
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

BOOST_AUTO_TEST_CASE(name_smoke)
{
    using namespace adobe::literals;

    constexpr std::size_t hello_world_hash = 0x38d1334144987bf4;
    constexpr std::size_t myhash = adobe::detail::name_hash("Hello, world!");

    BOOST_CHECK_EQUAL(myhash, hello_world_hash);

    "Hello, world!"_dump;
    "Red Sox"_dump;
    "Cardinals"_dump;
    ""_dump;

    adobe::static_name_t static_hello_world("Hello, world!"_name);
    adobe::static_name_t static_red_sox("Red Sox"_name);
    adobe::static_name_t static_null(""_name);

    adobe::name_t red_sox("Red Sox");
    adobe::name_t cardinals("Cardinals");
    adobe::name_t nullname(""_name);

    BOOST_CHECK_EQUAL(static_hello_world.hash_m, hello_world_hash);
    BOOST_CHECK_EQUAL(static_red_sox.hash_m,     0xc5746070bacfea32);
    BOOST_CHECK_EQUAL(static_null.hash_m,        0xcbf29ce484222325);

    BOOST_CHECK_EQUAL(std::hash<adobe::name_t>()(static_red_sox),
                      std::hash<adobe::name_t>()(red_sox));

    BOOST_CHECK_PREDICATE(std::equal_to<adobe::name_t>(), (static_red_sox) (red_sox));

    BOOST_CHECK_PREDICATE(std::not_equal_to<adobe::name_t>(), (static_red_sox) (cardinals));

    BOOST_CHECK_PREDICATE(std::not_equal_to<adobe::name_t>(), (red_sox) (cardinals));

    BOOST_CHECK_PREDICATE(std::less<adobe::name_t>(), (cardinals) (static_red_sox));

    BOOST_CHECK_PREDICATE(std::less<adobe::name_t>(), (cardinals) (red_sox));

    BOOST_CHECK(static_cast<bool>(static_red_sox));

    BOOST_CHECK(static_cast<bool>(red_sox));

    BOOST_CHECK(!static_cast<bool>(static_null));

    BOOST_CHECK(!static_cast<bool>(nullname));

    // Intentionally out of lexicographical order.
    std::vector<adobe::name_t> name_set =
    {
        "fig"_name,
        "apple"_name,
        "banana"_name,
        "eggplant"_name,
        "grape"_name,
        "horseradish"_name,
        "daikon"_name,
        "carrot"_name
    };

    BOOST_CHECK(!is_sorted(begin(name_set), end(name_set)));

    std::sort(begin(name_set), end(name_set));

    BOOST_CHECK(is_sorted(begin(name_set), end(name_set)));

    BOOST_CHECK(!is_sorted(begin(name_set), end(name_set), adobe::name_t::fast_sort));

    std::sort(begin(name_set), end(name_set), adobe::name_t::fast_sort);

    BOOST_CHECK(is_sorted(begin(name_set), end(name_set), adobe::name_t::fast_sort));

    dumpy("Hello, world!");
}

/****************************************************************************************************/
