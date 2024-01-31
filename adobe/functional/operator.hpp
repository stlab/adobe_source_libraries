/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_FUNCTIONAL_OPERATOR_HPP
#define ADOBE_FUNCTIONAL_OPERATOR_HPP

#include <adobe/config.hpp>

#include <functional>
#include <typeinfo>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

/*! \addtogroup misc_functional
@{
*/
struct equal_to {
    typedef bool result_type;

    template <typename T> // T models Regular
    bool operator()(const T& x, const T& y) const {
        return std::equal_to<T>()(x, y);
    }
};

struct not_equal_to {
    typedef bool result_type;

    template <typename T> // T models Regular
    bool operator()(const T& x, const T& y) const {
        return std::not_equal_to<T>()(x, y);
    }
};

struct greater {
    typedef bool result_type;

    template <typename T> // T models Regular
    bool operator()(const T& x, const T& y) const {
        return std::greater<T>()(x, y);
    }
};

struct less {
    typedef bool result_type;

    template <typename T> // T models Regular
    bool operator()(const T& x, const T& y) const {
        return typename std::less<T>()(x, y);
    }

    template <typename T, typename U> // T models Regular
    bool operator()(const T& x, const U& y) const {
        return x < y;
    }

    bool operator()(const std::type_info& x, const std::type_info& y) { return x.before(y) != 0; }
};

struct greater_equal {
    typedef bool result_type;

    template <typename T> // T models Regular
    bool operator()(const T& x, const T& y) const {
        return std::greater_equal<T>()(x, y);
    }
};

struct less_equal {
    typedef bool result_type;

    template <typename T> // T models Regular
    bool operator()(const T& x, const T& y) const {
        return std::less_equal<T>()(x, y);
    }
};

struct logical_and {
    typedef bool result_type;

    template <typename T> // T models Regular
    bool operator()(const T& x, const T& y) const {
        return std::logical_and<T>()(x, y);
    }
};

struct logical_or {
    typedef bool result_type;

    template <typename T> // T models Regular
    bool operator()(const T& x, const T& y) const {
        return std::logical_or<T>()(x, y);
    }
};

struct logical_not {
    typedef bool result_type;

    template <typename T> // T models Regular
    bool operator()(const T& x) const {
        return std::logical_not<T>()(x);
    }
};

// `assign` is a function object type for the assignment operator.
// `assign{}(x, r)` is equivalent to `(void)(r = x)`
struct assign {
    typedef void result_type;

    template <class T, class U>
    void operator()(T&& x, U& r) {
        r = std::forward<T>(x);
    }
};

/**************************************************************************************************/

template <typename T> // T models Regular
struct pointer_to {
    typedef T* result_type;

    T* operator()(T& x) const { return &x; }
};

/**************************************************************************************************/

template <typename T>
struct identity {
    typedef T& result_type;

    T& operator()(T& x) const { return x; }
};

/**************************************************************************************************/

/*!

\brief A function object for type \c T that invokes \c delete on a \c T*.

*/

struct delete_ptr {
    typedef void result_type;

    template <typename T>
    void operator()(const T* x) const {
        delete x;
    }
};

/*!

\brief A function object for type \c T that invokes \c delete[] on an array of \c T.

*/

struct delete_array {
    typedef void result_type;

    template <typename T>
    void operator()(const T* x) const {
        delete[] x;
    }
};

/**************************************************************************************************/

template <class T>
struct constructor {
    typedef T result_type;

    T operator()() const { return T(); }

    template <class A1>
    T operator()(const A1& a1) const {
        return T(a1);
    }

    template <class A1, class A2>
    T operator()(const A1& a1, const A2& a2) const {
        return T(a1, a2);
    }

    template <class A1, class A2, class A3>
    T operator()(const A1& a1, const A2& a2, const A3& a3) const {
        return T(a1, a2, a3);
    }

    template <class A1, class A2, class A3, class A4>
    T operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4) const {
        return T(a1, a2, a3, a4);
    }

    template <class A1, class A2, class A3, class A4, class A5>
    T operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) const {
        return T(a1, a2, a3, a4, a5);
    }
};

/**************************************************************************************************/

template <typename T> // T models Regular
struct indirect {
    typedef T& result_type;

    template <typename P> // models TrivialIterator where value_type(P) == T
    T& operator()(P x) const {
        return *x;
    }
};

/**************************************************************************************************/

template <class T>
struct bitwise_or {
    T operator()(const T& x, const T& y) const { return x | y; }
};

/**************************************************************************************************/

template <class T>
struct bitwise_and {
    T operator()(const T& x, const T& y) const { return x & y; }
};

/**************************************************************************************************/

template <class T>
struct bitwise_xor {
    T operator()(const T& x, const T& y) const { return x ^ y; }
};

/**************************************************************************************************/

/// \brief plus function object whose arguments may be of different type.
template <typename T1, typename T2>
struct plus_asymmetric {
    T1 operator()(T1 f1, T2 f2) const { return f1 + f2; }
};

/**************************************************************************************************/

/// \brief operator++ wrapped in a function object
template <typename T>
struct inc {
    T operator()(T x) const { return ++x; }
};

/**************************************************************************************************/

/// \brief operator-- wrapped in a function object
template <typename T>
struct dec {
    T operator()(T x) const { return --x; }
};

/**************************************************************************************************/

/// \brief typeid(x) wrapped in a function object
struct typeid_ {
    typedef std::type_info result_type;

    template <typename T>
    const result_type& operator()(T) const {
        return typeid(T);
    }
};

//!@}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
