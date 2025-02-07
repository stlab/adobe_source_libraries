/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_CONVERSION_HPP
#define ADOBE_CONVERSION_HPP

#include <adobe/config.hpp>
#include <string>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

template <class T>
struct promote {
    using type = std::
        conditional_t<std::is_arithmetic_v<T> && (2 <= sizeof(T) && sizeof(T) <= 4), double, T>;
};

template <>
struct promote<const char*> {
    using type = std::string;
};

template <class T>
using promote_t = typename promote<T>::type;

/**************************************************************************************************/

template <typename lht, typename rht>
inline lht explicit_cast(const rht& rhs) {
    return static_cast<lht>(rhs);
}

/**************************************************************************************************/

template <typename R, typename T>
struct runtime_cast_t {
    R operator()(T& x) const { return dynamic_cast<R>(x); }
};

template <typename R, typename T>
inline R runtime_cast(T& x) {
    return runtime_cast_t<R, T>()(x);
}

template <typename R, typename T>
inline R runtime_cast(T* x) {
    return runtime_cast_t<R, T*>()(x);
}

template <typename R, typename T>
inline bool runtime_cast(const T& x, R& r) {
    const R* p = runtime_cast<const R*>(&x);
    if (!p)
        return false;
    r = *p;
    return true;
}

/**************************************************************************************************/

template <typename T>
inline T& remove_const(const T& x) {
    return const_cast<T&>(x);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
