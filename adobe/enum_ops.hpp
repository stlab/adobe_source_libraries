/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_ENUM_OPS_HPP
#define ADOBE_ENUM_OPS_HPP

/*************************************************************************************************/
#include <type_traits>

#include <adobe/config.hpp>

/*************************************************************************************************/

/*!
    \defgroup enum_ops Typesafe Integers and Bit Fields (enums)
    \ingroup utility

    \section Description Description

    \c enum_ops provides optional typesafe bitset and arithmetic operations for enumeration types.
    Without these typesafe operations, the compiler will promote the operand(s) to the appropriate
    integral type, and the result will be an integral type. When the typesafe operations have been
    defined for an enumeration type, \c E, the result will be of type \c E exactly when the
    operand(s) are of type \c E.

    \c ADOBE_DEFINE_BITSET_OPS(E) enables the bitset operations <code>~, |, &, ^, |=, &=, ^= </code>
    for enumeration type \c E.

    \c ADOBE_DEFINE_ARITHMETIC_OPS(E) enables the typesafe arithmetic operations <code>+, -, *, /,
    %, +=, *=, -=, /=, \%=</code> for enumeration type \c E.

    \section Definition Definition

    Defined in \link enum_ops.hpp <code>adobe/enum_ops.hpp</code> \endlink

    \section Example Example

    The following is an example of code that will compile:
    \dontinclude enum_ops_example.cpp
    \skip start_of_example
    \until end_of_example

    The following is contains an example of code that will not compile
    since the typesafe operators have not been defined.

    \dontinclude enum_ops_example_fail.cpp
    \skip start_of_example
    \until end_of_example
*/

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

namespace implementation {

/*************************************************************************************************/
#if !defined(ADOBE_NO_DOCUMENTATION)
inline signed char promote_enum(signed char e) { return e; }
inline unsigned char promote_enum(unsigned char e) { return e; }
inline signed short promote_enum(signed short e) { return e; }
inline unsigned short promote_enum(unsigned short e) { return e; }
inline signed int promote_enum(signed int e) { return e; }
inline unsigned int promote_enum(unsigned int e) { return e; }
inline signed long promote_enum(signed long e) { return e; }
inline unsigned long promote_enum(unsigned long e) { return e; }

#ifdef BOOST_HAS_LONG_LONG
inline signed long long promote_enum(signed long long e) { return e; }
inline unsigned long long promote_enum(unsigned long long e) { return e; }
#endif
#endif
/*************************************************************************************************/

} // namespace implementation

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
constexpr auto enable_bitmask_enum(...) -> std::false_type;

template<typename T> struct is_enabled_bitmask
{
 	static constexpr bool enable = decltype(enable_bitmask_enum(std::declval<T>()))::value;
};

template <typename T>

constexpr auto operator&(const T lhs,const T rhs) ->
    std::enable_if_t<is_enabled_bitmask<T>::enable, T>
{
    using underlying =  std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template <typename T>
constexpr auto operator~(const T lhs) ->
    std::enable_if_t<is_enabled_bitmask<T>::enable, T>
{
	using underlying = std::underlying_type_t<T>;
    return static_cast<T>(
        ~static_cast<underlying>(lhs));
}

template <typename T>
constexpr auto operator|(const T lhs, const T rhs) ->
    std::enable_if_t<is_enabled_bitmask<T>::enable, T>
{
    using underlying =  std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template <typename T>
constexpr auto operator^(const T lhs, const T rhs) ->
    std::enable_if_t<is_enabled_bitmask<T>::enable, T>
{
    using underlying =  std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}

template <typename T>
constexpr auto operator^=(T& lhs, const T rhs) ->
    std::enable_if_t<is_enabled_bitmask<T>::enable, T>
{
	return lhs = lhs ^ rhs;
}

template <typename T>
constexpr auto operator&=(T& lhs, const T rhs) ->
    std::enable_if_t<is_enabled_bitmask<T>::enable, T>
{
	return lhs = lhs & rhs;
}

template <typename T>
constexpr auto operator|=(T& lhs, const T rhs) ->
    std::enable_if_t<is_enabled_bitmask<T>::enable, T>
{
	return lhs = lhs | rhs;
}
//this exist to mantain backwards compatability with the old ops
#define ADOBE_DEFINE_BITSET_OPS(EnumType)                                                     \
constexpr auto enable_bitmask_enum(EnumType) -> std::true_type;

/*************************************************************************************************/

#define ADOBE_DEFINE_ARITHMETIC_OPS(EnumType)                                                      \
    inline EnumType operator+(EnumType a) {                                                        \
        return EnumType(+adobe::implementation::promote_enum(a));                                  \
    }                                                                                              \
    inline EnumType operator-(EnumType a) {                                                        \
        return EnumType(-adobe::implementation::promote_enum(a));                                  \
    }                                                                                              \
    inline EnumType operator+(EnumType lhs, EnumType rhs) {                                        \
        return EnumType(adobe::implementation::promote_enum(lhs) +                                 \
                        adobe::implementation::promote_enum(rhs));                                 \
    }                                                                                              \
    inline EnumType operator-(EnumType lhs, EnumType rhs) {                                        \
        return EnumType(adobe::implementation::promote_enum(lhs) -                                 \
                        adobe::implementation::promote_enum(rhs));                                 \
    }                                                                                              \
    inline EnumType operator*(EnumType lhs, EnumType rhs) {                                        \
        return EnumType(adobe::implementation::promote_enum(lhs) *                                 \
                        adobe::implementation::promote_enum(rhs));                                 \
    }                                                                                              \
    inline EnumType operator/(EnumType lhs, EnumType rhs) {                                        \
        return EnumType(adobe::implementation::promote_enum(lhs) /                                 \
                        adobe::implementation::promote_enum(rhs));                                 \
    }                                                                                              \
    inline EnumType operator%(EnumType lhs, EnumType rhs) {                                        \
        return EnumType(adobe::implementation::promote_enum(lhs) %                                 \
                        adobe::implementation::promote_enum(rhs));                                 \
    }                                                                                              \
    inline EnumType& operator+=(EnumType& lhs, EnumType rhs) { return lhs = lhs + rhs; }           \
    inline EnumType& operator-=(EnumType& lhs, EnumType rhs) { return lhs = lhs - rhs; }           \
    inline EnumType& operator*=(EnumType& lhs, EnumType rhs) { return lhs = lhs * rhs; }           \
    inline EnumType& operator/=(EnumType& lhs, EnumType rhs) { return lhs = lhs / rhs; }           \
    inline EnumType& operator%=(EnumType& lhs, EnumType rhs) { return lhs = lhs % rhs; }

/*************************************************************************************************/

#endif

/*************************************************************************************************/
