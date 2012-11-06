/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_SHA_HPP
#define ADOBE_SHA_HPP

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <boost/static_assert.hpp>

#include <stdexcept>
#include <limits>
#include <cstring>

/*************************************************************************************************/

namespace adobe {

/*!
\class adobe::sha1_t
\ingroup secure_algorithm

\brief A bit-oriented implementation of the SHA-1 Secure Hash Algorithm
*/

/*!
\class adobe::sha224_t
\ingroup secure_algorithm

\brief A bit-oriented implementation of the SHA-224 Secure Hash Algorithm
*/

/*!
\class adobe::sha256_t
\ingroup secure_algorithm

\brief A bit-oriented implementation of the SHA-256 Secure Hash Algorithm
*/

/*!
\class adobe::sha384_t
\ingroup secure_algorithm

\brief A bit-oriented implementation of the SHA-384 Secure Hash Algorithm
*/

/*!
\class adobe::sha512_t
\ingroup secure_algorithm

\brief A bit-oriented implementation of the SHA-512 Secure Hash Algorithm
*/

/*!
\typedef adobe::sha1_t::digest_type

A <code>boost::array<boost::uint32_t, 5></code> for the 160-bit digest
*/

/*!
\fn adobe::sha1_t::digest_type adobe::sha1_t::digest(I first, I last)

\pre
    <code>sizeof(std::iterator_traits<I>::value_type)</code> must be 1

\param first    first iterator over the range to hash
\param last     last iterator over the range to hash

\return The SHA-1 digest of the message
*/

/*!
\fn adobe::sha1_t::digest_type adobe::sha1_t::digest(I first, boost::uint64_t num_bits)

\pre
    <code>sizeof(std::iterator_traits<I>::value_type)</code> must be 1

\param first    first iterator over the range to hash
\param num_bits number of bits to hash

\return The SHA-1 digest of the message
*/

/*!
\typedef adobe::sha224_t::digest_type

A <code>boost::array<boost::uint32_t, 7></code> for the 224-bit digest
*/

/*!
\fn adobe::sha224_t::digest_type adobe::sha224_t::digest(I first, I last)

\pre
    <code>sizeof(std::iterator_traits<I>::value_type)</code> must be 1

\param first    first iterator over the range to hash
\param last     last iterator over the range to hash

\return The SHA-224 digest of the message
*/

/*!
\fn adobe::sha224_t::digest_type adobe::sha224_t::digest(I first, boost::uint64_t num_bits)

\pre
    <code>sizeof(std::iterator_traits<I>::value_type)</code> must be 1

\param first    first iterator over the range to hash
\param num_bits number of bits to hash

\return The SHA-224 digest of the message
*/

/*!
\typedef adobe::sha256_t::digest_type

A <code>boost::array<boost::uint32_t, 8></code> for the 256-bit digest
*/

/*!
\fn adobe::sha256_t::digest_type adobe::sha256_t::digest(I first, I last)

\pre
    <code>sizeof(std::iterator_traits<I>::value_type)</code> must be 1

\param first    first iterator over the range to hash
\param last     last iterator over the range to hash

\return The SHA-256 digest of the message
*/

/*!
\fn adobe::sha256_t::digest_type adobe::sha256_t::digest(I first, boost::uint64_t num_bits)

\pre
    <code>sizeof(std::iterator_traits<I>::value_type)</code> must be 1

\param first    first iterator over the range to hash
\param num_bits number of bits to hash

\return The SHA-256 digest of the message
*/

/*!
\typedef adobe::sha384_t::digest_type

A <code>boost::array<boost::uint64_t, 6></code> for the 384-bit digest
*/

/*!
\fn adobe::sha384_t::digest_type adobe::sha384_t::digest(I first, I last)

\pre
    <code>sizeof(std::iterator_traits<I>::value_type)</code> must be 1

\param first    first iterator over the range to hash
\param last     last iterator over the range to hash

\return The SHA-384 digest of the message

\note While the SHA standard specifies the ability to process messages of 2^128 in length, currently this SHA-384 implementation only handles messages up to 2^64 in length.
*/

/*!
\fn adobe::sha384_t::digest_type adobe::sha384_t::digest(I first, boost::uint64_t num_bits)

\pre
    <code>sizeof(std::iterator_traits<I>::value_type)</code> must be 1

\param first    first iterator over the range to hash
\param num_bits number of bits to hash

\return The SHA-384 digest of the message

\note While the SHA standard specifies the ability to process messages of 2^128 in length, currently this SHA-384 implementation only handles messages up to 2^64 in length.
*/

/*!
\typedef adobe::sha512_t::digest_type

A <code>boost::array<boost::uint64_t, 8></code> for the 512-bit digest
*/

/*!
\fn adobe::sha512_t::digest_type adobe::sha512_t::digest(I first, I last)

\pre
    <code>sizeof(std::iterator_traits<I>::value_type)</code> must be 1

\param first    first iterator over the range to hash
\param last     last iterator over the range to hash

\return The SHA-512 digest of the message

\note While the SHA standard specifies the ability to process messages of 2^128 in length, currently this SHA-512 implementation only handles messages up to 2^64 in length.
*/

/*!
\fn adobe::sha512_t::digest_type adobe::sha512_t::digest(I first, boost::uint64_t num_bits)

\pre
    <code>sizeof(std::iterator_traits<I>::value_type)</code> must be 1

\param first    first iterator over the range to hash
\param num_bits number of bits to hash

\return The SHA-512 digest of the message

\note While the SHA standard specifies the ability to process messages of 2^128 in length, currently this SHA-512 implementation only handles messages up to 2^64 in length.
*/


/*************************************************************************************************/

class sha1_t;
class sha224_t;
class sha256_t;
class sha384_t;
class sha512_t;

/*************************************************************************************************/

namespace implementation {

/*************************************************************************************************/

#if !defined(ADOBE_NO_DOCUMENTATION)

/*************************************************************************************************/

template <typename HashT>
struct sha_traits;

/*************************************************************************************************/

template <typename I> // I models InputIterator
inline boost::uint64_t long_distance(I first, I last)
{
    boost::uint64_t result(0);

    while (first != last)
    {
        ++result;

        ++first;
    }

    return result;
}

/*************************************************************************************************/

template <typename I> // I models InputIterator
struct bit_packer
{
    typedef typename std::iterator_traits<I>::value_type value_type;

