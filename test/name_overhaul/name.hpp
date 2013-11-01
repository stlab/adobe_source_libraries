/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

// stdc++
#include <functional>
#include <iosfwd>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace detail {

/****************************************************************************************************/

constexpr std::size_t name_hash(const char* str,
                                std::size_t len,
                                std::size_t n,
                                std::size_t state)
{
    static_assert(sizeof(std::size_t) == 8, "std::size_t size mismatch.");

    return n+1 < len ?
               name_hash(str,
                         len,
                         n+1,
                         (state xor static_cast<std::size_t>(str[n])) * 1099511628211ULL) :
               state;
}

constexpr std::size_t name_hash(const char* str, std::size_t len)
{
    static_assert(sizeof(std::size_t) == 8, "std::size_t size mismatch.");

    return name_hash(str, len, 0, 14695981039346656037ULL);
}

template <std::size_t N>
constexpr std::size_t name_hash(const char (&str)[N])
{
    static_assert(sizeof(std::size_t) == 8, "std::size_t size mismatch.");

    return name_hash(str, N);
}

/****************************************************************************************************/

} // namespace detail

/****************************************************************************************************/

struct static_name_t;

/****************************************************************************************************/

namespace literals {

/****************************************************************************************************/

inline constexpr static_name_t operator"" _name (const char* str, std::size_t n);

/****************************************************************************************************/

} // namespace literals

/****************************************************************************************************/

struct static_name_t
{
    explicit operator bool() const;

private:
    static_name_t() = delete;

    constexpr static_name_t(const char* str, std::size_t hash) :
        string_m(str),
        hash_m(hash)
    { }

    friend struct name_t;

    friend constexpr static_name_t literals::operator"" _name (const char* str, std::size_t n);

    friend std::ostream& operator<<(std::ostream& s, const static_name_t& name);

    const char* string_m;

    std::size_t hash_m;
};

/****************************************************************************************************/

namespace literals {

/****************************************************************************************************/

inline constexpr static_name_t operator"" _name (const char* str, std::size_t n)
{
    return static_name_t{str, detail::name_hash(str, n+1)};
}

/****************************************************************************************************/

} // namespace literals

/****************************************************************************************************/

struct name_t
{
    name_t(const char* s) :
        ptr_m(map_string(s))
    { }

    name_t(const static_name_t& aggregate) :
        ptr_m(map_string(aggregate.string_m, aggregate.hash_m))
    { }

    friend std::ostream& operator<<(std::ostream& s, const name_t& name);

    explicit operator bool() const;

    friend bool operator==(const name_t& x, const name_t& y)
    { return x.ptr_m == y.ptr_m || std::strcmp(x.ptr_m, y.ptr_m) == 0; }

    friend bool operator!=(const name_t& x, const name_t& y)
    { return !(x == y); }

    friend bool operator<(const name_t& x, const name_t& y)
    { return std::strcmp(x.ptr_m, y.ptr_m) < 0; }

    const char* c_str() const
    { return ptr_m; }

    /**
    for use with sorting, e.g.:

        std::sort(begin(c), end(c), adobe::name_t::fast_sort);
    
    The implicit sort (operator<) is lexicographical ("slow"), whereas fast
    sort leverages the runtime hash of the name_t to speed things up. The sort
    order is *not* guaranteed between processes or DLLs, nor is it guaranteed to
    be lexicographical.

    It is only guaranteed to be stable for the lifetime of the process.
    */
    static inline bool fast_sort(const name_t& x, const name_t& y)
    { return hash(x) < hash(y); }

private:
    friend std::hash<name_t>;

    static inline std::size_t hash(const name_t& x)
    { return reinterpret_cast<std::size_t>(x.ptr_m); }

    static const char* map_string(const char* str);
    static const char* map_string(const char* str, std::size_t hash);

    const char* ptr_m;
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

namespace std {

/****************************************************************************************************/

template<>
struct hash<adobe::name_t>
{
public:
    inline std::size_t operator()(adobe::name_t const& name) const 
    {
        return adobe::name_t::hash(name);
    }
};

/****************************************************************************************************/

} // namespace std

/****************************************************************************************************/
