/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include <adobe/any_regular.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/eve.hpp>
#include <adobe/eve_evaluate.hpp>
#include <adobe/eve_parser.hpp>
#include <adobe/iomanip_asl_cel.hpp>
#include <adobe/name.hpp>
#include <adobe/unicode.hpp>

/**************************************************************************************************/

using namespace std::placeholders;

/**************************************************************************************************/

namespace {

/**************************************************************************************************/
//
// position_t assemble( ... )
//
/// This function dumps the output of the eve parser to std::cout. It probably
/// should create all of the widgets demanded by the Eve file, but we're not
/// doing that for now.
///
/// \param      name            the name of the thing to make.
/// \param      parameters      the parameters for the thing to make.
///
/// \return     an any containing the created widget.
//
adobe::eve_callback_suite_t::position_t assemble(adobe::name_t name,
                                                 const adobe::dictionary_t& parameters) {
#if defined(ADOBE_STD_SERIALIZATION)
    std::cout << adobe::begin_asl_cel << name << ' ' << parameters << adobe::end_asl_cel;
#endif

    return adobe::eve_callback_suite_t::position_t();
}

/**************************************************************************************************/

//
// void testParse( std::string fileName )
//
/// Try to parse the given filename and see what the parser throws up.
///
/// \param      fileName        the name of the file to parse.
//
void testParse(std::filesystem::path& fileName) {
    //
    // Open our input stream.
    //
    std::ifstream stream(fileName);

    if (!stream.is_open()) {
        std::stringstream err;

        err << "Could not open file: ";
        adobe::copy_utf<char>(fileName.native().begin(), fileName.native().end(),
                              std::ostream_iterator<char>(err));

        throw std::runtime_error(err.str());
    }

    //
    // Call the Eve parser.
    //

    adobe::sheet_t layout_sheet;

    const auto& native_path{fileName.native()};
    std::string path;
    adobe::copy_utf<char>(native_path.begin(), native_path.end(), std::back_inserter(path));

    adobe::parse(
        stream, adobe::line_position_t(path.c_str()),
        adobe::eve_callback_suite_t::position_t(),
        adobe::bind_layout(std::bind(&assemble, _2, _3), layout_sheet, layout_sheet.machine_m));
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

//
// int main()
//
/// Main driver, choose which file to parse and call testParse.
///
/// \param      argc    the count of arguments.
/// \param      argv    the vector containing the arguments.
///
/// \return     always zero.
//
int main(int argc, char* argv[]) {
    bool success(true);

    //
    // Try to open the filename given on the command line, otherwise
    // try to open "default.eve".
    //
    try {
        std::filesystem::path file_path("default.eve");

        if (argc > 1)
            file_path = std::filesystem::path(argv[1]);

        //
        // Call our testParse function with the selected filename.
        //

        testParse(file_path);
    } catch (const std::exception& error) {
        //
        // Oops, something didn't work out.
        //
        std::cerr << "Exception: " << error.what() << std::endl;
        success = false;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        success = false;
    }

    return success ? 0 : 1;
}