    BOOST_STATIC_ASSERT((sizeof(value_type) == 1));

    bit_packer(I first, I last) :
        first_m(first), bitsize_m(long_distance(first, last))
    { }

    bit_packer(I first, boost::uint64_t bitsize) :
        first_m(first), bitsize_m(bitsize)
    { }

    template <typename T>
    inline std::size_t operator () (T& result)
    { return byte_pack(result); }

private:
    template <typename T>
    std::size_t byte_pack(T& result)
    {
        std::size_t to_pack(sizeof(T));

        result = 0;

        while (to_pack != 0 && bitsize_m != 0)
        {
            std::size_t shift_count(8 * (to_pack - 1));
            T           mask(T(255) << shift_count);

            result |= (T(*first_m) << shift_count) & mask;

            if (bitsize_m >= 8)
            {
                bitsize_m -= 8;
            }
            else
            {
                std::size_t remaining(static_cast<std::size_t>(to_pack * 8 - bitsize_m));

                bitsize_m = 0;

                return remaining;
            }

            ++first_m;
            --to_pack;
        }

        return to_pack * 8;
    }

    I               first_m;
    boost::uint64_t bitsize_m;
};

/*************************************************************************************************/

template <typename T>
struct bitsizeof
{ enum { value = sizeof(T) * 8 }; };

/*************************************************************************************************/

template <std::size_t N, typename T>
inline T shr(const T& x)
{
    BOOST_STATIC_ASSERT((N < bitsizeof<T>::value));

    return x >> N;
}

template <std::size_t N, typename T>
inline T rotr(const T& x)
{
    BOOST_STATIC_ASSERT((N < bitsizeof<T>::value));

    enum { l_shift = bitsizeof<T>::value - N };

    return (x >> N) | (x << l_shift);
}

template <std::size_t N, typename T>
inline T rotl(const T& x)
{
    BOOST_STATIC_ASSERT((N < bitsizeof<T>::value));

    enum { r_shift = bitsizeof<T>::value - N };

    return (x << N) | (x >> r_shift);
}

/*************************************************************************************************/

template <typename T>
inline T ch(T x, T y, T z)
    { return (x & y) ^ (~x & z); }

template <typename T>
inline T parity(T x, T y, T z)
    { return x ^ y ^ z; }

template <typename T>
inline T maj(T x, T y, T z)
    { return (x & y) ^ (x & z) ^ (y & z); }

/*************************************************************************************************/

template <bool UseMB14, typename HashTraits>
struct message_block_part_14_set_t
{
    typedef HashTraits                                  traits_type;
    typedef typename traits_type::hash_type             hash_type;
    typedef typename traits_type::message_block_type    message_block_type;
    typedef typename message_block_type::value_type     message_block_value_type;

