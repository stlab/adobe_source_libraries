/*
    Copyright 2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/name.hpp>
#include <adobe/timer.hpp>

#include <iostream>

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

typedef std::size_t (*hash_proc_t)(adobe::name_t name);

/****************************************************************************************************/

std::size_t fnv1a(adobe::name_t name)
{
    static_assert(sizeof(std::size_t) == 8, "std::size_t size mismatch.");

    std::size_t result(14695981039346656037ULL);

    for (const char* first = name.c_str(); *first; ++first)
        result = (result xor static_cast<std::size_t>(*first)) * 1099511628211ULL;

    return result;
}

/****************************************************************************************************/

std::vector<adobe::name_t> corpus1_init()
{
    std::vector<adobe::name_t> result;

    for (std::size_t i(0); i < 216553; ++i)
        result.emplace_back(adobe::name_t(std::to_string(i).c_str()));

    return result;
}

/****************************************************************************************************/

const std::vector<adobe::name_t>& corpus1()
{
    static std::vector<adobe::name_t> result_s(corpus1_init());

    return result_s;
}

/****************************************************************************************************/

void hash_test_specific(const std::vector<adobe::name_t>& corpus,
                        const char*                       corpus_name,
                        hash_proc_t                       hash_proc,
                        const char*                       hash_name)
{
    typedef std::map<std::size_t, adobe::name_t> collider_t;

    collider_t     collider;
    std::size_t    collision_count(0);
    adobe::timer_t timer;

    for (auto& datum : corpus)
    {
        timer.reset();

        std::size_t hash(hash_proc(datum));

        timer.accrue();

        collider_t::const_iterator found(collider.find(hash));

        if (found == collider.end())
        {
            collider.insert(collider_t::value_type(hash, datum));
        }
        else
        {
            ++collision_count;
            // std::cout << "  " << datum << "/" << found->second << ": 0x" << std::hex << hash << std::dec << '\n';
        }
    }

    double time(timer.accrued_total());

    std::cout << "----\n"
              << "Corpus: " << corpus_name << '\n'
              << "  Hash: " << hash_name << '\n'
              << "  Coll: " << collision_count << '\n'
              << "  Time: " << time << "ms (" << (time / 1e3) << " s)\n";
}

/****************************************************************************************************/

void hash_test()
{
    hash_test_specific(corpus1(), "lexed numbers", &adobe::version_1::hash_value, "hash_value");

    hash_test_specific(corpus1(), "lexed numbers", &fnv1a, "fnv1a");
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

int main()
try
{
    hash_test();

    return 0;
}
catch (std::exception& error)
{
    std::cout << "Error: " << error.what() << '\n';

    return 1;
}
catch (...)
{
    std::cout << "Error: unknown\n";

    return 1;
}
