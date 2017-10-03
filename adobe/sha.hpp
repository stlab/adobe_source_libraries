/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_SHA_HPP
#define ADOBE_SHA_HPP

/**************************************************************************************************/

#include <array>
#include <cassert>
#include <cstring>
#include <cstdint>
#include <iterator>
#include <string>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

namespace implementation {

/**************************************************************************************************/

#if !defined(ADOBE_NO_DOCUMENTATION)

/**************************************************************************************************/

template <typename T>
constexpr std::size_t bitsizeof() {
    return sizeof(T) * 8;
}

/**************************************************************************************************/

template <std::size_t N, typename T>
inline T shr(T x) {
    static_assert(N < bitsizeof<T>(), "shr size mismatch.");

    return x >> N;
}

template <std::size_t N, typename T>
inline T rotr(T x) {
    static_assert(N < bitsizeof<T>(), "rotr size mismatch.");

    constexpr std::size_t l_shift = bitsizeof<T>() - N;

    return (x >> N) | (x << l_shift);
}

template <std::size_t N, typename T>
inline T rotl(T x) {
    static_assert(N < bitsizeof<T>(), "rotl size mismatch.");

    constexpr std::size_t r_shift = bitsizeof<T>() - N;

    return (x << N) | (x >> r_shift);
}

/**************************************************************************************************/

template <typename T>
inline T ch(T x, T y, T z) {
    return (x & y) ^ (~x & z);
}

template <typename T>
inline T parity(T x, T y, T z) {
    return x ^ y ^ z;
}

template <typename T>
inline T maj(T x, T y, T z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

/*
A source of bytes. The input is a range defined by two iterators. The length
of the range does not need to be known up front. This will work with
input iterators.
*/
template <typename I>
class byte_source_iterators
{
public:
    byte_source_iterators( I const& first, I const& last )
    : first_( first )
    , last_( last ) {
        static_assert(sizeof(typename std::iterator_traits<I>::value_type) == sizeof(std::uint8_t), "Iterator must supply bytes.");
    }

    // Note that this returns the number of *bits* available. Even though this
    // class can only returns complete bytes we need consistency with
    // byte_source_iterator_n.
    // The number available is not necessarily the total number, it is just
    // the number we can get hold of right now.
    std::size_t bits_available() const {
        return ( first_ == last_ ) ? 0 : 8;
    }

    std::uint8_t operator *() const {
        return *first_;
    }

    // Prefix
    byte_source_iterators& operator ++() {
        ++first_;
        return *this;
    }

    // Postfix operator ++ does not work with input iterators so we
    // do not provide it

    // Making copies does not work if we are using input iterators so forbid
    // copy construction and assigment
    byte_source_iterators( byte_source_iterators const& ) = delete;
    byte_source_iterators& operator =( byte_source_iterators const& ) = delete;

private:

    I first_;
    I const last_;
};

/*
A source of bytes. The input is an iterator and the number of *bits* the
source can provide. Unlike byte_source_iterators we know the length
up front
*/
template <typename I>
class byte_source_iterator_n
{
public:
    byte_source_iterator_n( I const& first, std::size_t num_bits )
    : first_( first )
    , num_bits_( num_bits ) {
        static_assert(sizeof(typename std::iterator_traits<I>::value_type) == sizeof(std::uint8_t), "Iterator must supply bytes.");
    }

    // The last byte might not be complete
    std::size_t bits_available() const {
        return ( num_bits_ > 8 ) ? 8 : num_bits_;
    }

    std::uint8_t operator *() const {
        return *first_;
    }

    // Prefix
    byte_source_iterator_n& operator ++() {
        ++first_;
        num_bits_ -= bits_available();
        return *this;
    }

    // Postfix operator ++ does not work with input iterators so we
    // do not provide it

    // Making copies does not work if we are using input iterators so forbid
    // copy construction and assigment
    byte_source_iterator_n( byte_source_iterator_n const& ) = delete;
    byte_source_iterator_n& operator =( byte_source_iterator_n const& ) = delete;

private:

    I first_;
    std::size_t num_bits_;
};

/**************************************************************************************************/

template <typename HashTraits, typename ByteSource>
std::uint64_t stuff_into_state(typename HashTraits::message_block_type& state,
                               std::uint16_t& stuff_bit_offset, std::size_t num_bits,
                               ByteSource& byte_source) {

    typedef HashTraits traits_type;
    typedef typename traits_type::message_block_type::value_type value_type;

    constexpr std::size_t value_size_k = sizeof(value_type);
    constexpr std::size_t value_bitsize_k = value_size_k * 8;

    /*
    We are given the stuff bit offset in terms of the whole message block. From there we need to
    derive which element of the array we are going to stuff into, and the shift within that element.
    Then we stuff bytes from *first, decreasing the internal shift until we hit zero (stuffing from
    most significant bit to least significant). Then we advance to the next element, reset the shift
    to far-right, and keep going.

    For values that come in less than a byte the high bits are where the value is stored, not the
    low bits.
    */

    std::size_t element = stuff_bit_offset / value_bitsize_k;
    std::size_t shift = value_bitsize_k - 8 -               // most sig bit, less bitsizeof(*first)
                        stuff_bit_offset % value_bitsize_k; // bits already stuffed.
    value_type* dst(&state[element]);

    /*
    As soon as we've gone once through this loop, we'll be on a byte boundary. If we don't go
    through the loop we'll be off a byte boundary and will have to shift the next byte appropriately
    (it should shift 0 when we've been through this loop, effectively a nop.)
    */

    std::uint64_t n_stuffed_bits = 0;

    while (num_bits >= 8 && byte_source.bits_available() >= 8) {
        *dst |= (value_type(*byte_source) & value_type(0xff)) << shift;

        ++byte_source;

        if (shift == 0) {
            ++dst;

            shift = value_bitsize_k - 8;
        } else {
            shift -= 8;
        }

        stuff_bit_offset += 8;
        n_stuffed_bits += 8;
        num_bits -= 8;
    }

    // sub-byte leftovers which need to be shifted into place.
    if (num_bits && byte_source.bits_available()) {
        num_bits = std::min(num_bits, byte_source.bits_available());
        /*
        dst is a window of size value_bitsize_k. The message ends somewhere in the window
        [ value_bitsize_k .. 1 ], meaning our leftovers get their high order bit pushed to position
        [ value_bitsize_k - 1 .. 0 ]. Since we know the value is in the low-8-bits of a
        value_bitsize_k-sized window, we need to shift the value either up or down, depending on
        where the message ended.

        message_end is the bit location of the last bit of the message. This can be value_bitsize_k,
        implying the message ended on the value_type boundary.
        */
        std::size_t message_end(value_bitsize_k - (stuff_bit_offset % value_bitsize_k));
        value_type v(*byte_source & 0xff); // our value in the low 8 bits.

        ++byte_source;

        if (message_end < 8) // shift down
            v >>= 8 - message_end;
        else if (message_end > 8) // shift up
            v <<= message_end - 8;

        *dst |= v;
		// static_cast to silence Visual Studio 2017:
		// warning C4267: '+=': conversion from 'size_t' to 'uint16_t', possible loss of data
        stuff_bit_offset += static_cast<std::uint16_t>(num_bits);
        n_stuffed_bits += num_bits;
    }
    return n_stuffed_bits;
}

/**************************************************************************************************/
/*
We keep a running message block as state, along with info about the total message length to this
point, as well as the number of bits stuffed in the current message block. Whenever the user wants
to update the message they pass in some data, which we use to stuff the current message block. When
that block fills to capacity we digest it and reset the state to prepare to digest the next message
block. In the end there is work to be done with whatever data might be left over. In finalize, then,
we add the spec'd 1-bit, zero-pad the message if need be, append the length of the message, and
digest that final block. The user gets whatever comes out.

This routine expects whole-byte input until the end of the message, at which point the remainder of
the message can be a subset of 8 bytes. If you're going to be busting up a messages into chunks to
hash it, then you need to do so at the byte boundary until the last chunk.
*/
template <typename HashTraits, typename ByteSource>
void block_and_digest(typename HashTraits::message_block_type& state, std::uint16_t& stuffed_size,
                      std::uint64_t& message_size, typename HashTraits::state_digest_type& digest,
                      ByteSource& byte_source ) {
    typedef HashTraits traits_type;

    // The size of the message block in bits. Either 512 or 1024.
    constexpr std::size_t message_blocksize_k = traits_type::message_blocksize_k;

    /*
    The SHA description has three phases for preparing to digest a message:
        - padding
        - parsing (aka blocking)
        - hash initialization

    All messages are composed of the following:
        - the message (length: num_bits bits)
        - a 1-bit (length: 1 bit)
        - zero padding (varies)
        - the number of bits of the message (max_message_bitsize_k bits)

    This is the padding and blocking phase. Pack the message into 512- or 1024-bit blocks for
    digest, padding the message out with a varible number of zero bits such that the last message
    block is also 512- or 1024-bits long. All blocks will be 100% message except for the last block
    in the digest That last block, then, will be:
        - zero-or-more bits of message
        - the 1 bit
        - zero-or-more bits of zero-bit padding
        - the length of the message (max_message_bitsize_k bits)
    */
    std::size_t bits_available(message_blocksize_k - stuffed_size);

    while (byte_source.bits_available() > 0) {
        std::uint64_t const bits_stuffed = stuff_into_state<traits_type>(state, stuffed_size, bits_available, byte_source);

        // If we have a full message block then digest it
        if( bits_stuffed == bits_available ) {
            traits_type::digest_message_block(digest, state, stuffed_size);
        }

        message_size += bits_stuffed;
        bits_available = message_blocksize_k;
    }
}

/**************************************************************************************************/

template <bool overshift, typename message_block_type>
struct shift_down
{
    void operator()(message_block_type& state,
                    std::uint64_t message_size,
                    typename message_block_type::value_type value_type_mask) const
    {
    }
};

template <typename message_block_type>
struct shift_down<false, message_block_type>
{
    void operator()(message_block_type& state,
                    std::uint64_t message_size,
                    typename message_block_type::value_type value_type_mask) const
    {
        constexpr std::size_t value_bitsize_k = sizeof(typename message_block_type::value_type) * 8;

        if (message_size > value_type_mask)
        {
            state[14] = (message_size >> value_bitsize_k) & value_type_mask;
        }
    }
};

/**************************************************************************************************/

template <typename HashTraits>
typename HashTraits::digest_type finalize(typename HashTraits::message_block_type& state,
                                          std::uint16_t stuffed_size, std::uint64_t message_size,
                                          typename HashTraits::state_digest_type& digest) {
    /*
    A couple things we have to do in this routine:
        - add a single 1-bit to the end of the message
        - pad out the final block to message_blocksize_k bits, less
          the size of the message
        - add the size of the message to the end.

    There are concessions we make for when these three elements cause us to cross over the block
    boundary, requiring us to digest more than one block as we finalize.
    */

    typedef HashTraits traits_type;
    typedef typename traits_type::message_block_type message_block_type;
    typedef typename message_block_type::value_type value_type;

    // Maximum length of message in bits (2^n); this is n. Either 64 or 128.
    constexpr std::size_t max_message_bitsize_k = traits_type::max_message_bitsize_k;
    // The size of the message block in bits. Either 512 or 1024.
    constexpr std::size_t message_blocksize_k = traits_type::message_blocksize_k;

    constexpr std::size_t value_size_k = sizeof(value_type);
    constexpr std::size_t value_bitsize_k = value_size_k * 8;

    /*
    Stuff into state needs to operate from the assumption that it is taking the most significant
    bits of a value and stuffing them into the message. As such our 1-bit here needs to be in the
    MSB, hence 0x80 and not 1.
    */
    std::uint8_t one_bit(0x80);
    std::uint8_t* one_bit_ptr(&one_bit);

    byte_source_iterator_n< std::uint8_t* > bit_source( one_bit_ptr, 1 );
    stuff_into_state<traits_type>(state, stuffed_size, 1, bit_source);

    /*
    If that one bit pushed us to the message block boundary, congratulations. Digest the message and
    reset the machine. Do NOT update the message length - the end bit isn't counted as part of the
    message.
    */
    if (stuffed_size == message_blocksize_k)
        traits_type::digest_message_block(digest, state, stuffed_size);

    /*
    Now that we have the last block with (maybe) enough space, find the end and insert the length of
    the message. Fortunately we have a routine for that.

    Note that if there is not enough space to insert the length of the message (i.e., offset will be
    less than stuffed_size) we need to digest the block and try again with an empty one.
    */
    constexpr std::size_t length_offset(message_blocksize_k - max_message_bitsize_k);

    /*
    The length of the message will always go into message block elements 14 and/or 15. (i.e., the
    last two.) State block 15 gets the first half of the size, and state block 14 gets the second
    half.

    In the event we dont have enough room at the end of the message block to add the length of the
    message, digest the block as-is (that is, with some amount of zero-padding at the end), then
    clear it and process another block whose contents are just the length of the message.
    */
    if (stuffed_size > length_offset)
        traits_type::digest_message_block(digest, state, stuffed_size);

    constexpr value_type value_type_mask = static_cast<value_type>(-1);

    /*
    If the message length is longer than what will fit in the last element of the message block,
    slice the high bits of the length off and stuff them into the next-to-last element. Note that
    because of the checks above, we are guaranteed the space will be zero-padded, so we can write
    over it OK.

    The 15th block requires a shift down of the message size, which in the SHA-512 case is a 64-bit
    shift of a 64-bit value, which is undefined according to the C++ standard. (This step is were we
    to store message_size as a 128-bit value, as is allowed by the SHA standard.) As such we derive
    the overshift boolean and give the compiler the freedom to elide the overshift-and-assignment at
    compile-time, eliminating the undefined behavior.
    */
    constexpr bool overshift = value_bitsize_k >= (sizeof(message_size) * 8);
    shift_down<overshift, message_block_type>()(state, message_size, value_type_mask);

    state[15] = message_size & value_type_mask;

    // Digest the final block of this message.
    traits_type::digest_message_block(digest, state, stuffed_size);

    /*
    Once the finalized block(s) have been digested, copy the result to the digest type and get outta
    here. Note that this could crop the number of bits between the state digest and the resulting
    digest, depending on the requirements of the hash algorithm being used.
    */
    typedef typename HashTraits::digest_type digest_type;

    digest_type result = {{0}};

    std::memcpy(&result[0], &digest[0], sizeof(digest_type));

    return result;
}

/**************************************************************************************************/

template <typename HashTraits>
void sha_2_digest_message_block(typename HashTraits::state_digest_type& digest,
                                typename HashTraits::message_block_type& message_block,
                                std::uint16_t& stuffed_size) {
    /*
    The "sha_2" in the name of this function is in reference to the second generation of SHA
    algorithms (224, 256, 384, and 512), all of which have the same message block process
    implementation.
    */

    typedef HashTraits traits_type;
    typedef typename traits_type::schedule_type schedule_type;
    typedef typename traits_type::state_digest_type::value_type digest_value_type;

    schedule_type schedule;

    std::copy(message_block.begin(), message_block.end(), &schedule[0]);

    for (std::size_t t(message_block.size()); t < schedule.size(); ++t)
        schedule[t] = traits_type::small_sigma_1(schedule[t - 2]) + schedule[t - 7] +
                      traits_type::small_sigma_0(schedule[t - 15]) + schedule[t - 16];

    digest_value_type a(digest[0]);
    digest_value_type b(digest[1]);
    digest_value_type c(digest[2]);
    digest_value_type d(digest[3]);
    digest_value_type e(digest[4]);
    digest_value_type f(digest[5]);
    digest_value_type g(digest[6]);
    digest_value_type h(digest[7]);

    for (std::size_t t(0); t < schedule.size(); ++t) {
        digest_value_type T1 = h + traits_type::big_sigma_1(e) + implementation::ch(e, f, g) +
                               traits_type::k(t) + schedule[t];
        digest_value_type T2 = traits_type::big_sigma_0(a) + implementation::maj(a, b, c);
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

    // clears potentially sensitive information
    schedule = {{0}};

    // reset the state machine to digest the upcoming block
    message_block = {{0}};
    stuffed_size = 0;
}

/**************************************************************************************************/

struct sha1_traits_t {
    typedef std::array<std::uint32_t, 5> state_digest_type;
    typedef std::array<std::uint32_t, 16> message_block_type;
    typedef std::array<std::uint32_t, 80> schedule_type;
    typedef state_digest_type digest_type;

    static constexpr std::size_t max_message_bitsize_k = 64;
    static constexpr std::size_t message_blocksize_k = 512;

    static constexpr state_digest_type initial_state() {
        return {{0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0}};
    }

    static inline void digest_message_block(state_digest_type& digest,
                                            message_block_type& message_block,
                                            std::uint16_t& stuffed_size) {
        schedule_type schedule;
		// static_cast schedule.size() to silence Visual Studio 2017:
		// warning C4267: 'initializing': conversion from 'size_t' to 'const uint_fast8_t', possible loss of data
        constexpr std::uint_fast8_t schedule_size = static_cast<std::uint_fast8_t>(schedule.size());

        std::memcpy(&schedule[0], &message_block[0], sizeof(message_block_type));

        for (std::size_t t(message_block.size()); t < schedule_size; ++t)
            schedule[t] = implementation::rotl<1>(schedule[t - 3] ^ schedule[t - 8] ^
                                                  schedule[t - 14] ^ schedule[t - 16]);

        std::uint32_t a(digest[0]);
        std::uint32_t b(digest[1]);
        std::uint32_t c(digest[2]);
        std::uint32_t d(digest[3]);
        std::uint32_t e(digest[4]);
        std::uint32_t T(0);

// Manually unrolling the loop in this fasion
// improves the digest speed by about 20%.

#define A_ROUND(t, F, k)                                                                           \
    T = implementation::rotl<5>(a) + F(b, c, d) + e + std::uint32_t(k) + schedule[t];              \
    e = d;                                                                                         \
    d = c;                                                                                         \
    c = implementation::rotl<30>(b);                                                               \
    b = a;                                                                                         \
    a = T

#define FIVE_ROUNDS(t, F, k)                                                                       \
    A_ROUND((t + 0), F, k);                                                                        \
    A_ROUND((t + 1), F, k);                                                                        \
    A_ROUND((t + 2), F, k);                                                                        \
    A_ROUND((t + 3), F, k);                                                                        \
    A_ROUND((t + 4), F, k)

#define TWENTY_ROUNDS(t, F, k)                                                                     \
    FIVE_ROUNDS((t + 0), F, k);                                                                    \
    FIVE_ROUNDS((t + 5), F, k);                                                                    \
    FIVE_ROUNDS((t + 10), F, k);                                                                   \
    FIVE_ROUNDS((t + 15), F, k)

        TWENTY_ROUNDS(0, implementation::ch, 0x5a827999);
        TWENTY_ROUNDS(20, implementation::parity, 0x6ed9eba1);
        TWENTY_ROUNDS(40, implementation::maj, 0x8f1bbcdc);
        TWENTY_ROUNDS(60, implementation::parity, 0xca62c1d6);

        digest[0] += a;
        digest[1] += b;
        digest[2] += c;
        digest[3] += d;
        digest[4] += e;

        // clears potentially sensitive information
        schedule = {{0}};

        // reset the state machine to digest the upcoming block
        message_block = {{0}};
        stuffed_size = 0;
    }
};

/**************************************************************************************************/

struct sha256_traits_t {
    typedef std::array<std::uint32_t, 8> state_digest_type;
    typedef std::array<std::uint32_t, 16> message_block_type;
    typedef std::array<std::uint32_t, 64> schedule_type;
    typedef state_digest_type digest_type;

    static constexpr std::size_t max_message_bitsize_k = 64;
    static constexpr std::size_t message_blocksize_k = 512;

    static constexpr state_digest_type initial_state() {
        return {{0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19}};
    }

    static inline void digest_message_block(state_digest_type& digest,
                                            message_block_type& message_block,
                                            std::uint16_t& stuffed_size) {
        sha_2_digest_message_block<sha256_traits_t>(digest, message_block, stuffed_size);
    }

    static inline std::uint32_t big_sigma_0(std::uint32_t x) {
        return implementation::rotr<2>(x) ^ implementation::rotr<13>(x) ^
               implementation::rotr<22>(x);
    }

    static inline std::uint32_t big_sigma_1(std::uint32_t x) {
        return implementation::rotr<6>(x) ^ implementation::rotr<11>(x) ^
               implementation::rotr<25>(x);
    }

    static inline std::uint32_t small_sigma_0(std::uint32_t x) {
        return implementation::rotr<7>(x) ^ implementation::rotr<18>(x) ^ implementation::shr<3>(x);
    }

    static inline std::uint32_t small_sigma_1(std::uint32_t x) {
        return implementation::rotr<17>(x) ^ implementation::rotr<19>(x) ^
               implementation::shr<10>(x);
    }

    static inline std::uint32_t k(std::size_t t) {
        static const std::uint32_t k_set[] = {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4,
            0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe,
            0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f,
            0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
            0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
            0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
            0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116,
            0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7,
            0xc67178f2};

        assert(t < 64);

        return k_set[t];
    }
};

/**************************************************************************************************/

struct sha224_traits_t : public sha256_traits_t {
    typedef std::array<std::uint32_t, 7> digest_type;

    static constexpr state_digest_type initial_state() {
        return {{0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
                 0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4}};
    }
};

/**************************************************************************************************/

struct sha512_traits_t {
    typedef std::array<std::uint64_t, 8> state_digest_type;
    typedef std::array<std::uint64_t, 16> message_block_type;
    typedef std::array<std::uint64_t, 80> schedule_type;
    typedef state_digest_type digest_type;

    static constexpr std::size_t max_message_bitsize_k = 128;
    static constexpr std::size_t message_blocksize_k = 1024;

    static constexpr state_digest_type initial_state() {
        return {{0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL,
                 0xa54ff53a5f1d36f1ULL, 0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL,
                 0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL}};
    }

    static inline void digest_message_block(state_digest_type& digest,
                                            message_block_type& message_block,
                                            std::uint16_t& stuffed_size) {
        sha_2_digest_message_block<sha512_traits_t>(digest, message_block, stuffed_size);
    }

    static inline std::uint64_t big_sigma_0(std::uint64_t x) {
        return implementation::rotr<28>(x) ^ implementation::rotr<34>(x) ^
               implementation::rotr<39>(x);
    }

    static inline std::uint64_t big_sigma_1(std::uint64_t x) {
        return implementation::rotr<14>(x) ^ implementation::rotr<18>(x) ^
               implementation::rotr<41>(x);
    }

    static inline std::uint64_t small_sigma_0(std::uint64_t x) {
        return implementation::rotr<1>(x) ^ implementation::rotr<8>(x) ^ implementation::shr<7>(x);
    }

    static inline std::uint64_t small_sigma_1(std::uint64_t x) {
        return implementation::rotr<19>(x) ^ implementation::rotr<61>(x) ^
               implementation::shr<6>(x);
    }

    static inline std::uint64_t k(std::size_t t) {
        static const std::uint64_t k_set[] = {
            0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL,
            0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
            0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL,
            0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
            0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
            0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
            0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL, 0x2de92c6f592b0275ULL,
            0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
            0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL,
            0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
            0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL,
            0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
            0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL,
            0x92722c851482353bULL, 0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
            0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
            0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
            0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL,
            0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
            0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL,
            0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
            0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL,
            0xc67178f2e372532bULL, 0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
            0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL,
            0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
            0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
            0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
            0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL};

        assert(t < 80);

        return k_set[t];
    }
};

/**************************************************************************************************/

struct sha384_traits_t : public sha512_traits_t {
    typedef std::array<std::uint64_t, 6> digest_type;

    static constexpr state_digest_type initial_state() {
        return {{0xcbbb9d5dc1059ed8ULL, 0x629a292a367cd507ULL, 0x9159015a3070dd17ULL,
                 0x152fecd8f70e5939ULL, 0x67332667ffc00b31ULL, 0x8eb44a8768581511ULL,
                 0xdb0c2e0d64f98fa7ULL, 0x47b5481dbefa4fa4ULL}};
    }
};

/**************************************************************************************************/

#endif

/**************************************************************************************************/

} // namespace implementation

/**************************************************************************************************/
/**
\defgroup sha SHA (Secure Hash Algorithm)
*/
/**
\ingroup sha

Generic engine implementation for SHA-*.

Given the similarity of behavior between the SHA-* class of algorithms,
the core functionality has been encapsulated in this class.

Users of the class can either call one of the one-shot sha::digest
routines or instantiate the class and call sha::update repeatedly,
then retrieve the digest with sha::finalize.
*/
template <class Traits>
class sha {
public:
#if !defined(ADOBE_NO_DOCUMENTATION)
    typedef Traits traits_type;
#endif

    /**
    \ingroup sha

    A statically-sized, contiguous array for the resulting SHA-* digest.

    \note the size of this digest will change depending on the SHA
          routine in use.
    */
    typedef typename traits_type::digest_type digest_type;

    /**
    \ingroup sha

    Sets the state of the digest machine to its default.
    */
    sha()
        : message_size_m(0), state_m({{0}}), state_digest_m(traits_type::initial_state()),
          stuffed_size_m(0) {}

    /**
    \ingroup sha

    This routine can be called successively to digest a data over one or more
    steps.

    \pre
        <code>sizeof(std::iterator_traits<I>::value_type)</code> must be 1

    \param first first iterator over the range to digest
    \param last  last iterator over the range to digest

    \note While the SHA standard specifies the ability to process messages up to
          2^128 bits, this routine is limited to
          `sizeof(std::iterator_traits<I>::difference_type) * 8` bits. A
          workaround to the limitation is to call this routine multiple times.
          Even then, the total message length is limited to 2^64 bits.
    */
    template <typename I>
    inline void update(I first, I last) {
        // We can only update if the current state is byte aligned
        assert(stuffed_size_m % 8 == 0);

        implementation::byte_source_iterators< I > byte_source(first, last);
        implementation::block_and_digest<traits_type>(state_m, stuffed_size_m, message_size_m,
                                                      state_digest_m, byte_source);
    }

    /**
    \ingroup sha

    This routine can be called successively to digest a data over one or more
    steps.

    \pre
        <code>sizeof(std::iterator_traits<I>::value_type)</code> must be 1

    \param first first iterator over the range to digest
    \param num_bits number of bits to digest

    \note While the SHA standard specifies the ability to process messages up to
          2^128 bits, this routine is limited to messages of 2^64 bits in length.

    \note If update is called multiple times all but the last call must have
          num_bits % 8 == 0
    */
    template <typename I>
    inline void update(I first, std::uint64_t num_bits) {
        // We can only update if the current state is byte aligned
        assert(stuffed_size_m % 8 == 0);

        implementation::byte_source_iterator_n< I > byte_source(first, num_bits);
        implementation::block_and_digest<traits_type>(state_m, stuffed_size_m, message_size_m,
                                                      state_digest_m, byte_source);
    }

    /**
    \ingroup sha

    \return The SHA-* digest of the message
    */
    inline digest_type finalize() {
        std::uint64_t message_size(message_size_m);
        typename traits_type::message_block_type state(state_m);
        typename traits_type::state_digest_type state_digest(state_digest_m);
        std::uint16_t stuffed_size(stuffed_size_m);

        return implementation::finalize<traits_type>(state, stuffed_size, message_size,
                                                     state_digest);
    }

    /**
    \ingroup sha

    \pre
        <code>sizeof(std::iterator_traits<I>::value_type)</code> must be 1

    \param first first iterator over the range to digest
    \param last  last iterator over the range to digest

    \note While the SHA standard specifies the ability to process messages up to
          2^128 bits, this routine is limited to
          `sizeof(std::iterator_traits<I>::difference_type) * 8` bits.

    \return The SHA-* digest of the message
    */
    template <typename I>
    static inline digest_type digest(I first, I last) {
        sha instance;

        instance.update(first, last);

        return instance.finalize();
    }

    /**
    \ingroup sha

    \pre
        <code>sizeof(std::iterator_traits<I>::value_type)</code> must be 1

    \param first    first iterator over the range to digest
    \param num_bits number of bits to digest

    \note While the SHA standard specifies the ability to process messages up to
          2^128 bits, this routine is limited to messages of 2^64 bits in length.

    \return The SHA-* digest of the message
    */
    template <typename I>
    static inline digest_type digest(I first, std::uint64_t num_bits) {
        sha instance;

        instance.update(first, num_bits);

        return instance.finalize();
    }

    /**
    Returns the finalized digest as an ASCII string.

    There is also an
    optional parameter to add spaces between elements of the digest, defaulting
    to false.
    */
    static std::string to_string(const typename traits_type::digest_type& digest,
                                 bool spaces = false) {
        typedef typename traits_type::digest_type::value_type value_type;

        constexpr const char* lut_k = "0123456789abcdef";
        constexpr std::size_t size_k = sizeof(value_type);
        constexpr std::size_t bitsize_k = implementation::bitsizeof<value_type>();

        bool first(true);
        std::string result;

        for (const auto& element : digest) {
            if (!first && spaces)
                result += ' ';

            first = false;

            for (std::size_t i(0); i < size_k; ++i) {
                // shifting instead of a char* pointer walk avoids endian issues
                char c((element >> (bitsize_k - (i + 1) * 8)) & 0xff);
                char hi(lut_k[(c >> 4) & 0xf]);
                char lo(lut_k[c & 0xf]);

                result += hi;
                result += lo;
            }
        }

        return result;
    }

    inline std::string to_string(bool spaces = false) { return to_string(finalize(), spaces); }

#ifndef ADOBE_NO_DOCUMENTATION
private:
    // ordered to try and maximize fastest cache alignment. This
    // could be improved.
    std::uint64_t message_size_m;
    typename traits_type::message_block_type state_m;
    typename traits_type::state_digest_type state_digest_m;
    std::uint16_t stuffed_size_m;
#endif
};

/**************************************************************************************************/

/*!
\ingroup sha

\brief A bit-oriented implementation of the SHA-1 Secure Hash Algorithm
*/

typedef sha<implementation::sha1_traits_t> sha1_t;

/*!
\ingroup sha

\brief A bit-oriented implementation of the SHA-224 Secure Hash Algorithm
*/

typedef sha<implementation::sha224_traits_t> sha224_t;

/*!
\ingroup sha

\brief A bit-oriented implementation of the SHA-256 Secure Hash Algorithm
*/

typedef sha<implementation::sha256_traits_t> sha256_t;

/*!
\ingroup sha

\brief A bit-oriented implementation of the SHA-384 Secure Hash Algorithm
*/

typedef sha<implementation::sha384_traits_t> sha384_t;

/*!
\ingroup sha

\brief A bit-oriented implementation of the SHA-512 Secure Hash Algorithm
*/

typedef sha<implementation::sha512_traits_t> sha512_t;

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
