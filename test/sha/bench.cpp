/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

// config
#include <adobe/config.hpp>

// stdc++
#include <iostream>
#include <sstream>

#ifndef USING_OPENSSL
#define USING_OPENSSL 0
#endif

#ifndef USING_BOOSTCRYPTO
#define USING_BOOSTCRYPTO 0
#endif

#if USING_OPENSSL
// openssl
#include <openssl/sha.h>
#endif

// asl
#include <adobe/sha.hpp>
#include <adobe/timer.hpp>

#if ADOBE_PLATFORM_MAC
// Mac OS X Common Crypto
#include <CommonCrypto/CommonDigest.h>
#endif

/*
    Boost.crypto can be obtained from here:

    https://github.com/boost-vault/Miscellaneous/blob/master/crypto_v01.zip

    Unzip the folder and leave it unmodified as a sibling to this source file.

    Then build using:

        bjam define=USING_BOOSTCRYPTO=1 release
*/
#if USING_BOOSTCRYPTO
#include <boost/crypto/sha1.hpp>
#include <boost/crypto/sha2.hpp>
#endif

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

template <typename DigestType>
std::string digest_binary(const DigestType& digest) {
    enum {
        num_bytes = sizeof(typename DigestType::value_type)
    };

    std::string digest_str;

    digest_str.reserve(digest.size() * num_bytes + 1);

    for (auto& curdigest : digest) {
        for (int i(num_bytes - 1); i >= 0; --i) {
            std::size_t shift_amt(8 * i);

            digest_str += char((curdigest >> shift_amt) & 0xff);
        }
    }

    return digest_str;
}

/**************************************************************************************************/

void print_binary_message(const std::string& str, std::size_t unit_size = 0) {
    std::cout << std::hex;

    std::size_t count(0);

    for (std::string::const_iterator first(str.begin()), last(str.end()); first != last; ++first) {
        int value(int(*first) & 0xff);

        std::cout.width(2);
        std::cout.fill('0');

        std::cout << value;

        if (++count == unit_size) {
            std::cout << ' ';
            count = 0;
        }
    }

    std::cout << std::dec;
}

/**************************************************************************************************/

template <typename Hash>
std::vector<std::string> bench(const std::vector<std::string>& corpus, const char* timer_header,
                               Hash hash) {
    std::vector<std::string> result;

    result.reserve(corpus.size());

    adobe::timer_t timer;

    std::for_each(corpus.begin(), corpus.end(),
                  [&](const std::string& element) { result.emplace_back(hash(element)); });

    timer.report(timer_header);

    return result;
}

/**************************************************************************************************/
#if 0
#pragma mark -
#endif
/**************************************************************************************************/

template <typename Hash>
std::vector<std::string> asl_bench(const std::vector<std::string>& corpus) {
    return bench(corpus, "    adobe", [&](const std::string& element) {
        return digest_binary(Hash::digest(element.begin(), element.end()));
    });
}

/**************************************************************************************************/

inline std::vector<std::string> asl_bench_1(const std::vector<std::string>& corpus) {
    return asl_bench<adobe::sha1_t>(corpus);
}

/**************************************************************************************************/

inline std::vector<std::string> asl_bench_224(const std::vector<std::string>& corpus) {
    return asl_bench<adobe::sha224_t>(corpus);
}

/**************************************************************************************************/

inline std::vector<std::string> asl_bench_256(const std::vector<std::string>& corpus) {
    return asl_bench<adobe::sha256_t>(corpus);
}

/**************************************************************************************************/

inline std::vector<std::string> asl_bench_384(const std::vector<std::string>& corpus) {
    return asl_bench<adobe::sha384_t>(corpus);
}

/**************************************************************************************************/

inline std::vector<std::string> asl_bench_512(const std::vector<std::string>& corpus) {
    return asl_bench<adobe::sha512_t>(corpus);
}

/**************************************************************************************************/
#if 0
#pragma mark -
#endif
/**************************************************************************************************/

#if USING_OPENSSL

/**************************************************************************************************/

std::vector<std::string> openssl_bench_1(const std::vector<std::string>& corpus) {
    std::vector<std::uint8_t> digest(SHA_DIGEST_LENGTH, 0);

    return bench(corpus, "  openssl", [&](const std::string& element) {
        SHA1(reinterpret_cast<const std::uint8_t*>(&element[0]), element.size(), &digest[0]);

        return digest_binary(digest);
    });
}

/**************************************************************************************************/

std::vector<std::string> openssl_bench_224(const std::vector<std::string>& corpus) {
    std::vector<std::uint8_t> digest(SHA224_DIGEST_LENGTH, 0);

    return bench(corpus, "  openssl", [&](const std::string& element) {
        SHA224(reinterpret_cast<const std::uint8_t*>(&element[0]), element.size(), &digest[0]);

        return digest_binary(digest);
    });
}

/**************************************************************************************************/

std::vector<std::string> openssl_bench_256(const std::vector<std::string>& corpus) {
    std::vector<std::uint8_t> digest(SHA256_DIGEST_LENGTH, 0);

    return bench(corpus, "  openssl", [&](const std::string& element) {
        SHA256(reinterpret_cast<const std::uint8_t*>(&element[0]), element.size(), &digest[0]);

        return digest_binary(digest);
    });
}

