/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <string>

#include <adobe/implementation/parser_shared.hpp>
#include <adobe/istream.hpp>
#include <adobe/name.hpp>

using namespace std;

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

void throw_parser_exception(const char *error_string, const line_position_t &position) {
    throw stream_error_t(error_string, position);
}

/*************************************************************************************************/

void throw_parser_exception(const char *expected, const char *found,
                            const adobe::line_position_t &position) {
    throw stream_error_t(string() + "Expected \"" + expected + "\", Found \"" + found + "\"",
                         position);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
