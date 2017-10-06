/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_CMATH_HPP
#define ADOBE_CMATH_HPP

#include <adobe/config.hpp>

#include <cmath>
#include <functional>
#include <limits.h>

/*************************************************************************************************/

#define ADOBE_HAS_C99_STD_MATH_H

namespace adobe {

/*************************************************************************************************/

using std::float_t;
using std::double_t;

using std::round;
using std::lround;
using std::trunc;

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

template <typename A, typename R>
struct nearest_cast_fn;

/*************************************************************************************************/

inline double round_half_up(double x) { return std::floor(x + 0.5); }

inline float round_half_up(float x) { return std::floor(x + 0.5f); }

inline long lround_half_up(double x) { return static_cast<long>(std::floor(x + 0.5)); }

inline long lround_half_up(float x) { return static_cast<long>(std::floor(x + 0.5f)); }

/*
    REVISIT (sparent) : Should complete the rounding modes by providing a round_half_even()
    function.

    Names are borrowed from the EDA rounding modes:

    <http://www.gobosoft.com/eiffel/gobo/math/decimal/>
*/

/*************************************************************************************************/

template <typename R, typename A>
inline R nearest_cast(const A& x) {
    return nearest_cast_fn<A, R>()(x);
}

/*************************************************************************************************/

template <typename A, typename R>
struct nearest_cast_fn : std::unary_function<A, R> {
    R operator()(const A& x) const { return static_cast<R>(round_half_up(x)); }
};

template <typename A>
struct nearest_cast_fn<A, float> : std::unary_function<A, float> {
    float operator()(const A& x) const { return static_cast<float>(x); }
};

template <typename A>
struct nearest_cast_fn<A, double> : std::unary_function<A, double> {
    double operator()(const A& x) const { return static_cast<double>(x); }
};

/*************************************************************************************************/

template <class T>
constexpr T bits = static_cast<T>(CHAR_BIT * sizeof(T));

template <class T>
constexpr T lower(T x) {
	static_assert(std::is_unsigned<T>::value, "T must be unsigned");
	T shift = bits<T> / T(2);
	T mask = (T(1) << shift) - T(1);
	return x & mask;
	}

template <class T>
constexpr T upper(T x) {
	static_assert(std::is_unsigned<T>::value, "T must be unsigned");
	T shift = bits<T> / T(2);
	return lower(x >> shift);
	}

// This multiplies two T and throws away the overflow in a way
// that avoids overflow complaints from Visual Studio 2017:
// Warning C4307: '*': integral constant overflow
//
template <class T>
constexpr T multiply_unsigned(const T x, T y) {
	static_assert(std::is_unsigned<T>::value, "T must be unsigned");

	T result = 0;

#if !defined(_MSC_VER)
	result = static_cast<T>(x * y);
#elif _MSC_VER > 1911
#error "Revisit this code to see if static_cast silences the integral constant overflow warning."
// And, if the bug has been fixed, and static_cast does silence the warning, track down
// all of the calls to this function and remove them, along with this function. If the bug
// remains, update the number in the #elif above to the current value of _MSC_VER.
//
#else
	// Let P = 2 to the power of bits<T>/2 and decompose x and y this way:
	// x = P*x1 + x0
	// y = P*y1 + y0
	// This makes it safe to multiply xi*yi without overflow, and
	// x*y = P*P*(x1*y1) + P*(x0*y1 + x1*y0) + (x0*y0). Ignoring overflow, we have
	//     = P*lower(lower(x0*y1) + lower(x1*y0) + upper(x0*y0)) + lower(x0*y0)

	T x0 = lower(x);
	T x1 = upper(x);
	T y0 = lower(y);
	T y1 = upper(y);
	T x0y0 = x0 * y0;
	T shift = bits<T> / T(2);
	T the_upper = lower(x0 * y1) + lower(x1 * y0) + upper(x0y0);
	result = (lower(the_upper) << shift) | lower(x0y0);
#endif

	return result;
	}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