/**************************************************************************************************/

std::vector<std::string> openssl_bench_384(const std::vector<std::string>& corpus) {
    std::vector<std::uint8_t> digest(SHA384_DIGEST_LENGTH, 0);

    return bench(corpus, "  openssl", [&](const std::string& element) {
        SHA384(reinterpret_cast<const std::uint8_t*>(&element[0]), element.size(), &digest[0]);

        return digest_binary(digest);
    });
}

/**************************************************************************************************/

std::vector<std::string> openssl_bench_512(const std::vector<std::string>& corpus) {
    std::vector<std::uint8_t> digest(SHA512_DIGEST_LENGTH, 0);

    return bench(corpus, "  openssl", [&](const std::string& element) {
        SHA512(reinterpret_cast<const std::uint8_t*>(&element[0]), element.size(), &digest[0]);

        return digest_binary(digest);
    });
}

/**************************************************************************************************/

#endif // USING_OPENSSL

/**************************************************************************************************/
#if 0
#pragma mark -
#endif
/**************************************************************************************************/

#if USING_BOOSTCRYPTO

/**************************************************************************************************/

template <typename Hash>
std::vector<std::string> boostcrypto_bench(const std::vector<std::string>& corpus) {
    return bench(corpus, "    boost", [&](const std::string& element) {
        Hash sha(reinterpret_cast<const std::uint8_t*>(&element[0]), element.size());

        const std::uint8_t* digest(reinterpret_cast<const std::uint8_t*>(sha.digest()));

        return std::string(digest, digest + Hash::digest_length);
    });
}

/**************************************************************************************************/

inline std::vector<std::string> boostcrypto_bench_1(const std::vector<std::string>& corpus) {
    return boostcrypto_bench<boost::crypto::sha1>(corpus);
}

/**************************************************************************************************/

inline std::vector<std::string> boostcrypto_bench_224(const std::vector<std::string>& corpus) {
    return boostcrypto_bench<boost::crypto::sha224>(corpus);
}

/**************************************************************************************************/

inline std::vector<std::string> boostcrypto_bench_256(const std::vector<std::string>& corpus) {
    return boostcrypto_bench<boost::crypto::sha256>(corpus);
}

/**************************************************************************************************/

inline std::vector<std::string> boostcrypto_bench_384(const std::vector<std::string>& corpus) {
    return boostcrypto_bench<boost::crypto::sha384>(corpus);
}

/**************************************************************************************************/

inline std::vector<std::string> boostcrypto_bench_512(const std::vector<std::string>& corpus) {
    return boostcrypto_bench<boost::crypto::sha512>(corpus);
}

/**************************************************************************************************/

#endif // USING_BOOSTCRYPTO

/**************************************************************************************************/
#if 0
#pragma mark -
#endif
/**************************************************************************************************/

#if ADOBE_PLATFORM_MAC

/**************************************************************************************************/

std::vector<std::string> commoncrypto_bench_1(const std::vector<std::string>& corpus) {
    std::vector<std::uint8_t> digest(CC_SHA1_DIGEST_LENGTH, 0);

    return bench(corpus, "    apple", [&](const std::string& element) {
        CC_SHA1(reinterpret_cast<const std::uint8_t*>(&element[0]), element.size(), &digest[0]);

        return digest_binary(digest);
    });
}

/**************************************************************************************************/

std::vector<std::string> commoncrypto_bench_224(const std::vector<std::string>& corpus) {
    std::vector<std::uint8_t> digest(CC_SHA224_DIGEST_LENGTH, 0);

    return bench(corpus, "    apple", [&](const std::string& element) {
        CC_SHA224(reinterpret_cast<const std::uint8_t*>(&element[0]), element.size(), &digest[0]);

        return digest_binary(digest);
    });
}

/**************************************************************************************************/

std::vector<std::string> commoncrypto_bench_256(const std::vector<std::string>& corpus) {
    std::vector<std::uint8_t> digest(CC_SHA256_DIGEST_LENGTH, 0);

    return bench(corpus, "    apple", [&](const std::string& element) {
        CC_SHA256(reinterpret_cast<const std::uint8_t*>(&element[0]), element.size(), &digest[0]);

        return digest_binary(digest);
    });
}

/**************************************************************************************************/

std::vector<std::string> commoncrypto_bench_384(const std::vector<std::string>& corpus) {
    std::vector<std::uint8_t> digest(CC_SHA384_DIGEST_LENGTH, 0);

    return bench(corpus, "    apple", [&](const std::string& element) {
        CC_SHA384(reinterpret_cast<const std::uint8_t*>(&element[0]), element.size(), &digest[0]);

        return digest_binary(digest);
    });
}

/**************************************************************************************************/

