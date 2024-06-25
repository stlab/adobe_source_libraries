/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/string/to_string.hpp>

#if defined(__cpp_lib_to_chars) >= 201611L
#include <array>
#include <charconv>
#include <system_error>
#endif

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

std::string to_string(double x) {
    if (std::isnan(x)) return "NaN";
    if (x == std::numeric_limits<double>::infinity()) return "Infinity";
    if (x == -std::numeric_limits<double>::infinity()) return "-Infinity";

#if defined(__cpp_lib_to_chars) >= 201611L
    std::array<char, 64> str;
    char* first = &str[0];
    char* last = first + str.size();
    const std::to_chars_result tcr = std::to_chars(first, last, x);

    return tcr.ec == std::errc() ?
               std::string(first, tcr.ptr - first) :
               std::make_error_code(tcr.ec).message();
#else
    std::string result;
    double f3;
    double f2 = std::modf(x, &f3);
    const auto use_precise = std::abs(std::log10(f2)) > 7;
    to_string(x, std::back_inserter(result), use_precise);
    return result;
#endif
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