    enum { half_max_message_bitsize_k = traits_type::max_message_bitsize_k / 2 };

    void operator () (message_block_value_type& mbp14, boost::uint64_t num_bits)
    {
        message_block_value_type message_block_value_type_max(std::numeric_limits<message_block_value_type>::max());

        mbp14 = static_cast<message_block_value_type>((num_bits >> (half_max_message_bitsize_k)) & message_block_value_type_max);
    }
};

/*************************************************************************************************/

template <typename HashTraits>
struct message_block_part_14_set_t<false, HashTraits>
{
    typedef HashTraits                                  traits_type;
    typedef typename traits_type::message_block_type    message_block_type;
    typedef typename message_block_type::value_type     message_block_value_type;

    void operator () (message_block_value_type& mbp14, boost::uint64_t)
    { mbp14 = 0; }
};

/*************************************************************************************************/

template <typename HashTraits, typename I>
void block_and_digest(typename HashTraits::digest_type& digest, I first, boost::uint64_t num_bits)
{
    typedef HashTraits                                  traits_type;
    typedef typename traits_type::hash_type             hash_type;
    typedef typename traits_type::message_block_type    message_block_type;
    typedef typename message_block_type::value_type     message_block_value_type;

    enum
    {
        max_message_bitsize_k = traits_type::max_message_bitsize_k,
        half_max_message_bitsize_k = max_message_bitsize_k / 2,
        message_blocksize_k = traits_type::message_blocksize_k,
        use_mb_14 = half_max_message_bitsize_k < bitsizeof<boost::uint64_t>::value
    };

    message_block_value_type    message_block_value_type_max(std::numeric_limits<message_block_value_type>::max());
    message_block_type          message_block;
    boost::uint64_t             message_size(num_bits + max_message_bitsize_k);
    boost::uint64_t             num_blocks(message_size / message_blocksize_k + 1);
    bool                        in_padding(false);
    bit_packer<I>               bits(first, num_bits);

    while (num_blocks != 0)
    {
        for (std::size_t i(0); i < 16; ++i)
        {
            if (!in_padding)
            {
                std::size_t unset_bits(bits(message_block[i]));

                if (unset_bits != 0)
                {
                    message_block[i] |= message_block_value_type(1) << (unset_bits - 1);

                    in_padding = true;
                }
            }
            else
            {
                if (num_blocks == 1)
                {
                    // REVISIT (fbrereto) : According to the SHA standard the message length in the
                    //                      1024-block-size case can be up to 2^128 bits long,
                    //                      but we only support messages up to 2^64 in length. In
                    //                      all instances when padding in the generic case, block
                    //                      part 14 would be:
                    //                          mbp14 = (num_bits >> (half_max_message_bitsize_k)) &
                    //                              message_block_value_type_max
                    //                      But in the 1024-block-size case
                    //                      half_max_message_bitsize_k == num_bits, and we will get
                    //                      a compiler error basically saying "hey, you're
                    //                      overshifting this value to 0", which would be fine in
                    //                      this case because the number should be set to 0, but
                    //                      the compiler is still (rightfully) noisy about it. This
                    //                      workaround forces the right thing to do in that it sets
                    //                      message block part 14 to zero in this special
                    //                      1024-block-size case, thus sliencing the compiler.

                    if (i == 14)
                        message_block_part_14_set_t<use_mb_14, traits_type>()(message_block[i], num_bits);
                    else if (i == 15)
                        message_block[i] = static_cast<message_block_value_type>(num_bits & message_block_value_type_max);
                    else
                        message_block[i] = 0;
                }
                else
                    message_block[i] = 0;
            }
        }

        sha_traits<hash_type>().digest_message_block(digest, message_block);

        --num_blocks;
    }

    // clears potentioally sensitive information
    std::memset(&message_block, 0, sizeof(message_block));
}

/*************************************************************************************************/

template <typename HashTraits>
void sha_2_digest_message_block(typename HashTraits::digest_type&               digest,
                                const typename HashTraits::message_block_type&  message_block)
{
    //  The "sha_2" in the name of this function is in
    //  reference to the second generation of SHA algorithms
    //  (224, 256, 384, and 512), all of which have the same
    //  message block process implementation.

    typedef HashTraits                                  traits_type;
    typedef typename traits_type::hash_type             hash_type;
    typedef typename traits_type::message_block_type    message_block_type;
    typedef typename traits_type::schedule_type         schedule_type;
    typedef typename hash_type::digest_type::value_type digest_value_type;

    schedule_type schedule;

    adobe::copy(message_block, &schedule[0]);

    for (std::size_t t(message_block_type::static_size); t < schedule_type::static_size; ++t)
        schedule[t] =   traits_type::small_sigma_1(schedule[t - 2]) + schedule[t - 7] +
                        traits_type::small_sigma_0(schedule[t - 15]) + schedule[t - 16];

    digest_value_type a(digest[0]);
    digest_value_type b(digest[1]);
    digest_value_type c(digest[2]);
    digest_value_type d(digest[3]);
    digest_value_type e(digest[4]);
    digest_value_type f(digest[5]);
    digest_value_type g(digest[6]);
    digest_value_type h(digest[7]);

    for (std::size_t t(0); t < schedule.size(); ++t)
    {
        digest_value_type T1 =  h                           +
                                traits_type::big_sigma_1(e) +
                                implementation::ch(e, f, g) +
                                traits_type::k(t)           +
                                schedule[t];
        digest_value_type T2 =  traits_type::big_sigma_0(a) +
                                implementation::maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }

    digest[0] += a;
    digest[1] += b;
    digest[2] += c;
    digest[3] += d;
    digest[4] += e;
    digest[5] += f;
    digest[6] += g;
    digest[7] += h;

    // clears potentioally sensitive information
    std::memset(&schedule, 0, sizeof(schedule));
}

/*************************************************************************************************/

template <>
struct sha_traits<sha1_t>
{
    typedef class sha1_t                 hash_type;
    typedef boost::array<boost::uint32_t, 5>    digest_type;
    typedef boost::array<boost::uint32_t, 16>   message_block_type;
    typedef boost::array<boost::uint32_t, 80>   schedule_type;

