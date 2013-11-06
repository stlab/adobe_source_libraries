/*
    Copyright 2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

#include <adobe/name.hpp>
#include <adobe/timer.hpp>
#include <adobe/zuid.hpp>
#include <adobe/sha.hpp>

#include <iostream>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

typedef std::size_t (*hash_proc_t)(adobe::name_t name);

/****************************************************************************************************/

std::size_t hash_value(adobe::name_t name) {
    std::size_t seed(0);

    for (const char *first(name.c_str()); *first; ++first)
        seed = 5 * seed + *first;

    return seed;
}

/****************************************************************************************************/

std::size_t fnv1a64(adobe::name_t name) {
    return adobe::fnv1a<64>(name.c_str(), adobe::logical_not());
}

/****************************************************************************************************/

std::size_t fnv1a32(adobe::name_t name) {
    return adobe::fnv1a<32>(name.c_str(), adobe::logical_not());
}

/****************************************************************************************************/

std::vector<adobe::name_t> lexed_number_corpus_init() {
    std::vector<adobe::name_t> result;

    for (std::size_t i(0); i < 500000; ++i)
        result.emplace_back(adobe::name_t(std::to_string(i).c_str()));

    return result;
}

/****************************************************************************************************/

const std::vector<adobe::name_t> &lexed_number_corpus() {
    static std::vector<adobe::name_t> result_s(lexed_number_corpus_init());

    return result_s;
}

/****************************************************************************************************/

std::vector<adobe::name_t> zuid_corpus_init() {
    adobe::zuid_t zuid("0a90862c-fb82-1fff-8749-bd8434b0eeb7");
    std::vector<adobe::name_t> result;

    for (std::size_t i(0); i < 500000; ++i) {
        result.emplace_back(adobe::name_t(zuid.c_str()));

        zuid = adobe::zuid_t(zuid, zuid.str());
    }

    return result;
}

/****************************************************************************************************/

const std::vector<adobe::name_t> &zuid_corpus() {
    static std::vector<adobe::name_t> result_s(zuid_corpus_init());

    return result_s;
}

/****************************************************************************************************/

std::vector<adobe::name_t> word_corpus_init(boost::filesystem::ifstream &stream) {
    std::vector<adobe::name_t> result;

    while (stream) {
        std::string line;

        std::getline(stream, line);

        result.emplace_back(adobe::name_t(line.c_str()));
    }

    return result;
}

/****************************************************************************************************/

const std::vector<adobe::name_t> &word_corpus(boost::filesystem::ifstream &stream) {
    static std::vector<adobe::name_t> result_s(word_corpus_init(stream));

    return result_s;
}

/****************************************************************************************************/

struct collision_t {
    adobe::name_t name1_m;
    adobe::name_t name2_m;
    std::size_t hash_m;
};

/****************************************************************************************************/

void hash_test_specific(const std::vector<adobe::name_t> &corpus, const char *corpus_name,
                        hash_proc_t hash_proc, const char *hash_name) {
    typedef std::map<std::size_t, adobe::name_t> collider_t;

    collider_t collider;
    std::vector<collision_t> collision_vector;

    std::vector<collider_t::value_type> hash_result;

    hash_result.reserve(corpus.size());

    adobe::timer_t timer;

    for (auto &datum : corpus)
        hash_result.emplace_back(collider_t::value_type{hash_proc(datum), datum});

    double time(timer.split());

    for (auto &value : hash_result) {
        collider_t::const_iterator found(collider.find(value.first));

        if (found == collider.end()) {
            collider.insert(value);
        } else {
            collision_vector.push_back(collision_t{value.second, found->second, value.first});
        }
    }

    std::cout << "----\n"
              << "Corpus: " << corpus_name << '\n' << "Hash: " << hash_name << '\n'
              << "Time: " << time << "ms (" << (time / 1e3) << " s)\n"
              << "Collisions: " << collision_vector.size() << '\n';

    if (!collision_vector.empty() && collision_vector.size() < 50) {
        std::cout << "Details:\n";

        for (auto &collision : collision_vector) {
            std::cout << "  " << collision.name1_m << "/" << collision.name2_m << ": 0x" << std::hex
                      << collision.hash_m << std::dec << '\n';
        }
    }
}

/****************************************************************************************************/

void hash_test_corpus(const std::vector<adobe::name_t> &corpus, const char *corpus_name) {
    hash_test_specific(corpus, corpus_name, &hash_value, "hash_value");

    hash_test_specific(corpus, corpus_name, &fnv1a64, "fnv1a64");

    hash_test_specific(corpus, corpus_name, &fnv1a32, "fnv1a32");
}

/****************************************************************************************************/

void hash_test(boost::filesystem::ifstream &&corpus_file) {
    hash_test_corpus(lexed_number_corpus(), "lexed numbers");
    hash_test_corpus(zuid_corpus(), "zuids");

    if (corpus_file)
        hash_test_corpus(word_corpus(corpus_file), "corpus_file");
    else
        std::cout << "INFO : Pass a corpus file to test further.\n";
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

int main(int argc, char **argv) try {
    boost::filesystem::path corpus_file;

    if (argc > 1)
        corpus_file = boost::filesystem::path(argv[1]);

    hash_test(boost::filesystem::ifstream(corpus_file));

    return 0;
}
catch (std::exception &error) {
    std::cout << "Error: " << error.what() << '\n';

    return 1;
}
catch (...) {
    std::cout << "Error: unknown\n";

    return 1;
}

/****************************************************************************************************/
