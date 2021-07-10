/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_NAME_HPP
#define ADOBE_NAME_HPP

/**************************************************************************************************/

// stdc++
#include <cstring>
#include <functional>
#include <iosfwd>

// boost
#include <boost/mpl/bool.hpp>
#include <boost/operators.hpp>
#include <boost/type_traits/is_pod.hpp>

// asl
#include <adobe/conversion.hpp>
#include <adobe/cstring.hpp>
#include <adobe/fnv.hpp>

/**
    \defgroup name name_t and static_name_t

    \details
    name_t is holding onto a `std::unordered_map` on the backside that maps from
    a `name_t`'s hash to its string. When a `name_t` is constructed, its hash is
    used to find the unique string pointer common to all `name_t`s with that
    same string.

    \par
    Although static_name_t and name_t are related, their hash values are quite
    different. static_name_t's hash is of its *string*, whereas name_t's hash
    is a `reinterpret_cast` of its unique string pointer in the underlying
    string map. Both hashes will point to the same unique string in the string
    map, making for very fast conversion from a static_name_t to a name_t.
*/
/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

namespace detail {

/**************************************************************************************************/

constexpr std::size_t sizesz_k = sizeof(std::size_t);

constexpr bool sizeok_k = sizesz_k == 8 || sizesz_k == 4;

constexpr std::size_t name_fnv_prime_k =
    sizesz_k == 8 ? static_cast<std::size_t>(0x100000001b3) : static_cast<std::size_t>(0x1000193);

constexpr std::size_t name_fnv_basis_k = sizesz_k == 8
                                             ? static_cast<std::size_t>(0xcbf29ce484222325ULL)
                                             : static_cast<std::size_t>(0x811c9dc5);

constexpr std::size_t name_hash(const char* str, std::size_t len, std::size_t n,
                                std::size_t state) {
    static_assert(sizeok_k, "Unknown sizeof std::size_t (must be 4 or 8).");

    return n < len ? name_hash(str, len, n + 1,
                               (state xor static_cast<std::size_t>(str[n])) * name_fnv_prime_k)
                   : state;
}

constexpr std::size_t name_hash(const char* str, std::size_t len) {
    static_assert(sizeok_k, "Unknown sizeof std::size_t (must be 4 or 8).");

    return name_hash(str, len, 0, name_fnv_basis_k);
}

template <std::size_t N>
constexpr std::size_t name_hash(const char (&str)[N]) {
    static_assert(sizeok_k, "Unknown sizeof std::size_t (must be 4 or 8).");

    return name_hash(str, N - 1);
}

/**************************************************************************************************/

} // namespace detail

/**************************************************************************************************/

struct static_name_t;

/**************************************************************************************************/

namespace literals {

/**************************************************************************************************/

inline constexpr static_name_t operator"" _name(const char* str, std::size_t n) noexcept;

/**************************************************************************************************/

} // namespace literals

/**************************************************************************************************/

using namespace literals;

/**************************************************************************************************/
/**
    \ingroup name

    A compile-time precursor to name_t.

    This type is especially useful for setting up keys in advance, as work is
    done at compile-time to make conversion from a static_name_t to a name_t to
    be faster than creating a name_t at runtime. static_name_t does no work at
    runtime, instead leveraging c++11's `constexpr` feature to precompute token
    string hash values for immediate insertion into the underlying name table.

    \note
    You *cannot* create a `static_name_t` any way other than through the user
    defined literal. They can only be created at compile-time.

    \complexity
    The first time a static_name_t with a particular string is converted to a
    name_t is a `O(log N)` operation. Thereafter conversion of that same literal
    from a static_name_t to a name_t is on average `O(1)`.

    \promotes_to
        name_t
*/
struct static_name_t {
    /**
        \return
            \false iff the instance is equal to the empty string.

        \complexity
            O(1)
    */
    explicit operator bool() const;

    friend bool operator==(const static_name_t& x, const static_name_t& y) {
        return x.hash_m == y.hash_m;
    }

    friend bool operator!=(const static_name_t& x, const static_name_t& y) { return !(x == y); }

    friend bool operator<(const static_name_t& x, const static_name_t& y);

private:
    static_name_t() = delete;

    constexpr static_name_t(const char* str, std::size_t hash) : string_m(str), hash_m(hash) {}

    friend struct name_t;

    friend constexpr static_name_t literals::operator"" _name(const char* str, std::size_t n) noexcept;

    friend std::ostream& operator<<(std::ostream& s, const static_name_t& name);

    const char* string_m;

    std::size_t hash_m;
};

/**************************************************************************************************/

