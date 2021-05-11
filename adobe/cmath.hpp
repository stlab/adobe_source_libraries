/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_CMATH_HPP
#define ADOBE_CMATH_HPP

#include <adobe/config.hpp>

#include <cmath>
#include <functional>

/**************************************************************************************************/

#define ADOBE_HAS_C99_STD_MATH_H

namespace adobe {

/**************************************************************************************************/

using std::double_t;
using std::float_t;

using std::lround;
using std::round;
using std::trunc;

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

template <typename A, typename R>
struct nearest_cast_fn;

/**************************************************************************************************/

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

/**************************************************************************************************/

template <typename R, typename A>
inline R nearest_cast(const A& x) {
    return nearest_cast_fn<A, R>()(x);
}

/**************************************************************************************************/

template <typename A, typename R>
struct nearest_cast_fn {
    R operator()(const A& x) const { return static_cast<R>(round_half_up(x)); }
};

template <typename A>
struct nearest_cast_fn<A, float> {
    float operator()(const A& x) const { return static_cast<float>(x); }
};

template <typename A>
struct nearest_cast_fn<A, double> {
    double operator()(const A& x) const { return static_cast<double>(x); }
};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
