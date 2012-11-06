/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#include <adobe/algorithm/copy.hpp>
#include <adobe/algorithm/minmax.hpp>
#include <adobe/algorithm/sort.hpp>
#include <adobe/sha.hpp>
#include <adobe/string.hpp>
#include <adobe/timer.hpp>

#include <iostream>
#include <sstream>

/****************************************************************************************************/

#define ADOBE_TEST_SHA_MILLION_A_STRING 0

/****************************************************************************************************/

/*
    A note about this testing
    
    A lot of this testing comes from documents on the National Institute of Standards and Technology
    (NIST) website
        - http://csrc.nist.gov/cryptval/

    Most of the sample tests come from one of the following documents:
        - SHA Standards document:
            - http://csrc.nist.gov/publications/fips/fips180-2/fips180-2withchangenotice.pdf
        - SHA Verification Standard:
            - http://csrc.nist.gov/cryptval/shs/SHAVS.pdf
        - Wikipedia entry on SHA
            - http://en.wikipedia.org/wiki/SHA-1

    The Pseudorandomly Generated Messages Test is described in the SHAVS document.
*/

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

struct unit_test_t
{
    unit_test_t(const char*         message,
                boost::uint64_t     bitsize,
                const char*         digest) :
        message_m(message, static_cast<std::size_t>(bitsize / 8 + ((bitsize % 8 == 0) ? 0 : 1))), bitsize_m(bitsize), digest_m(digest)
    { }

    unit_test_t(const std::string&  message,
                const char*         digest) :
        message_m(message), bitsize_m((std::numeric_limits<boost::uint64_t>::max)()), digest_m(digest)
    { }

    unit_test_t(const unit_test_t& rhs) :
        message_m(rhs.message_m), bitsize_m(rhs.bitsize_m), digest_m(rhs.digest_m)
    { }

    unit_test_t& operator = (const unit_test_t& rhs)
    {
        message_m = rhs.message_m;
        bitsize_m = rhs.bitsize_m;
        digest_m = rhs.digest_m;

        return *this;
    }

    std::string     message_m;
    boost::uint64_t bitsize_m;
    const char*     digest_m;
};

/****************************************************************************************************/

template <typename DigestType>
std::string digest_string(const DigestType& digest, bool spaces = true)
{
    std::stringstream digest_str;

    digest_str << std::hex;

    for (   typename DigestType::const_iterator first(digest.begin()),
            last(digest.end()); first != last; ++first)
    {
        digest_str.width(sizeof(typename DigestType::value_type) * 2);
        digest_str.fill('0');

        digest_str << *first;

        if (first + 1 != last && spaces)
            digest_str << ' ';
    }

    return digest_str.str();
}

/****************************************************************************************************/

template <typename DigestType>
std::string digest_binary(const DigestType& digest)
{
    enum { num_bytes = sizeof(typename DigestType::value_type) };

    std::string digest_str;

    digest_str.reserve(digest.size() * num_bytes + 1);

    for (   typename DigestType::const_iterator first(digest.begin()),
            last(digest.end()); first != last; ++first)
    {
        typename DigestType::value_type cur(*first);

        for (int i(num_bytes - 1); i >= 0; --i)
        {
            std::size_t shift_amt(8 * i);

            digest_str += char((cur >> shift_amt) & 0xff);
        }
    }

    return digest_str;
}

/****************************************************************************************************/

void print_binary_message(const std::string& str, std::size_t unit_size)
{
    std::cout << std::hex;

    std::size_t count(0);

    for (   std::string::const_iterator first(str.begin()),
            last(str.end()); first != last; ++first)
    {
        int value(int(*first) & 0xff);

        std::cout.width(2);
        std::cout.fill('0');

        std::cout << value;

        if (++count == unit_size)
        {
            std::cout << ' ';
            count = 0;
        }
    }

    std::cout << std::dec;
}

/****************************************************************************************************/

template <typename HashT>
std::string pgmt_seed();

