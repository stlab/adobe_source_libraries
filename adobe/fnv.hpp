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

#include <adobe/functional/operator.hpp>

/*************************************************************************************************/
/**
    \defgroup fnv FNV Hashing Algorithm

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

    You can also specify a result of any bitsize between 1 and 1024 (or 1 to 64
    with `ADOBE_FNV_NO_BIGINTS` defined), and the algorithm will produce a
    truncated result. When computing a hash that is a bitsize other than those
    explicitly defined, the recommendation from the above website is to use
    `xor_fold`:

    `((value >> (sizeof(T) * 8 - Bits)) ^ value) & ((1 << Bits) - 1)`

    However, `xor_fold` is not really necessary given the excellent distribution
    of the algorithm class, and a straightforward mask will result in bits that
    are as well distributed as result from `xor_fold`. So we mask.

    \warning
    This algorithm class is not (nor was ever intended to be) cryptographically
    secure. Do not use this algorithm where secure algorithms (e.g., \ref sha)
    are required.
*/
/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

namespace detail {

/*************************************************************************************************/

constexpr std::size_t rollup(std::size_t n) {
    return n == 0 ? 0 : n <= 32 ? 32 : n <= 64 ? 64 : n <= 128 ? 128 : n <= 256
                                                                           ? 256
                                                                           : n <= 512
                                                                                 ? 512
                                                                                 : n <= 1024 ? 1024
                                                                                             : 0;
}

/*************************************************************************************************/

template <std::size_t Bits>
struct fnv_base_traits {};

/*************************************************************************************************/

template <>
struct fnv_base_traits<32> {
    typedef std::uint32_t value_type;

    static constexpr std::size_t size() { return 32; }

    static constexpr value_type offset_basis() { return 0x811c9dc5; }

    static constexpr value_type prime() { return 0x1000193; }
};

/*************************************************************************************************/

template <>
struct fnv_base_traits<64> {
    typedef std::uint64_t value_type;

    static constexpr std::size_t size() { return 64; }

    static constexpr value_type offset_basis() { return 0xcbf29ce484222325; }

    static constexpr value_type prime() { return 0x100000001b3; }
};

/*************************************************************************************************/
#ifndef ADOBE_FNV_NO_BIGINTS
using namespace boost::multiprecision::literals;

/*************************************************************************************************/

template <>
struct fnv_base_traits<128> {
    typedef boost::multiprecision::uint128_t value_type;

    static constexpr std::size_t size() { return 128; }

    static constexpr value_type offset_basis() {
        return 0x6C62272E07BB014262B821756295C58D_cppui128;
    }

    static constexpr value_type prime() { return 0x1000000000000000000013B_cppui128; }
};

/*************************************************************************************************/

template <>
struct fnv_base_traits<256> {
    typedef boost::multiprecision::uint256_t value_type;

    static constexpr std::size_t size() { return 256; }

    static constexpr value_type offset_basis() {
        return 0xDD268DBCAAC550362D98C384C4E576CCC8B1536847B6BBB31023B4C8CAEE0535_cppui256;
    }

    static constexpr value_type prime() {
        return 0x1000000000000000000000000000000000000000163_cppui256;
    }
};

/*************************************************************************************************/

template <>
struct fnv_base_traits<512> {
    typedef boost::multiprecision::uint512_t value_type;

    static constexpr std::size_t size() { return 512; }

    static constexpr value_type offset_basis() {
        return 0xB86DB0B1171F4416DCA1E50F309990ACAC87D059C90000000000000000000D21E948F68A34C192F62EA79BC942DBE7CE182036415F56E34BAC982AAC4AFE9FD9_cppui512;
    }

    static constexpr value_type prime() {
        return 0x100000000000000000000000000000000000000000000000000000000000000000000000000000000000157_cppui512;
    }
};

/*************************************************************************************************/

template <>
struct fnv_base_traits<1024> {
    typedef boost::multiprecision::uint1024_t value_type;

    static constexpr std::size_t size() { return 1024; }

    static constexpr value_type offset_basis() {
        return 0x5F7A76758ECC4D32E56D5A591028B74B29FC4223FDADA16C3BF34EDA3674DA9A21D9000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004C6D7EB6E73802734510A555F256CC005AE556BDE8CC9C6A93B21AFF4B16C71EE90B3_cppui1024;
    }

    static constexpr value_type prime() {
        return 0x10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000018D_cppui1024;
    }
};
#endif
/*************************************************************************************************/

template <std::size_t FromBits, std::size_t ToBits>
struct bitmask {
    template <typename T>
    inline static T mask(T value) {
        return value & ((T(1) << ToBits) - 1);
    }
};

template <std::size_t SameBits>
struct bitmask<SameBits, SameBits> {
    template <typename T>
    inline static T mask(T value) {
        return value;
    }
};

/*************************************************************************************************/

} // namespace detail

/*************************************************************************************************/
/**
    \ingroup fnv

    Traits class used by adobe::fnv1a.

    \note
    In the case the implementation is not defined for the bit size passed, this will bump to the
    next highest implementation and mask the automatically. For example, fnv1a<56> will roll up
    to use fnv_traits<64> and mask the result back down to 56 bits before returning it.
*/
template <std::size_t Bits>
using fnv_traits = typename detail::fnv_base_traits<detail::rollup(Bits)>;

/**
    \ingroup fnv

    Integral type used to return the result of a call to adobe::fnv1a.

    The size of the type depends on the version of fnv called.
*/
template <std::size_t Bits>
using fnvtype = typename fnv_traits<Bits>::value_type;

/*************************************************************************************************/
/**
    \ingroup fnv

    Performs the FNV-1a hash over the advancing iterator until the (sentinel)
    predicate returns true.

    \todo Refactor this variant with the Iterator/Iterator one so there is one
    implementation of the algorithm in this header.
*/
template <std::size_t Bits, typename Iterator, typename Predicate>
fnvtype<Bits> fnv1a(Iterator first, Predicate p) {
    static_assert(sizeof(typename std::iterator_traits<Iterator>::value_type) == 1,
                  "Iterator value_type must be 1 byte.");

    typedef fnvtype<Bits> result_type;

    result_type result(fnv_traits<Bits>::offset_basis());

    while (!p(*first))
        result = (result xor static_cast<result_type>(*first++)) * fnv_traits<Bits>::prime();

    return detail::bitmask<fnv_traits<Bits>::size(), Bits>::template mask(result);
}

/*************************************************************************************************/
/**
    \ingroup fnv

    Performs the FNV-1a hash over the specified range.

    \todo Refactor this variant with the Iterator/Predicate one so there is one
    implementation of the algorithm in this header.
*/
template <std::size_t Bits, typename Iterator>
inline fnvtype<Bits> fnv1a(Iterator first, Iterator last) {
    static_assert(sizeof(typename std::iterator_traits<Iterator>::value_type) == 1,
                  "Iterator value_type must be 1 byte.");

    typedef fnvtype<Bits> result_type;

    result_type result(fnv_traits<Bits>::offset_basis());

    while (first != last)
        result = (result xor static_cast<result_type>(*first++)) * fnv_traits<Bits>::prime();

    return detail::bitmask<fnv_traits<Bits>::size(), Bits>::template mask(result);
}

/*************************************************************************************************/
/**
    \ingroup fnv

    Performs the FNV-1a hash over the specified container.
*/
template <std::size_t Bits, typename Container>
inline fnvtype<Bits> fnv1a(Container c) {
    return fnv1a<Bits>(begin(c), end(c));
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
