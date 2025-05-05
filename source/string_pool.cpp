/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/implementation/string_pool.hpp>

#include <cassert>
#include <cstddef>
#include <vector>

#include <adobe/algorithm/copy.hpp>
#include <adobe/algorithm/for_each.hpp>
#include <adobe/functional.hpp>
#include <adobe/string.hpp>
#include <adobe/name.hpp>
#include <adobe/closed_hash.hpp>

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

constexpr std::size_t empty_hash_s = adobe::detail::name_hash("");

struct str_name_hash {
    std::size_t operator()(const char* str) const {
        return adobe::detail::name_hash(str, std::strlen(str));
    }
};

/**************************************************************************************************/

class string_pool_t : boost::noncopyable {
private:
    enum { default_pool_size_k = 1024 * 4 };

public:
    explicit string_pool_t(std::size_t pool_size = default_pool_size_k)
        : pool_size_m(pool_size), next_m(NULL), end_m(NULL) {}

    ~string_pool_t() { adobe::for_each(pool_m, adobe::delete_ptr()); }

    const char* add(const char* ident) {
        std::size_t n = std::strlen(ident);

        char* result(allocate(n));

        adobe::copy_n(ident, n, result);

        return result;
    }

private:
    char* allocate(std::size_t length) {
        std::size_t full_length(length + 1);

        if (full_length > std::size_t(end_m - next_m)) {
            std::size_t pool_size((std::max)(pool_size_m, full_length));

            pool_m.push_back(static_cast<char*>(::operator new(pool_size)));
            next_m = pool_m.back();
            end_m = next_m + pool_size;
        }

        char* result(next_m);

        next_m += length;
        *next_m++ = '\0';

        return result;
    }

    std::size_t pool_size_m;
    std::vector<char*> pool_m;
    char* next_m;
    char* end_m;
};

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

struct unique_string_pool_t::implementation_t {
public:
    using lock_t = std::scoped_lock<std::mutex>;
    using index_t = closed_hash_set<const char*, identity<>, str_name_hash, str_equal_to_t>;

    implementation_t() {}

    const char* add(const char* str) {
        return add(str, adobe::detail::name_hash(str, std::strlen(str)), false);
    }

    const char* add(const char* str, std::size_t hash, bool is_static) {
        if (!str || !*str)
            return detail::empty_string_s();

        auto& shard = _shards[hash % shard_count];
        auto& index = shard._index;
        lock_t lock(shard._mutex);

        if (is_static) {
            return *index.insert(shard._pool.add(str), hash).first;
        }

        index_t::const_iterator found(index.find(str, hash));

        return found == index.end() ? *index.insert(shard._pool.add(str), hash).first : *found;
    }

private:
    // The size of 8 for sharding was chosen to avoid waisting space with buckets.
    // Ps currently (2025-05-05) allocates 6 * 4K buckets at startup and most access
    // is from the main thread.
    static constexpr std::size_t shard_count = 8;

    // After some research a shard appears to be the best, simple, option to share a hashmap.
    // https://le.qun.ch/en/blog/sharding/
    struct shard {
        std::mutex _mutex;
        index_t _index;
        string_pool_t _pool;
    };

    shard _shards[shard_count];
};

/**************************************************************************************************/

unique_string_pool_t::unique_string_pool_t() : object_m(new implementation_t()) {}

unique_string_pool_t::~unique_string_pool_t() { delete object_m; }

const char* unique_string_pool_t::add(const char* str) { return object_m->add(str); }

const char* unique_string_pool_t::add(const char* str, std::size_t hash, bool is_static) { return object_m->add(str, hash, is_static); }

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