    enum
    {
        max_message_bitsize_k = 64,
        message_blocksize_k = 512
    };

    static void reset_digest(digest_type& digest)
    {
        digest[0] = 0x67452301;
        digest[1] = 0xefcdab89;
        digest[2] = 0x98badcfe;
        digest[3] = 0x10325476;
        digest[4] = 0xc3d2e1f0;
    }

    static void digest_message_block(digest_type& digest, const message_block_type& message_block)
    {   
        schedule_type schedule;

        adobe::copy(message_block, &schedule[0]);

        for (std::size_t t(message_block_type::static_size); t < schedule_type::static_size; ++t)
            schedule[t] = implementation::rotl<1>(  schedule[t - 3] ^ schedule[t - 8] ^
                                                    schedule[t - 14] ^ schedule[t - 16]);

        boost::uint32_t a(digest[0]);
        boost::uint32_t b(digest[1]);
        boost::uint32_t c(digest[2]);
        boost::uint32_t d(digest[3]);
        boost::uint32_t e(digest[4]);

        for (std::size_t t(0); t < schedule.size(); ++t)
        {
            boost::uint32_t T = implementation::rotl<5>(a)  +
                                f(t, b, c, d)               +
                                e                           +
                                k(t)                        +
                                schedule[t];
            e = d;
            d = c;
            c = implementation::rotl<30>(b);
            b = a;
            a = T;
        }

        digest[0] += a;
        digest[1] += b;
        digest[2] += c;
        digest[3] += d;
        digest[4] += e;

        // clears potentioally sensitive information
        std::memset(&schedule, 0, sizeof(schedule));
    }

private:
    static boost::uint32_t f(   std::size_t     t,
                                boost::uint32_t x,
                                boost::uint32_t y,
                                boost::uint32_t z)
    {
        assert (t < 80);
    
        if (t <= 19)        return implementation::ch(x, y, z);
        else if (t <= 39)   return implementation::parity(x, y, z);
        else if (t <= 59)   return implementation::maj(x, y, z);
    
        return implementation::parity(x, y, z);
    }

