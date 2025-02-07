/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ISTREAM_FWD_HPP
#define ADOBE_ISTREAM_FWD_HPP

#include <adobe/config.hpp>

#include <ios>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

struct line_position_t;

class stream_error_t;

[[deprecated]] std::string format_stream_error(std::istream&, const stream_error_t&);

std::string format_stream_error(const stream_error_t&);

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif // ADOBE_ISTREAM_FWD_HPP

/**************************************************************************************************/