template <>
std::string pgmt_seed<adobe::sha1_t>()
{ return "\xd0\x56\x9c\xb3\x66\x5a\x8a\x43\xeb\x6e\xa2\x3d\x75\xa3\xc4\xd2\x05\x4a\x0d\x7d"; }

template <>
std::string pgmt_seed<adobe::sha224_t>()
{ return "\xd0\x56\x9c\xb3\x66\x5a\x8a\x43\xeb\x6e\xa2\x3d\x75\xa3\xc4\xd2\x05\x4a\x0d\x7d\x66\xa9\xca\x99\xc9\xce\xb0\x27"; }

template <>
std::string pgmt_seed<adobe::sha256_t>()
{ return "\xf4\x1e\xce\x26\x13\xe4\x57\x39\x15\x69\x6b\x5a\xdc\xd5\x1c\xa3\x28\xbe\x3b\xf5\x66\xa9\xca\x99\xc9\xce\xb0\x27\x9c\x1c\xb0\xa7"; }

template <>
std::string pgmt_seed<adobe::sha384_t>()
{ return "\x82\x40\xbc\x51\xe4\xec\x7e\xf7\x6d\x18\xe3\x52\x04\xa1\x9f\x51\xa5\x21\x3a\x73\xa8\x1d\x6f\x94\x46\x80\xd3\x07\x59\x48\xb7\xe4\x63\x80\x4e\xa3\xd2\x6e\x13\xea\x82\x0d\x65\xa4\x84\xbe\x74\x53"; }

template <>
std::string pgmt_seed<adobe::sha512_t>()
{ return std::string("\x47\x3f\xf1\xb9\xb3\xff\xdf\xa1\x26\x69\x9a\xc7\xef\x9e\x8e\x78\x77\x73\x09\x58\x24\xc6\x42\x55\x7c\x13\x99\xd9\x8e\x42\x20\x44\x8d\xc3\x5b\x99\xbf\xdd\x44\x77\x95\x43\x92\x4c\x1c\xe9\x3b\xc5\x94\x15\x38\x89\x5d\xb9\x88\x26\x1b\x00\x77\x4b\x12\x27\x20\x39", 64); }

/****************************************************************************************************/

template <typename HashT>
void pgmt(std::string seed)
{
    // Pseudorandomly Generated Messages Test
    // REVISIT (fbrereto) : Automate verification of output

    typedef HashT                           hash_type;
    typedef typename hash_type::digest_type digest_type;

    std::cout << "*** pseudorandomly generated messages test for " << typeid(hash_type).name() << std::endl;

    std::string md[1003];
    hash_type   hash;

    for (std::size_t j(0); j < 4; ++j)
    {
        md[0] = seed;
        md[1] = seed;
        md[2] = seed;

        std::string m[1003];

        for (std::size_t i(3); i < 1003; ++i)
        {
            m[i] = md[i - 3] + md[i - 2] + md[i - 1];

            md[i] = digest_binary(hash.digest(m[i].begin(), m[i].end()));
        }

        seed = md[1002];

        md[j] = seed;

        std::cout << "\tMD[" << static_cast<unsigned int>(j) << "]: ";
        print_binary_message(md[j], sizeof(typename digest_type::value_type));
        std::cout << std::endl;
    }
}

/****************************************************************************************************/

