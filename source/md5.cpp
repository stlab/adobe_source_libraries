/*
    Copyright 2005-2007 Adobe Systems Incorporated and others
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/md5.hpp>

#include <cstring>

/*************************************************************************************************/

/*
    Relevant copyright information is provided below and may not be removed from this file. 

    Derived from the RSA Data Security, Inc. MD5 Message-Digest Algorithm.
*/

/*************************************************************************************************/

/*
    MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm

    Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights 
    reserved.

    License to copy and use this software is granted provided that it is 
    identified as the "RSA Data Security, Inc. MD5 Message-Digest Algorithm" in 
    all material mentioning or referencing this software or this function. 

    License is also granted to make and use derivative works provided that such 
    works are identified as "derived from the RSA Data Security, Inc. MD5 
    Message-Digest Algorithm" in all material mentioning or referencing the 
    derived work. 
    
    RSA Data Security, Inc. makes no representations concerning either the 
    merchantability of this software or the suitability of this software for 
    any particular purpose. It is provided "as is" without express or implied 
    warranty of any kind. 
    
    These notices must be retained in any copies of any part of this 
    documentation and/or software. 
*/

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

namespace {

/*************************************************************************************************/

/* Constants for MD5Transform routine. */

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

/*************************************************************************************************/

void MD5Transform (boost::uint32_t [4], boost::uint8_t [64]);
void Encode (boost::uint8_t *, boost::uint32_t *, boost::uint16_t);
void Decode (boost::uint32_t *, boost::uint8_t *, boost::uint16_t);

/*************************************************************************************************/

/* F, G, H and I are basic MD5 functions. */

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/*************************************************************************************************/

/* ROTATE_LEFT rotates x left n bits. */

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/*************************************************************************************************/

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation. */

#define FF(a, b, c, d, x, s, ac) { \
     (a) += F ((b), (c), (d)) + (x) + (boost::uint32_t)(ac); \
     (a) = ROTATE_LEFT ((a), (s)); \
     (a) += (b); \
     }
#define GG(a, b, c, d, x, s, ac) { \
     (a) += G ((b), (c), (d)) + (x) + (boost::uint32_t)(ac); \
     (a) = ROTATE_LEFT ((a), (s)); \
     (a) += (b); \
     }
#define HH(a, b, c, d, x, s, ac) { \
     (a) += H ((b), (c), (d)) + (x) + (boost::uint32_t)(ac); \
     (a) = ROTATE_LEFT ((a), (s)); \
     (a) += (b); \
     }
#define II(a, b, c, d, x, s, ac) { \
     (a) += I ((b), (c), (d)) + (x) + (boost::uint32_t)(ac); \
     (a) = ROTATE_LEFT ((a), (s)); \
     (a) += (b); \
     }

/*************************************************************************************************/

/* MD5 basic transformation. Transforms state based on block. */

void MD5Transform ( boost::uint32_t state[4],
                    boost::uint8_t block[64])
{
    boost::uint32_t a = state[0], b = state[1], c = state[2], d = state[3], x[16];

    Decode (x, block, 64);

    /* Round 1 */
    FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    /* Round 2 */
    GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
    HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

    /* Round 4 */
    II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    /* Zeroize sensitive information. */
    std::memset (x, 0, sizeof(x));
}

/*************************************************************************************************/

/* Encodes input (boost::uint32_t) into output (boost::uint8_t). Assumes len is a multiple of 4. */

void Encode (   boost::uint8_t*     output,
                boost::uint32_t*    input,
                boost::uint16_t     len)
{
    boost::uint16_t i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
    {
        output[j] = (boost::uint8_t)(input[i] & 0xff);
        output[j+1] = (boost::uint8_t)((input[i] >> 8) & 0xff);
        output[j+2] = (boost::uint8_t)((input[i] >> 16) & 0xff);
        output[j+3] = (boost::uint8_t)((input[i] >> 24) & 0xff);
    }
}

/*************************************************************************************************/

/* Decodes input (boost::uint8_t) into output (boost::uint32_t). Assumes len is a multiple of 4. */

void Decode (   boost::uint32_t*    output,
                boost::uint8_t*     input,
                boost::uint16_t     len)
{
     boost::uint16_t i, j;

     for (i = 0, j = 0; j < len; i++, j += 4)
        output[i] = ((boost::uint32_t)input[j]) | (((boost::uint32_t)input[j+1]) << 8) |
            (((boost::uint32_t)input[j+2]) << 16) | (((boost::uint32_t)input[j+3]) << 24);
}

/*************************************************************************************************/

} // namespace

/*************************************************************************************************/

md5_t::md5_t()
{ reset(); }

/*************************************************************************************************/

/* MD5 initialization. Begins an MD5 operation, writing a new context. */

void md5_t::reset()
{
    count_m[0] = 0;
    count_m[1] = 0;

    /* Load magic initialization constants. */

    state_m[0] = 0x67452301;
    state_m[1] = 0xefcdab89;
    state_m[2] = 0x98badcfe;
    state_m[3] = 0x10325476;
}

/*************************************************************************************************/

/*
    MD5 block update operation. Continues an MD5 message-digest operation,
    processing another message block, and updating the context.
*/

void md5_t::update(void* input_block, std::size_t input_length)
{
     
    /* Compute number of bytes mod 64 */
    std::size_t index ((count_m[0] >> 3) & 0x3f);

    /* Update number of bits */
    boost::uint32_t lsb_length (static_cast<boost::uint32_t>(input_length << 3)); // low order length in bits
    count_m[0] += lsb_length;
    count_m[1] += count_m[0] < lsb_length; // add cary bit
    count_m[1] += static_cast<boost::uint32_t>(input_length >> 29); // high order bits.
    
    std::size_t partLen (64 - index);

    /* Transform as many times as possible. */
    std::size_t i (0);
    
    if (input_length >= partLen)
    {
        std::memcpy (&buffer_m[index], input_block, partLen);
        MD5Transform (state_m, buffer_m);

        for (i = partLen; i + 63 < input_length; i += 64)
            MD5Transform (state_m, &static_cast<boost::uint8_t*>(input_block)[i]);

        index = 0;
     }

    /* Buffer remaining input */
    std::memcpy (&buffer_m[index], &static_cast<boost::uint8_t*>(input_block)[i], input_length - i);
}

/*************************************************************************************************/

/*
    MD5 finalization. Ends an MD5 message-digest operation,
    writing the the message digest and resets the context.
*/

md5_t::digest_t md5_t::final()
{
    static boost::uint8_t padding_s[64] =
    {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    digest_t digest;

    boost::uint8_t bits[8];

    /* Save number of bits */
    Encode (bits, count_m, 8);

    /* Pad out to 56 mod 64. */
    std::size_t index   ((count_m[0] >> 3) & 0x3f);
    std::size_t padLen  ((index < 56) ? (56 - index) : (120 - index));

    update (padding_s, padLen);

    /* Append length (before padding) */
    update (bits, 8);

    /* Store state in digest */
    Encode (&digest[0], state_m, 16);

    /* Reset sensitive information. */
    reset();

    return digest;
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
