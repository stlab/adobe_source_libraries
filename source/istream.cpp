/*
    (c) Copyright 2005-2008 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)
*/
// $Id: $
// $DateTime: $
// $Change: $
// $Author: $

/**************************************************************************************************/

#include <iomanip>
#include <ostream>
#include <sstream>

#include <adobe/istream.hpp>
#include <adobe/name.hpp>

using namespace std;
using namespace adobe;

/**************************************************************************************************/

namespace {

auto format_snippet(ostream& out, const line_position_t& position) -> ostream& {
    std::string line_string(position.file_snippet());
    if (line_string.empty())
        return out;

    // Replace any tabs with spaces
    std::replace(line_string.begin(), line_string.end(), '\t', ' ');

    // Count any leading spaces
    std::string::size_type leading_spaces(line_string.find_first_not_of(' '));

    // Trim leading spaces
    if (leading_spaces != std::string::npos)
        line_string.erase(0, leading_spaces);

    // Trim trailing nulls (sean-parent: why?)
    std::string::size_type trailing_nulls(line_string.find_last_not_of((char)0));

    if (trailing_nulls != std::string::npos)
        line_string.erase(trailing_nulls + 1);

    // Determine the caret position
    auto width = (position.position_m == streampos(-1))
                     ? streampos(std::streamoff(line_string.size()))
                     : streampos(position.position_m - position.line_start_m);

    width -= std::streamoff(leading_spaces);

    size_t width_int(static_cast<size_t>(std::streamoff(width))); // convert to size_t

    out << line_string << '\n';
    out << std::setw(width_int) << '^' << '\n';
    return out;
}

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

std::ostream& operator<<(std::ostream& result, const line_position_t& position) {
    if (position.stream_name() && *position.stream_name()) {
        result << position.stream_name();
    } else {
        result << "<unknown>";
    }

    result << ':' << position.line_number_m << ':'
           << (position.position_m - position.line_start_m + 1);

    return result;
}

/**************************************************************************************************/

line_position_t::line_position_t(adobe::name_t file_path, getline_proc_t getline_proc,
                                 int line_number, std::streampos line_start,
                                 std::streampos position)
    : line_number_m(line_number), line_start_m(line_start), position_m(position),
      file_name_m(file_path), getline_proc_m(getline_proc) {}

line_position_t::line_position_t(const char* stream_name, int line_number)
    : line_number_m(line_number), line_start_m(0), position_m(-1),
      file_name_m(adobe::name_t(stream_name)) {}

#if !defined(ADOBE_NO_DOCUMENTATION)
line_position_t::line_position_t() : line_number_m(1), line_start_m(0), position_m(-1) {}
#endif // !defined(ADOBE_NO_DOCUMENTATION)

/**************************************************************************************************/

std::string format_stream_error(const stream_error_t& error) {
    std::ostringstream out;

    if (!error.line_position_set().empty()) {
        out << error.line_position_set().front() << ": ";
    }

    out << "error: " << error.what() << '\n';

    format_snippet(out, error.line_position_set().front());

    for (auto iter(error.line_position_set().begin() + 1), last(error.line_position_set().end());
         iter != last; ++iter) {
        out << *iter << '\n';
        format_snippet(out, *iter);
    }

    return out.str();
}

/**************************************************************************************************/

std::ostream& operator<<(std::ostream& out, const stream_error_t& error) {
    out << format_stream_error(error);
    return out;
}

/**************************************************************************************************/

std::string format_stream_error(std::istream&, const stream_error_t& error) {
    return format_stream_error(error);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

/*
    REVISIT (t_schwinger) : This is needed to disable automatic CR-LF conversion, which seems
    to be broken in MinGW 3.2 (GCC 3.4).
*/
#ifdef __MINGW32__
extern "C" {
#include <fcntl.h>
unsigned int _CRT_fmode = _O_BINARY;
}
#endif

/**************************************************************************************************/