std::vector<std::string> commoncrypto_bench_512(const std::vector<std::string>& corpus) {
    std::vector<std::uint8_t> digest(CC_SHA512_DIGEST_LENGTH, 0);

    return bench(corpus, "    apple", [&](const std::string& element) {
        CC_SHA512(reinterpret_cast<const std::uint8_t*>(&element[0]), element.size(), &digest[0]);

        return digest_binary(digest);
    });
}

/**************************************************************************************************/

#endif // ADOBE_PLATFORM_MAC

/**************************************************************************************************/

void validate(const std::vector<std::string>& x, const std::vector<std::string>& y) {
    if (x.size() != y.size())
        throw std::runtime_error("Hash vector size mismatch");

    for (std::size_t i(0); i < x.size(); ++i) {
        if (x[i] == y[i])
            continue;

        std::cout << "    x: ";
        print_binary_message(x[i]);
        std::cout << '\n';

        std::cout << "    y: ";
        print_binary_message(y[i]);
        std::cout << '\n';

        throw std::runtime_error("Digest mismatch");
    }
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

int main() try {
#if 0
    std::vector<std::string> corpus;

    for (std::size_t i(0); i < 4; ++i)
    {
        std::size_t sz((i + 1) * 1000000);

        for (char c('a'); c < 'z'; ++c)
            corpus.emplace_back(sz, c);

        for (char c('0'); c < '9'; ++c)
            corpus.emplace_back(sz, c);
    }
#else
    const std::size_t factor(1000000);
    std::size_t n1(1);
    std::size_t n2(1);
    std::vector<std::string> corpus;

    while (n2 <= 5702887) {
        std::size_t count(std::min<std::size_t>(1000000, n2 / factor));

        for (std::size_t i(0); i < count; ++i)
            corpus.emplace_back(n2, static_cast<char>(i));

        // advance the counters
        std::size_t n(n1 + n2);

        n1 = n2;
        n2 = n;
    }
#endif

    std::cout << "SHA-1:\n";
    std::vector<std::string> asl_result_1(asl_bench_1(corpus));
#if USING_OPENSSL
    validate(asl_result_1, openssl_bench_1(corpus));
#endif
#if ADOBE_PLATFORM_MAC
    validate(asl_result_1, commoncrypto_bench_1(corpus));
#endif // ADOBE_PLATFORM_MAC
#if USING_BOOSTCRYPTO
    validate(asl_result_1, boostcrypto_bench_1(corpus));
#endif // USING_BOOSTCRYPTO

    // It would be very surprising to find a difference in timing between
    // SHA-224 and SHA-256. They are, after all, the same fundamental algorithm.
    // But that's why we test these things, isn't it?

    std::cout << "SHA-224:\n";
    std::vector<std::string> asl_result_224(asl_bench_224(corpus));
#if USING_OPENSSL
    validate(asl_result_224, openssl_bench_224(corpus));
#endif
#if ADOBE_PLATFORM_MAC
    validate(asl_result_224, commoncrypto_bench_224(corpus));
#endif // ADOBE_PLATFORM_MAC
#if USING_BOOSTCRYPTO
    validate(asl_result_224, boostcrypto_bench_224(corpus));
#endif // USING_BOOSTCRYPTO

    std::cout << "SHA-256:\n";
    std::vector<std::string> asl_result_256(asl_bench_256(corpus));
#if USING_OPENSSL
    validate(asl_result_256, openssl_bench_256(corpus));
#endif
#if ADOBE_PLATFORM_MAC
    validate(asl_result_256, commoncrypto_bench_256(corpus));
#endif // ADOBE_PLATFORM_MAC
#if USING_BOOSTCRYPTO
    validate(asl_result_256, boostcrypto_bench_256(corpus));
#endif // USING_BOOSTCRYPTO

    // It would be very surprising to find a difference in timing between
    // SHA-384 and SHA-512. They are, after all, the same fundamental algorithm.
    // But that's why we test these things, isn't it?

    std::cout << "SHA-384:\n";
    std::vector<std::string> asl_result_384(asl_bench_384(corpus));
#if USING_OPENSSL
    validate(asl_result_384, openssl_bench_384(corpus));
#endif
#if ADOBE_PLATFORM_MAC
    validate(asl_result_384, commoncrypto_bench_384(corpus));
#endif // ADOBE_PLATFORM_MAC
#if USING_BOOSTCRYPTO
    validate(asl_result_384, boostcrypto_bench_384(corpus));
#endif // USING_BOOSTCRYPTO

    std::cout << "SHA-512:\n";
    std::vector<std::string> asl_result_512(asl_bench_512(corpus));
#if USING_OPENSSL
    validate(asl_result_512, openssl_bench_512(corpus));
#endif
#if ADOBE_PLATFORM_MAC
    validate(asl_result_512, commoncrypto_bench_512(corpus));
#endif // ADOBE_PLATFORM_MAC
#if USING_BOOSTCRYPTO
    validate(asl_result_512, boostcrypto_bench_512(corpus));
#endif // USING_BOOSTCRYPTO

    return 0;
}
catch (const std::exception& error) {
    std::cerr << "Error: " << error.what() << '\n';

    return 1;
}
catch (...) {
    std::cerr << "Error: unknown\n";

    return 1;
}

/**************************************************************************************************/