    static boost::uint32_t k(std::size_t t)
    {
        assert(t < 80);
    
        if (t <= 19)        return 0x5a827999;
        else if (t <= 39)   return 0x6ed9eba1;
        else if (t <= 59)   return 0x8f1bbcdc;
    
        return 0xca62c1d6;
    }
};

/*************************************************************************************************/

template <>
struct sha_traits<sha256_t>
{
    typedef class sha256_t               hash_type;
    typedef boost::array<boost::uint32_t, 8>    digest_type;
    typedef boost::array<boost::uint32_t, 16>   message_block_type;
    typedef boost::array<boost::uint32_t, 64>   schedule_type;

    enum
    {
        max_message_bitsize_k = 64,
        message_blocksize_k = 512
    };

    static void reset_digest(digest_type& digest)
    {
        digest[0] = 0x6a09e667;
        digest[1] = 0xbb67ae85;
        digest[2] = 0x3c6ef372;
        digest[3] = 0xa54ff53a;
        digest[4] = 0x510e527f;
        digest[5] = 0x9b05688c;
        digest[6] = 0x1f83d9ab;
        digest[7] = 0x5be0cd19;
    }

    static void digest_message_block(digest_type& digest, const message_block_type& message_block)
        { sha_2_digest_message_block<sha_traits<hash_type> >(digest, message_block); }

    static boost::uint32_t big_sigma_0(boost::uint32_t x)
        { return implementation::rotr<2>(x) ^ implementation::rotr<13>(x) ^ implementation::rotr<22>(x); }

    static boost::uint32_t big_sigma_1(boost::uint32_t x)
        { return implementation::rotr<6>(x) ^ implementation::rotr<11>(x) ^ implementation::rotr<25>(x); }

    static boost::uint32_t small_sigma_0(boost::uint32_t x)
        { return implementation::rotr<7>(x) ^ implementation::rotr<18>(x) ^ implementation::shr<3>(x); }

    static boost::uint32_t small_sigma_1(boost::uint32_t x)
        { return implementation::rotr<17>(x) ^ implementation::rotr<19>(x) ^ implementation::shr<10>(x); }

    static boost::uint32_t k(std::size_t t)
    {
        static const boost::uint32_t k_set[] =
        {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
            0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
            0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
        };

        assert (t < 64);

        return k_set[t];
    }
};

/*************************************************************************************************/

template <>
struct sha_traits<sha224_t>
{
    typedef class sha224_t                           hash_type;
    typedef sha_traits<sha256_t>::digest_type        digest_type;
    typedef sha_traits<sha256_t>::message_block_type message_block_type;
    typedef sha_traits<sha256_t>::schedule_type      schedule_type;

    enum
    {
        max_message_bitsize_k = sha_traits<sha256_t>::max_message_bitsize_k,
        message_blocksize_k = sha_traits<sha256_t>::message_blocksize_k
    };

    static void reset_digest(digest_type& digest)
    {
        digest[0] = 0xc1059ed8;
        digest[1] = 0x367cd507;
        digest[2] = 0x3070dd17;
        digest[3] = 0xf70e5939;
        digest[4] = 0xffc00b31;
        digest[5] = 0x68581511;
        digest[6] = 0x64f98fa7;
        digest[7] = 0xbefa4fa4;
    }

    static void digest_message_block(digest_type& digest, const message_block_type& message_block)
        { sha_2_digest_message_block<sha_traits<hash_type> >(digest, message_block); }

    static boost::uint32_t big_sigma_0(boost::uint32_t x)
        { return sha_traits<sha256_t>::big_sigma_0(x); }

    static boost::uint32_t big_sigma_1(boost::uint32_t x)
        { return sha_traits<sha256_t>::big_sigma_1(x); }

    static boost::uint32_t small_sigma_0(boost::uint32_t x)
        { return sha_traits<sha256_t>::small_sigma_0(x); }

    static boost::uint32_t small_sigma_1(boost::uint32_t x)
        { return sha_traits<sha256_t>::small_sigma_1(x); }

    static boost::uint32_t k(std::size_t x)
        { return sha_traits<sha256_t>::k(x); }
};

/*************************************************************************************************/

template <>
struct sha_traits<sha512_t>
{
    typedef class sha512_t               hash_type;
    typedef boost::array<boost::uint64_t, 8>    digest_type;
    typedef boost::array<boost::uint64_t, 16>   message_block_type;
    typedef boost::array<boost::uint64_t, 80>   schedule_type;

