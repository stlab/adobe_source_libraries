/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_PARSER_SHARED_HPP
#define ADOBE_PARSER_SHARED_HPP

/**************************************************************************************************/

#include <string>

#include <adobe/istream_fwd.hpp>
#include <adobe/name.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

void throw_parser_exception(const char* error_string, const line_position_t& position);
void throw_parser_exception(std::string&& error_string, const line_position_t& position);

void throw_parser_exception(const char* expected, const char* found,
                            const line_position_t& position);

inline void throw_parser_exception(name_t expected, name_t found, const line_position_t& position) {
    throw_parser_exception(expected.c_str(), found.c_str(), position);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
