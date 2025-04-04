/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <string>

#include <adobe/implementation/parser_shared.hpp>
#include <adobe/istream.hpp>
#include <adobe/name.hpp>

using namespace std;

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

void throw_parser_exception(const char* error_string, const line_position_t& position) {
    throw stream_error_t(error_string, position);
}

/**************************************************************************************************/

void throw_parser_exception(std::string&& error_string, const line_position_t& position) {
    throw stream_error_t(std::move(error_string), position);
}

/**************************************************************************************************/

void throw_parser_exception(const char* expected, const char* found,
                            const adobe::line_position_t& position) {
    throw stream_error_t(string{"expected `"} + expected + "`, found `" + found + "`.", position);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