    enum
    {
        max_message_bitsize_k = 128,
        message_blocksize_k = 1024
    };

    static void reset_digest(digest_type& digest)
    {
        digest[0] = 0x6a09e667f3bcc908ULL;
        digest[1] = 0xbb67ae8584caa73bULL;
        digest[2] = 0x3c6ef372fe94f82bULL;
        digest[3] = 0xa54ff53a5f1d36f1ULL;
        digest[4] = 0x510e527fade682d1ULL;
        digest[5] = 0x9b05688c2b3e6c1fULL;
        digest[6] = 0x1f83d9abfb41bd6bULL;
        digest[7] = 0x5be0cd19137e2179ULL;
    }

    static void digest_message_block(digest_type& digest, const message_block_type& message_block)
        { sha_2_digest_message_block<sha_traits<hash_type> >(digest, message_block); }

    static boost::uint64_t big_sigma_0(boost::uint64_t x)
        { return implementation::rotr<28>(x) ^ implementation::rotr<34>(x) ^ implementation::rotr<39>(x); }

    static boost::uint64_t big_sigma_1(boost::uint64_t x)
        { return implementation::rotr<14>(x) ^ implementation::rotr<18>(x) ^ implementation::rotr<41>(x); }

    static boost::uint64_t small_sigma_0(boost::uint64_t x)
        { return implementation::rotr<1>(x) ^ implementation::rotr<8>(x) ^ implementation::shr<7>(x); }

    static boost::uint64_t small_sigma_1(boost::uint64_t x)
        { return implementation::rotr<19>(x) ^ implementation::rotr<61>(x) ^ implementation::shr<6>(x); }

    static boost::uint64_t k(std::size_t t)
    {
        static const boost::uint64_t k_set[] =
        {
            0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
            0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
            0xd807aa98a3030242ULL, 0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
            0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
            0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
            0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
            0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
            0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL, 0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
            0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
            0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
            0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
            0xd192e819d6ef5218ULL, 0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
            0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
            0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
            0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
            0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
            0xca273eceea26619cULL, 0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
            0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
            0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
            0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
        };

        assert (t < 80);

        return k_set[t];
    }
};

/*************************************************************************************************/

template <>
struct sha_traits<sha384_t>
{
    typedef class sha384_t                           hash_type;
    typedef sha_traits<sha512_t>::digest_type        digest_type;
    typedef sha_traits<sha512_t>::message_block_type message_block_type;
    typedef sha_traits<sha512_t>::schedule_type      schedule_type;

    enum
    {
        max_message_bitsize_k = sha_traits<sha512_t>::max_message_bitsize_k,
        message_blocksize_k = sha_traits<sha512_t>::message_blocksize_k
    };

    static void reset_digest(digest_type& digest)
    {
        digest[0] = 0xcbbb9d5dc1059ed8ULL;
        digest[1] = 0x629a292a367cd507ULL;
        digest[2] = 0x9159015a3070dd17ULL;
        digest[3] = 0x152fecd8f70e5939ULL;
        digest[4] = 0x67332667ffc00b31ULL;
        digest[5] = 0x8eb44a8768581511ULL;
        digest[6] = 0xdb0c2e0d64f98fa7ULL;
        digest[7] = 0x47b5481dbefa4fa4ULL;
    }

    static void digest_message_block(digest_type& digest, const message_block_type& message_block)
        { sha_2_digest_message_block<sha_traits<hash_type> >(digest, message_block); }

    static boost::uint64_t big_sigma_0(boost::uint64_t x)
        { return sha_traits<sha512_t>::big_sigma_0(x); }

    static boost::uint64_t big_sigma_1(boost::uint64_t x)
        { return sha_traits<sha512_t>::big_sigma_1(x); }

    static boost::uint64_t small_sigma_0(boost::uint64_t x)
        { return sha_traits<sha512_t>::small_sigma_0(x); }

    static boost::uint64_t small_sigma_1(boost::uint64_t x)
        { return sha_traits<sha512_t>::small_sigma_1(x); }

    static boost::uint64_t k(std::size_t x)
        { return sha_traits<sha512_t>::k(x); }
};

/*************************************************************************************************/

#endif

/*************************************************************************************************/

} // namespace implementation

/*************************************************************************************************/

class sha1_t
{
public:
#if !defined(ADOBE_NO_DOCUMENTATION)
    typedef implementation::sha_traits<sha1_t>   traits_type;
#endif
    typedef traits_type::digest_type                    digest_type;

