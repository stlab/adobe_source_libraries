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

    Currently only the 32- and 64-bit versions of the algorithm are available.
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
struct fnv_traits<64>
{
    typedef std::uint64_t                                          value_type;
    typedef std::integral_constant<value_type, 0xcbf29ce484222325> offset_basis_k;
    typedef std::integral_constant<value_type, 0x100000001b3>      prime_k;
};

/*************************************************************************************************/

template <>
struct fnv_traits<32>
{
    typedef std::uint32_t                                  value_type;
    typedef std::integral_constant<value_type, 0x811c9dc5> offset_basis_k;
    typedef std::integral_constant<value_type, 0x1000193>  prime_k;
};

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

    result_type result(detail::fnv_traits<BitCount>::offset_basis_k::value);

    while (first != last)
        result = (result xor static_cast<result_type>(*first++)) *
                     detail::fnv_traits<BitCount>::prime_k::value;

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