template <typename HashT, typename I> // I models InputIterator
bool test_hash(I first, I last)
{
    typedef HashT                           hash_type;
    typedef typename hash_type::digest_type digest_type;

    hash_type   hash;
    std::size_t test_count(0);

    std::cout << "*** " << typeid(hash).name() << " unit testing" << std::endl;

    while (first != last)
    {
        const unit_test_t&          cur_test(*first);
        const std::string&          test_string(cur_test.message_m);
        std::string::const_iterator test_string_first(test_string.begin());
        std::string::const_iterator test_string_last(test_string.end());
        bool                        modified_bitsize(cur_test.bitsize_m != (std::numeric_limits<boost::uint64_t>::max)());
        adobe::timer_t              timer;
        digest_type                 hash_digest((modified_bitsize) ?
                                        hash.digest(test_string_first, cur_test.bitsize_m) :
                                        hash.digest(test_string_first, test_string_last));
        double                      time(timer.split());
        std::string                 digest(digest_string(hash_digest));
        bool                        test_passed(digest == cur_test.digest_m);

        if (test_passed)
        {
            std::cout   << "\ttest " << static_cast<unsigned int>(++test_count) << " passed in "
                        << time << " miliseconds (" << (time / 1e3)
                        << " seconds)" << std::endl;
        }
        else
        {
            std::cout   << "test " << static_cast<unsigned int>(++test_count) << ":" << std::endl
                        << "\tmessage length: " << static_cast<unsigned int>(test_string.size()) << " bytes ("
                            << static_cast<unsigned int>(modified_bitsize ? cur_test.bitsize_m : test_string.size() * 8)
                            << " bits)" << std::endl
                        << "\tdigest:         " << digest << std::endl
                        << "\tknown good:     " << cur_test.digest_m << std::endl
                        << "\tsame?:          " << (test_passed ? "Yes" : "NO") << std::endl
                        << "\ttime to digest: " << time << " miliseconds (" << (time / 1e3) << " seconds)"
                        << std::endl;

            return false;
        }

        ++first;
    }

    // Perform the Pseudorandomly Generated Messages Test

    pgmt<HashT>(pgmt_seed<HashT>());

    return true;
}

/****************************************************************************************************/

}// namespace

/****************************************************************************************************/

