/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_CONVERSION_HPP
#define ADOBE_CONVERSION_HPP

#include <adobe/config.hpp>
#include <string>

#include <adobe/string_fwd.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

template <typename T>
struct promote
{
    typedef T type;
};

template <> struct promote<short>           { typedef double type; };
template <> struct promote<int>             { typedef double type; };
template <> struct promote<long>            { typedef double type; };

template <> struct promote<unsigned short>  { typedef double type; };
template <> struct promote<unsigned int>    { typedef double type; };
template <> struct promote<unsigned long>   { typedef double type; };

template <> struct promote<float>           { typedef double type; };

// Under 64-bit Windows size_t and ptrdiff_t do not fall into the categories above,
// and can cause bad cast failures in any_regular_t when one presumes it would, as
// is the case in a 32-bit environment. By default, then, we emit an error when an
// attempt is made to promote these types. You can turn off the error by defining
// ADOBE_NO_MSVC64_PROMOTION_ERROR in your project.
#if defined(BOOST_MSVC) && defined(_M_X64) && !defined(ADOBE_NO_MSVC64_PROMOTION_ERROR)
template <> struct promote<std::size_t>    { };
template <> struct promote<std::ptrdiff_t> { };
#endif

template <> struct promote<const char*>     { typedef version_1::string_t type; };
template <> struct promote<std::string>     { typedef version_1::string_t type; };

/*************************************************************************************************/

template <typename lht, typename rht>
inline lht explicit_cast(const rht& rhs)
{ return static_cast<lht>(rhs); }

/*************************************************************************************************/

template <typename R, typename T>
struct runtime_cast_t {
    R operator()(T& x) const { return dynamic_cast<R>(x); }
};

template <typename R, typename T>
inline R runtime_cast(T& x)
{ return runtime_cast_t<R, T>()(x); }

template <typename R, typename T>
inline R runtime_cast(T* x)
{ return runtime_cast_t<R, T*>()(x); }

template <typename R, typename T>
inline bool runtime_cast(const T& x, R& r)
{
    const R* p = runtime_cast<const R*>(&x);
    if (!p) return false;
    r = *p;
    return true;
}

/*************************************************************************************************/

template <typename T>
inline T& remove_const(const T& x)
{ return const_cast<T&>(x); }

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