    template <typename I>
    inline digest_type digest(I first, I last)
    {
        enum { ibits_k = implementation::bitsizeof<typename std::iterator_traits<I>::value_type>::value };

        return digest(first, implementation::long_distance(first, last) * ibits_k);
    }

    template <typename I>
    inline digest_type digest(I first, boost::uint64_t num_bits)
    {
        traits_type().reset_digest(digest_m);

        implementation::block_and_digest<traits_type>(digest_m, first, num_bits);

        return digest_m;
    }

private:
    digest_type digest_m;
};

/*************************************************************************************************/

class sha224_t
{
public:
#if !defined(ADOBE_NO_DOCUMENTATION)
    typedef implementation::sha_traits<sha224_t> traits_type;
#endif
    typedef boost::array<boost::uint32_t, 7>            digest_type;

    template <typename I>
    inline digest_type digest(I first, I last)
    {
        enum { ibits_k = implementation::bitsizeof<typename std::iterator_traits<I>::value_type>::value };

        return digest(first, implementation::long_distance(first, last) * ibits_k);
    }

    template <typename I>
    inline digest_type digest(I first, boost::uint64_t num_bits)
    {
        traits_type().reset_digest(digest_m);

        implementation::block_and_digest<traits_type>(digest_m, first, num_bits);

        return crop_digest(digest_m);
    }

private:
    inline digest_type crop_digest(traits_type::digest_type& digest)
    {
        digest_type result;

        std::copy(digest.begin(), digest.begin() + digest_type::static_size, &result[0]);

        return result;
    }

    traits_type::digest_type digest_m;
};

/*************************************************************************************************/

class sha256_t
{
public:
#if !defined(ADOBE_NO_DOCUMENTATION)
    typedef implementation::sha_traits<sha256_t> traits_type;
#endif
    typedef traits_type::digest_type                    digest_type;

    template <typename I>
    inline digest_type digest(I first, I last)
    {
        enum { ibits_k = implementation::bitsizeof<typename std::iterator_traits<I>::value_type>::value };

        return digest(first, implementation::long_distance(first, last) * ibits_k);
    }

    template <typename I>
    inline digest_type digest(I first, boost::uint64_t num_bits)
    {
        traits_type().reset_digest(digest_m);

        implementation::block_and_digest<traits_type>(digest_m, first, num_bits);

        return digest_m;
    }

private:
    digest_type digest_m;
};

/*************************************************************************************************/

class sha512_t
{
public:
#if !defined(ADOBE_NO_DOCUMENTATION)
    typedef implementation::sha_traits<sha512_t> traits_type;
#endif
    typedef traits_type::digest_type                    digest_type;

    template <typename I>
    inline digest_type digest(I first, I last)
    {
        enum { ibits_k = implementation::bitsizeof<typename std::iterator_traits<I>::value_type>::value };

        return digest(first, implementation::long_distance(first, last) * ibits_k);
    }

    template <typename I>
    inline digest_type digest(I first, boost::uint64_t num_bits)
    {
        traits_type().reset_digest(digest_m);

        implementation::block_and_digest<traits_type>(digest_m, first, num_bits);

        return digest_m;
    }

private:
    digest_type digest_m;
};

/*************************************************************************************************/

class sha384_t
{
public:
#if !defined(ADOBE_NO_DOCUMENTATION)
    typedef implementation::sha_traits<sha384_t> traits_type;
#endif
    typedef boost::array<boost::uint64_t, 6>            digest_type;

    template <typename I>
    inline digest_type digest(I first, I last)
    {
        enum { ibits_k = implementation::bitsizeof<typename std::iterator_traits<I>::value_type>::value };

        return digest(first, implementation::long_distance(first, last) * ibits_k);
    }

    template <typename I>
    inline digest_type digest(I first, boost::uint64_t num_bits)
    {
        traits_type().reset_digest(digest_m);

        implementation::block_and_digest<traits_type>(digest_m, first, num_bits);

        return crop_digest(digest_m);
    }

private:
    inline digest_type crop_digest(traits_type::digest_type& digest)
    {
        digest_type result;

        std::copy(digest.begin(), digest.begin() + digest_type::static_size, &result[0]);

        return result;
    }

    traits_type::digest_type digest_m;
};

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
