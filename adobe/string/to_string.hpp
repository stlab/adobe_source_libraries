/*
    Copyright 2012 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_STRING_TO_STRING_HPP
#define ADOBE_STRING_TO_STRING_HPP

#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <string>

#include <adobe/cassert.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

/*!
    \ingroup string_algorithm

    \brief Convert double precision floating point numbers to ascii representation.

    \c to_string() will output x as a sequence of characters (not null terminated) to \c out.
    \c snprintf() is used with the formatting string \c "%.*g" where the precision determined by
    \c precise.

    If \c precise is false (the default) then at most 15 significant digits will be output. This
    allows strings to be round-tripped (so long as they have 15 or fewer significant digits)
    without error. However, values with more than 15 significant digits will not roundtrip.

    To illustrate the following asserts hold:

        char buf[32] = { 0 };
        to_string(atof("0.1"), &buf[0], false);  // buf contains "0.1"
        assert(strcmp(&buf[0], "0.1") == 0);     // result is equal to source string "0.1"

        char buf[32] = { 0 };
        to_string(DBL_MAX, &buf[0], false);      // buf contains "1.79769313486232e+308"
        assert(atof(&buf[0]) != DBL_MAX);        // result is not equal to source value DBL_MAX

    If precise is true then at most 17 significant digits will be output. This is sufficient to
    fully represent any double precision value. However, values that cannot be represented
    precisely as a double will appear in error.

        char buf[32] = { 0 };
        to_string(atof("0.1"), &buf[0], true);   // buf contains 0.10000000000000001
        assert(strcmp(&buf[0], "0.1") != 0);     // result is not equal to source string "0.1"

        char buf[32] = { 0 };
        to_string(DBL_MAX, &buf[0], true);       // buf contains 1.7976931348623157e+308
        assert(atof(&buf[0]) == DBL_MAX);        // result is equal to source value DBL_MAX

    \c to_string() returns an updated copy of \c out. The number of characters output will be at
    least 1 and not more that 24 if \c precise and 22 otherwise.

    If \c isinf(x) or \c isnan(x) then the results are undefined (and vary by platform).

    Example output:

        42
        12.536
        -20.5
        -1.375e+112
        3.14159265358979
        -1.7976931348623157e+308     // -DBL_MAX precise (max length value)

    \param x value to be converted
    \param out models output iterator accepting type char
    \param precise exact representation of \c x, default is inexact but roundtrips decimal values
        (see discussion above).

    \return updated \c out

    \note
        <http://www.cs.berkeley.edu/~wkahan/ieee754status/IEEE754.PDF>
*/

template <typename O> // O models output iterator accepting type char
O to_string(double x, O out, bool precise = false) {
    ADOBE_ASSERT((-DBL_MAX <= x && x <= DBL_MAX) &&
                 "WARNING (sparent) : to_string() only supports finite values.");

    /*
        longest possible string result is:
             1 "-"
             1 digit (will not be 0)
             1 "."
            16 digits (precise, 14 digits otherwise).
             2 "e+" or "e-"
             3 digit exponent
             1 null
          -------------------
          = 25 characters
        Use 32 as the next power-of-two for some safety with no overhead.
    */
    char buf[32];

#if defined(_MSC_VER)
    int size = _snprintf_s(&buf[0], sizeof(buf), sizeof(buf), "%.*g", precise ? 17 : 15, x);
#else
    int size = std::snprintf(&buf[0], sizeof(buf), "%.*g", precise ? 17 : 15, x);
    ADOBE_ASSERT(size < static_cast<int>(sizeof(buf)) && "WARNING (sparent) : snprintf truncated.");
#endif
    ADOBE_ASSERT(size < (precise ? 25 : 23) &&
                 "WARNING (sparent) : to_string() result larger than expected.");
    ADOBE_ASSERT(0 < size && "FATAL (sparent) : snprintf failed in to_string().");

    return std::copy(&buf[0], &buf[0] + size, out);
}

/**************************************************************************************************/
/*!
    \ingroup string_algorithm

    \brief Convert double precision floating point numbers to ascii representation.

    \return A `std::string` with the representation, similar to the `std::to_string` routines.

    Google's `double-conversion` library was originally used by ASL to serialize floating-point
    values (mostly indirectly when serializing `any_regular_t`s that held a double.) In an effort
    to reduce our dependencies, we have removed ASL's use of `double-conversion` in favor of this
    variant of `adobe::to_string`, which itself is a light wrapping around `std::to_chars`.

    `double-conversion`'s serialization class (`DoubleToStringConverter`) is highly configurable.
    There is a comment detailing the all the knobs one can twist and their effects here:

        https://github.com/google/double-conversion/blob/4f7a25d8ced8c7cf6eee6fd09d6788eaa23c9afe/double-conversion/double-to-string.h#L86-L164

    `double-conversion` also comes with a handful of predefined settings for these knobs to make it
    easier for library consumers to serialize floating-point values consistently. ASL used one of
    these, called `EcmaScriptConverter`, and would then invoke its `ToShortest` API. The knob
    values for the `EcmaScriptConverter` are defined here:

        https://github.com/google/double-conversion/blob/4f7a25d8ced8c7cf6eee6fd09d6788eaa23c9afe/double-conversion/double-to-string.cc#L42-L51

    `EcmaScriptConverter` sets the `DoubleToStringConverter` knobs a way that is not exactly
    reproducible by `std::to_chars`. `adobe::to_string` gets us as close as possible to
    `EcmaScriptConverter` without inundating the implementation with special cases. (There are
    special cases for nan and +/-infinity so this variant's output matches `EcmaScriptConverter`'s
    for those values.) The ways in which `EcmaScriptConverter` and `adobe::to_string` still differ
    are considered to be acceptable tradeoffs in light of the eliminated dependency. Some of those
    differences can be seen in ASL's `to_string` tests.
*/
std::string to_string(double x);

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
