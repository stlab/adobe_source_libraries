/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/implementation/parser_shared.hpp>
#include <adobe/istream.hpp>
#include <adobe/name.hpp>
#include <adobe/string.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

void throw_parser_exception(const char*                     error_string,
                            const adobe::line_position_t&   position)
{
    throw adobe::stream_error_t(error_string, position);
}

/*************************************************************************************************/

void throw_parser_exception(const char*                     expected,
                            const char*                     found,
                            const adobe::line_position_t&   position)
{
    std::string error_string;

    error_string << "Expected \"" << expected << "\", Found \"" << found << "\"";

    throw adobe::stream_error_t(error_string, position);
}
    
/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