int main()
{
    std::cout << "Compiled " << __DATE__ << " " << __TIME__ << std::endl;


    /************************** SHA-1 Unit Tests **************************/


    std::vector<unit_test_t> sha1_test_set;

    sha1_test_set.push_back(unit_test_t("abc",
        "a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d"));
    sha1_test_set.push_back(unit_test_t("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        "84983e44 1c3bd26e baae4aa1 f95129e5 e54670f1"));
#if ADOBE_TEST_SHA_MILLION_A_STRING
    sha1_test_set.push_back(unit_test_t(std::string(1000000, 'a'),
        "34aa973c d4c4daa4 f61eeb2b dbad2731 6534016f"));
#endif
    sha1_test_set.push_back(unit_test_t("The quick brown fox jumps over the lazy dog",
        "2fd4e1c6 7a2d28fc ed849ee1 bb76e739 1b93eb12"));
    sha1_test_set.push_back(unit_test_t("The quick brown fox jumps over the lazy cog",
        "de9f2c7f d25e1b3a fad3e85a 0bd17d9b 100db4b3"));
    sha1_test_set.push_back(unit_test_t("",
        "da39a3ee 5e6b4b0d 3255bfef 95601890 afd80709"));
    sha1_test_set.push_back(unit_test_t("\x98", 5,
        "29826b00 3b906e66 0eff4027 ce98af35 31ac75ba"));
    sha1_test_set.push_back(unit_test_t("\x49\xb2\xae\xc2\x59\x4b\xbe\x3a\x3b\x11\x75\x42\xd9\x4a\xc8\x80", 123,
        "6239781e 03729919 c01955b3 ffa8acb6 0b988340"));
    sha1_test_set.push_back(unit_test_t("\x65\xf9\x32\x99\x5b\xa4\xce\x2c\xb1\xb4\xa2\xe7\x1a\xe7\x02\x20\xaa\xce\xc8\x96\x2d\xd4\x49\x9c\xbd\x7c\x88\x7a\x94\xea\xaa\x10\x1e\xa5\xaa\xbc\x52\x9b\x4e\x7e\x43\x66\x5a\x5a\xf2\xcd\x03\xfe\x67\x8e\xa6\xa5\x00\x5b\xba\x3b\x08\x22\x04\xc2\x8b\x91\x09\xf4\x69\xda\xc9\x2a\xaa\xb3\xaa\x7c\x11\xa1\xb3\x2a\xe0", 611,
        "8c5b2a5d dae5a97f c7f9d856 61c672ad bf7933d4"));

    if (!test_hash<adobe::sha1_t>(sha1_test_set.begin(), sha1_test_set.end()))
        return 1;


    /************************** SHA-224 Unit Tests **************************/


    std::vector<unit_test_t> sha224_test_set;

    sha224_test_set.push_back(unit_test_t("abc",
        "23097d22 3405d822 8642a477 bda255b3 2aadbce4 bda0b3f7 e36c9da7"));
    sha224_test_set.push_back(unit_test_t("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        "75388b16 512776cc 5dba5da1 fd890150 b0c6455c b4f58b19 52522525"));
#if ADOBE_TEST_SHA_MILLION_A_STRING
    sha224_test_set.push_back(unit_test_t(std::string(1000000, 'a'),
        "20794655 980c91d8 bbb4c1ea 97618a4b f03f4258 1948b2ee 4ee7ad67"));
#endif
    sha224_test_set.push_back(unit_test_t("\x68", 5,
        "e3b04855 2c3c387b cab37f6e b06bb79b 96a4aee5 ff27f515 31a9551c"));
    sha224_test_set.push_back(unit_test_t("\xf0\x70\x06\xf2\x5a\x0b\xea\x68\xcd\x76\xa2\x95\x87\xc2\x8d\xa0", 123,
        "1b01db6c b4a9e43d ed1516be b3db0b87 b6d1ea43 187462c6 08137150"));
    sha224_test_set.push_back(unit_test_t("\xa2\xbe\x6e\x46\x32\x81\x09\x02\x94\xd9\xce\x94\x82\x65\x69\x42\x3a\x3a\x30\x5e\xd5\xe2\x11\x6c\xd4\xa4\xc9\x87\xfc\x06\x57\x00\x64\x91\xb1\x49\xcc\xd4\xb5\x11\x30\xac\x62\xb1\x9d\xc2\x48\xc7\x44\x54\x3d\x20\xcd\x39\x52\xdc\xed\x1f\x06\xcc\x3b\x18\xb9\x1f\x3f\x55\x63\x3e\xcc\x30\x85\xf4\x90\x70\x60\xd2\xe0", 611,
        "54bea6ea b8195a2e b0a7906a 4b4a8766 66300eef bd1f3b84 74f9cd57"));

    if (!test_hash<adobe::sha224_t>(sha224_test_set.begin(), sha224_test_set.end()))
        return 1;


    /************************** SHA-256 Unit Tests **************************/


    std::vector<unit_test_t> sha256_test_set;

    sha256_test_set.push_back(unit_test_t("abc",
        "ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 96177a9c b410ff61 f20015ad"));
    sha256_test_set.push_back(unit_test_t("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        "248d6a61 d20638b8 e5c02693 0c3e6039 a33ce459 64ff2167 f6ecedd4 19db06c1"));
#if ADOBE_TEST_SHA_MILLION_A_STRING
    sha256_test_set.push_back(unit_test_t(std::string(1000000, 'a'),
        "cdc76e5c 9914fb92 81a1c7e2 84d73e67 f1809a48 a497200e 046d39cc c7112cd0"));
#endif
    sha256_test_set.push_back(unit_test_t("\x68", 5,
        "d6d3e02a 31a84a8c aa9718ed 6c2057be 09db45e7 823eb507 9ce7a573 a3760f95"));
    sha256_test_set.push_back(unit_test_t("\xbe\x27\x46\xc6\xdb\x52\x76\x5f\xdb\x2f\x88\x70\x0f\x9a\x73\x60", 123,
        "77ec1dc8 9c821ff2 a1279089 fa091b35 b8cd960b caf7de01 c6a76807 56beb972"));
    sha256_test_set.push_back(unit_test_t("\x3e\x74\x03\x71\xc8\x10\xc2\xb9\x9f\xc0\x4e\x80\x49\x07\xef\x7c\xf2\x6b\xe2\x8b\x57\xcb\x58\xa3\xe2\xf3\xc0\x07\x16\x6e\x49\xc1\x2e\x9b\xa3\x4c\x01\x04\x06\x91\x29\xea\x76\x15\x64\x25\x45\x70\x3a\x2b\xd9\x01\xe1\x6e\xb0\xe0\x5d\xeb\xa0\x14\xeb\xff\x64\x06\xa0\x7d\x54\x36\x4e\xff\x74\x2d\xa7\x79\xb0\xb3\xa0", 611,
        "3e9ad646 8bbbad2a c3c2cdc2 92e018ba 5fd70b96 0cf16797 77fce708 fdb066e9"));

    if (!test_hash<adobe::sha256_t>(sha256_test_set.begin(), sha256_test_set.end()))
        return 1;


    /************************** SHA-384 Unit Tests **************************/


    std::vector<unit_test_t> sha384_test_set;

    sha384_test_set.push_back(unit_test_t("abc",
        "cb00753f45a35e8b b5a03d699ac65007 272c32ab0eded163 1a8b605a43ff5bed 8086072ba1e7cc23 58baeca134c825a7"));
    sha384_test_set.push_back(unit_test_t("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
        "09330c33f71147e8 3d192fc782cd1b47 53111b173b3b05d2 2fa08086e3b0f712 fcc7c71a557e2db9 66c3e9fa91746039"));
#if ADOBE_TEST_SHA_MILLION_A_STRING
    sha384_test_set.push_back(unit_test_t(std::string(1000000, 'a'),
        "9d0e1809716474cb 086e834e310a4a1c ed149e9c00f24852 7972cec5704c2a5b 07b8b3dc38ecc4eb ae97ddd87f3d8985"));
#endif
    sha384_test_set.push_back(unit_test_t("\x10", 5,
        "8d17be79e32b6718 e07d8a603eb84ba0 478f7fcfd1bb9399 5f7d1149e09143ac 1ffcfc56820e469f 3878d957a15a3fe4"));
    sha384_test_set.push_back(unit_test_t("\x8b\xc5\x00\xc7\x7c\xee\xd9\x87\x9d\xa9\x89\x10\x7c\xe0\xaa\xa0", 123,
        "d8c43b38e12e7c42 a7c9b810299fd6a7 70bef30920f17532 a898de62c7a07e42 93449c0b5fa70109 f0783211cfc4bce3"));
    sha384_test_set.push_back(unit_test_t("\x68\xf5\x01\x79\x2d\xea\x97\x96\x76\x70\x22\xd9\x3d\xa7\x16\x79\x30\x99\x20\xfa\x10\x12\xae\xa3\x57\xb2\xb1\x33\x1d\x40\xa1\xd0\x3c\x41\xc2\x40\xb3\xc9\xa7\x5b\x48\x92\xf4\xc0\x72\x4b\x68\xc8\x75\x32\x1a\xb8\xcf\xe5\x02\x3b\xd3\x75\xbc\x0f\x94\xbd\x89\xfe\x04\xf2\x97\x10\x5d\x7b\x82\xff\xc0\x02\x1a\xeb\x1c\xcb\x67\x4f\x52\x44\xea\x34\x97\xde\x26\xa4\x19\x1c\x5f\x62\xe5\xe9\xa2\xd8\x08\x2f\x05\x51\xf4\xa5\x30\x68\x26\xe9\x1c\xc0\x06\xce\x1b\xf6\x0f\xf7\x19\xd4\x2f\xa5\x21\xc8\x71\xcd\x23\x94\xd9\x6e\xf4\x46\x8f\x21\x96\x6b\x41\xf2\xba\x80\xc2\x6e\x83\xa9\xe0", 1123,
        "5860e8de91c21578 bb4174d227898a98 e0b45c4c760f0095 49495614daedc077 5d92d11d9f8ce9b0 64eeac8dafc3a297"));

    if (!test_hash<adobe::sha384_t>(sha384_test_set.begin(), sha384_test_set.end()))
        return 1;


    /************************** SHA-512 Unit Tests **************************/


    std::vector<unit_test_t> sha512_test_set;

    sha512_test_set.push_back(unit_test_t("abc",
        "ddaf35a193617aba cc417349ae204131 12e6fa4e89a97ea2 0a9eeee64b55d39a 2192992a274fc1a8 36ba3c23a3feebbd 454d4423643ce80e 2a9ac94fa54ca49f"));
    sha512_test_set.push_back(unit_test_t("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
        "8e959b75dae313da 8cf4f72814fc143f 8f7779c6eb9f7fa1 7299aeadb6889018 501d289e4900f7e4 331b99dec4b5433a c7d329eeb6dd2654 5e96e55b874be909"));
#if ADOBE_TEST_SHA_MILLION_A_STRING
    sha512_test_set.push_back(unit_test_t(std::string(1000000, 'a'),
        "e718483d0ce76964 4e2e42c7bc15b463 8e1f98b13b204428 5632a803afa973eb de0ff244877ea60a 4cb0432ce577c31b eb009c5c2c49aa2e 4eadb217ad8cc09b"));
#endif
    sha512_test_set.push_back(unit_test_t("\xb0", 5,
        "d4ee29a9e9098544 6b913cf1d1376c83 6f4be2c1cf3cada0 720a6bf4857d886a 7ecb3c4e4c0fa8c7 f95214e41dc1b0d2 1b22a84cc03bf8ce 4845f34dd5bdbad4"));
    sha512_test_set.push_back(unit_test_t("\x08\xec\xb5\x2e\xba\xe1\xf7\x42\x2d\xb6\x2b\xcd\x54\x26\x70\x80", 123,
        "ed8dc78e8b01b697 50053dbb7a0a9eda 0fb9e9d292b1ed71 5e80a7fe290a4e16 664fd913e8585440 0c5af05e6dad316b 7359b43e64f8bec3 c1f237119986bbb6"));
    sha512_test_set.push_back(unit_test_t("\x3a\xdd\xec\x85\x59\x32\x16\xd1\x61\x9a\xa0\x2d\x97\x56\x97\x0b\xfc\x70\xac\xe2\x74\x4f\x7c\x6b\x27\x88\x15\x10\x28\xf7\xb6\xa2\x55\x0f\xd7\x4a\x7e\x6e\x69\xc2\xc9\xb4\x5f\xc4\x54\x96\x6d\xc3\x1d\x2e\x10\xda\x1f\x95\xce\x02\xbe\xb4\xbf\x87\x65\x57\x4c\xbd\x6e\x83\x37\xef\x42\x0a\xdc\x98\xc1\x5c\xb6\xd5\xe4\xa0\x24\x1b\xa0\x04\x6d\x25\x0e\x51\x02\x31\xca\xc2\x04\x6c\x99\x16\x06\xab\x4e\xe4\x14\x5b\xee\x2f\xf4\xbb\x12\x3a\xab\x49\x8d\x9d\x44\x79\x4f\x99\xcc\xad\x89\xa9\xa1\x62\x12\x59\xed\xa7\x0a\x5b\x6d\xd4\xbd\xd8\x77\x78\xc9\x04\x3b\x93\x84\xf5\x49\x06\x80", 1123,
        "32ba76fc30eaa020 8aeb50ffb5af1864 fdbf17902a4dc0a6 82c61fcea6d92b78 3267b21080301837 f59de79c6b337db2 526f8a0a510e5e53 cafed4355fe7c2f1"));

    if (!test_hash<adobe::sha512_t>(sha512_test_set.begin(), sha512_test_set.end()))
        return 1;


    return 0;
}
