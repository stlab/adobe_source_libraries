/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ADAM_TEST_PARSER_HPP
#define ADOBE_ADAM_TEST_PARSER_HPP

#include <adobe/config.hpp>

#include <iosfwd>
#include <adobe/dictionary_fwd.hpp>
#include <adobe/istream_fwd.hpp>


/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

class sheet_t;

/*!
   This parses adam_test streams. The grammar lives in \ref
   adam_parser_impl.hpp. Pass the stream to parse with a line_pos, a
   sheet to hold the result, dictionaries to input/interface-in,
   output/interface-out values, and an ostream for output. Returns
   true if no checks failed.
   
 */
bool parse(std::istream& stream, line_position_t line_pos, std::ostream& messages);


/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif // ADOBE_ADAM_TEST_PARSER_HPP

/*************************************************************************************************/