namespace literals {

/**************************************************************************************************/
/**
    \ingroup name

    User defined literal to convert a compile-time string into a static_name_t.
    It has been defined in the adobe::literals namespace to avoid namespace
    pollution.

    \example

        using namespace adobe::literals;

        adobe::static_name_t foo("foo"_name); // OK, creation via user defined literal
        adobe::name_t        bar(foo); // OK, implicit promotion to a name_t
        adobe::name_t        baz("baz"_name); // OK, creation and then promotion

    \par
    The `adobe` namespace itself uses the `literals` namespace, so you do not
    need to specify `using namespace adobe::literals` if you are already
    `using namespace adobe`:

    \par

        using namespace adobe;

        static_name_t foo("foo"_name); // OK
        name_t        bar("bar"_name); // OK
*/
inline constexpr static_name_t operator"" _name(const char* str, std::size_t n) noexcept {
    return static_name_t{str, detail::name_hash(str, n)};
}

/**************************************************************************************************/

} // namespace literals

/**************************************************************************************************/
/**
    \ingroup name

    A name is an immutable string literal used for indexing associative
    containers (e.g., dictionary_t.) It has several performance guarantees
    that make it a preferred alternative to other string-based key types.
*/
struct name_t : boost::totally_ordered<name_t, name_t> {
    using const_iterator = const char*;
    using iterator = const char*;

    explicit name_t(const char* s = "") : ptr_m(map_string(s)) {}

    /**
        Implicit conversion constructor from a static_name_t.
    */
    name_t(const static_name_t& static_name)
        : ptr_m(map_string(static_name.string_m, static_name.hash_m)) {}

    friend std::ostream& operator<<(std::ostream& s, const name_t& name);

    /**
        \return
            \false iff the instance is equal to the empty string.

        \complexity
            O(1)
    */
    explicit operator bool() const;

    /**
        \complexity
            O(1)
    */
    friend bool operator==(const name_t& x, const name_t& y) { return x.ptr_m == y.ptr_m; }

    /**
        Lexicographical comparison of two names. For a faster comparsion
        use name_t::fast_sort.

        \complexity
            O(N)
    */
    friend bool operator<(const name_t& x, const name_t& y) {
        return std::strcmp(x.ptr_m, y.ptr_m) < 0;
    }

    const char* c_str() const { return ptr_m; }

    const char* begin() const { return ptr_m; }
    const char* end() const { return begin() + std::strlen(begin()); }

    /**
        for use with sorting, e.g.:

            std::sort(begin(c), end(c), adobe::name_t::fast_sort);

        The implicit sort (`operator<`) is lexicographical ("slow"), whereas fast
        sort leverages the runtime hash of the name_t to speed things up. The sort
        order is *not* guaranteed between processes or DLLs, nor is it guaranteed to
        be lexicographical. It is only guaranteed to be stable for the lifetime
        of the process.

        \complexity
            O(1)
    */
    static inline bool fast_sort(const name_t& x, const name_t& y) { return hash(x) < hash(y); }

private:
    friend struct std::hash<name_t>;

    /**
        Used internally within name_t and also called as part of the std::hash<T>
        specialization.

        \complexity
            O(N)
    */
    static inline std::size_t hash(const name_t& x) {
        return reinterpret_cast<std::size_t>(x.ptr_m);
    }

    static const char* map_string(const char* str);
    static const char* map_string(const char* str, std::size_t hash);

    const char* ptr_m;
};

/**************************************************************************************************/
/**
    \ingroup name

    Template specialization of promote<T> for static_name_t.

    This ensures a static_name_t instance will be converted to a name_t
    as it is being assigned into an any_regular_t.
*/
template <>
struct promote<static_name_t> {
    typedef name_t type;
};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

namespace std {

/**************************************************************************************************/
/**
    \ingroup name

    Template specialization of std::hash<T> for adobe::name_t.

    \complexity
        O(1)
*/
template <>
struct hash<adobe::name_t> {
public:
    inline std::size_t operator()(adobe::name_t const& name) const {
        return adobe::name_t::hash(name);
    }
};

/**
    \ingroup name

    Template specialization of std::hash<T> for adobe::static_name_t. Promotes
    to a name_t and uses its hash.

    \complexity
        O(1)
*/
template <>
struct hash<adobe::static_name_t> {
public:
    inline std::size_t operator()(adobe::static_name_t const& name) const {
        return std::hash<adobe::name_t>()(name);
    }
};

/**************************************************************************************************/

} // namespace std

/**************************************************************************************************/

namespace boost {

/**************************************************************************************************/

/**
    \ingroup name

    Template specialization of boost::is_pod<T> for adobe::name_t.
*/
template <>
struct is_pod<adobe::name_t> : boost::mpl::true_ {};

/**************************************************************************************************/

} // namespace boost

/**************************************************************************************************/

#endif

/**************************************************************************************************/
