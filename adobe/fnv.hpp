/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_FNV_HPP
#define ADOBE_FNV_HPP

/*************************************************************************************************/

#include <cstddef>

#ifndef ADOBE_FNV_NO_BIGINTS
#include <boost/multiprecision/cpp_int.hpp>
#endif

/*************************************************************************************************/
/**
    Implementation of the FNV (Fowler Noll Vo) class of hash algorithms. From
    the homepage (http://www.isthe.com/chongo/tech/comp/fnv/):

    <blockquote>
    The FNV speed allows one to quickly hash lots of data while maintaining a
    reasonable collision rate. The high dispersion of the FNV hashes makes them
    well suited for hashing nearly identical strings such as URLs, hostnames,
    filenames, text, IP addresses, etc.
    </blockquote>

    There are two implementations of FNV: FNV-1 and FNV-1a. The latter has a
    minor change over the former which results in better avalanche
    characteristics, reducing collisions at no cost to performance. As such
    FNV-1a is implemented here; FNV-1 is not.

    According to the author (on the same website above) these algorithms are
    in the public domain:

    <blockquote>
    FNV hash algorithms and source code have been released into the public
    domain. The authors of the FNV algorithmm [sic] look deliberate steps to
    disclose the algorhtm [sic] in a public forum soon after it was invented.
    More than a year passed after this public disclosure and the authors
    deliberatly [sic] took no steps to patent the FNV algorithm. Therefore it is
    safe to say that the FNV authors have no patent claims on the FNV algorithm
    as published.
    </blockquote>

    All versions of the algorithm are available (32, 64, 128, 256, 512, and 1024)
    however only the 32- and 64-bit versions have no external dependencies.
    Beyond 64-bits the implementations leverage the Boost multiprecision
    library. If you would prefer to remove the dependency (and implicitly limit
    yourself to the 32- and 64-bit algorithms), define the
    `ADOBE_FNV_NO_BIGINTS` macro.
*/
/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

namespace detail {

/*************************************************************************************************/

template <std::size_t BitCount>
struct fnv_traits
{ };

/*************************************************************************************************/

template <>
struct fnv_traits<32>
{
    typedef std::uint32_t value_type;

    static constexpr value_type offset_basis()
    {
        return 0x811c9dc5;
    }

    static constexpr value_type prime()
    {
        return 0x1000193;
    }
};

/*************************************************************************************************/

template <>
struct fnv_traits<64>
{
    typedef std::uint64_t value_type;

    static constexpr value_type offset_basis()
    {
        return 0xcbf29ce484222325;
    }

    static constexpr value_type prime()
    {
        return 0x100000001b3;
    }
};

/*************************************************************************************************/
#ifndef ADOBE_FNV_NO_BIGINTS
using namespace boost::multiprecision::literals;

/*************************************************************************************************/

template <>
struct fnv_traits<128>
{
    typedef boost::multiprecision::uint128_t value_type;

    static constexpr value_type offset_basis()
    {
        return 0x6C62272E07BB014262B821756295C58D_cppui128;
    }

    static constexpr value_type prime()
    {
        return 0x1000000000000000000013B_cppui128;
    }
};

/*************************************************************************************************/

template <>
struct fnv_traits<256>
{
    typedef boost::multiprecision::uint256_t value_type;

    static constexpr value_type offset_basis()
    {
        return 0xDD268DBCAAC550362D98C384C4E576CCC8B1536847B6BBB31023B4C8CAEE0535_cppui256;
    }

    static constexpr value_type prime()
    {
        return 0x1000000000000000000000000000000000000000163_cppui256;
    }
};

/*************************************************************************************************/

template <>
struct fnv_traits<512>
{
    typedef boost::multiprecision::uint512_t value_type;

    static constexpr value_type offset_basis()
    {
        return 0xB86DB0B1171F4416DCA1E50F309990ACAC87D059C90000000000000000000D21E948F68A34C192F62EA79BC942DBE7CE182036415F56E34BAC982AAC4AFE9FD9_cppui512;
    }

    static constexpr value_type prime()
    {
        return 0x100000000000000000000000000000000000000000000000000000000000000000000000000000000000157_cppui512;
    }
};

/*************************************************************************************************/

template <>
struct fnv_traits<1024>
{
    typedef boost::multiprecision::uint1024_t value_type;

    static constexpr value_type offset_basis()
    {
        return 0x5F7A76758ECC4D32E56D5A591028B74B29FC4223FDADA16C3BF34EDA3674DA9A21D9000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004C6D7EB6E73802734510A555F256CC005AE556BDE8CC9C6A93B21AFF4B16C71EE90B3_cppui1024;
    }

    static constexpr value_type prime()
    {
        return 0x10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000018D_cppui1024;
    }
};
#endif
/*************************************************************************************************/

} // namespace detail

/*************************************************************************************************/
/**
    Integral type used to return the result of a call to adobe::fnv1a. The size
    of the type depends on the version of fnv called.
*/
template <std::size_t BitCount>
using fnvtype = typename detail::fnv_traits<BitCount>::value_type;

/*************************************************************************************************/
/**
    Performs the FNV-1a hash over the specified range.
*/
template <std::size_t BitCount, typename Iterator>
fnvtype<BitCount> fnv1a(Iterator first, Iterator last)
{
    static_assert(sizeof (typename std::iterator_traits<Iterator>::value_type) == 1,
                  "Iterator value_type must be 1 byte.");

    typedef fnvtype<BitCount> result_type;

    result_type result(detail::fnv_traits<BitCount>::offset_basis());

    while (first != last)
        result = (result xor static_cast<result_type>(*first++)) *
                     detail::fnv_traits<BitCount>::prime();

    return result;
}

/*************************************************************************************************/
/**
    Performs the FNV-1a hash over the specified container.
*/
template <std::size_t BitCount, typename Container>
inline fnvtype<BitCount> fnv1a(Container c)
{
    return fnv1a<BitCount>(begin(c), end(c));
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
