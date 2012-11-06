/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <adobe/adam.hpp>
#include <adobe/adam_evaluate.hpp>
#include <adobe/adam_parser.hpp>
#include <adobe/any_regular.hpp>
#include <adobe/implementation/expression_parser.hpp>
#include <adobe/name.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/virtual_machine.hpp>

#include <adobe/iomanip_asl_cel.hpp>

#include <iostream>

/****************************************************************************************************/

namespace bfs = boost::filesystem;

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

struct option_result_t
{
    std::string directory_m;
    std::string sheet_m;
};

/****************************************************************************************************/

void usage(const std::string& app_name)
{
    std::cout << "\nAdam smoke test app v" << ADOBE_VERSION_MAJOR << "." << ADOBE_VERSION_MINOR << "." << ADOBE_VERSION_SUBMINOR << "\n\n";
    std::cout << "usage: " << app_name << " [ root_path_name ]\n\n";
    std::cout << "\troot_path_name specifies the root name of an Adam sheet/Known good file pair. The Adam\n";
    std::cout << "\tsheet contains the model we would like to test, and the known good file contains\n";
    std::cout << "\tcontributing and resulting information. When the contributing information is passed\n";
    std::cout << "\tthrough the model, the known good resultant information should match the result. If the results\n";
    std::cout << "\tmatch, the app will return 0, 1 otherwise.\n";
    std::cout << "\n";
}

/****************************************************************************************************/

adobe::dictionary_t read_dictionary(const bfs::path& filepath)
{
    std::ifstream               input_file(filepath.native_file_string().c_str());
    adobe::expression_parser    parser(input_file, adobe::line_position_t("input dictionary"));
    adobe::array_t              expression;

    if (!input_file.is_open())
    {
        std::cout << "Could not open \"" << filepath.native_file_string() << "\"!\n";

        throw std::runtime_error("file error");
    }

    parser.require_expression(expression);

    adobe::virtual_machine_t machine;

    machine.evaluate(expression);

    
return machine.back().cast<adobe::dictionary_t>();
}

/****************************************************************************************************/

void read_sheet(const bfs::path& filepath, adobe::sheet_t& sheet)
{
    std::ifstream  input_file(filepath.native_file_string().c_str());

    if (!input_file.is_open())
    {
        std::cout << "Could not open \"" << filepath.native_file_string() << "\"!\n";

        throw std::runtime_error("file error");
    }

    try
    {
        // set up adam sheet
        adobe::parse(input_file, adobe::line_position_t(filepath.native_file_string().c_str()), adobe::bind_to_sheet(sheet));
    }
    catch (const adobe::stream_error_t& error)
    {
        std::cerr << "adobe:: " << ": " << format_stream_error(input_file, error) << "\n";

        throw;
    }
}

/****************************************************************************************************/

bool compare_arrays(const adobe::array_t& a, const adobe::array_t& b)
{
	/*
		NOTE (sparent) : If we have precision loss due to serialization I would like to know about
		and fix it - so we simply compare the arrays directly here.
	*/
#if 0
    /*
        This is in place to handle precision loss due to serialization --
        serialize both (so the loss is the same, ideally) and see if the
        strings are the same. Consider a more efficient means at some point.
    */

    std::stringstream a_str;
    std::stringstream b_str;

    a_str << adobe::begin_asl_cel << a << adobe::end_asl_cel;
    b_str << adobe::begin_asl_cel << b << adobe::end_asl_cel;

    return a_str.str() == b_str.str();
#endif
	return a == b;
}

/****************************************************************************************************/

bool test_sheet(const bfs::path& root)
{
    bool                	success(true);
    std::string         	triple_name(root.leaf());
    bfs::path           	sheet_path(root.branch_path() / (triple_name + ".adm"));
    bfs::path           	input_path(root.branch_path() / (triple_name + ".admi"));
    adobe::sheet_t      	sheet;
    adobe::dictionary_t 	input;
    adobe::dictionary_t 	contributing;
    adobe::any_regular_t    sheet_value;
    adobe::any_regular_t    input_value;

    // Connect the virtual machine to the sheet.
    
    sheet.machine_m.set_variable_lookup(boost::bind(&adobe::sheet_t::get, &sheet, _1));

    read_sheet(sheet_path, sheet);

    input = read_dictionary(input_path);

    // get contributing cells array
    contributing = get_value(input, adobe::static_name_t("contributing")).cast<adobe::dictionary_t>();

    // get expected value array
    input_value = get_value(input, adobe::static_name_t("value"));

    sheet.update();

    for (adobe::dictionary_t::const_iterator first(contributing.begin()), last(contributing.end()); first != last; ++first)
    {
        adobe::name_t   		cell(first->first);
        adobe::any_regular_t  	value(first->second);

        std::cout << "Setting Cell '" << cell.c_str() << "' to " << adobe::begin_asl_cel << value << adobe::end_asl_cel;

        sheet.set(cell, value);
    }

    sheet.update();

    // get calculated value array
    sheet_value = sheet.inspect(adobe::parse_adam_expression(std::string("result")));

    // compare and show interesting results
    if (compare_arrays(input_value.cast<adobe::array_t>(), sheet_value.cast<adobe::array_t>()))
    {
        std::cout << "*** PASS *** Expected value calculated!\n";
    }
    else
    {
        std::cout << "*** FAIL *** Values are different...\n";
        std::cout << "Expected Value:\n";
        std::cout << '\t' << adobe::begin_asl_cel << input_value << adobe::end_asl_cel;
        std::cout << "Calculated Value:\n";
        std::cout << '\t' << adobe::begin_asl_cel << sheet_value << adobe::end_asl_cel;

        success = false;
    }

    return success;
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

int main(int argc, char* argv[])
{
    int result(0);

    try
    {
        bfs::path sheet(bfs::path("../rtd", bfs::native));

        if (argc > 1)
        {
            if (!std::strcmp(argv[1], "--help"))
                usage(argv[0]);
            else
            {
                sheet = bfs::path(argv[1], bfs::native);

                result = !test_sheet(sheet);
            }
        }
        else
        {
            std::cout << "No sheet specified-- using test sheet...\n";

            result = !test_sheet(sheet);
        }
    }
    catch (const std::exception& error)
    {
        std::cerr << "std::exception: " << typeid(error).name() << ": " << error.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Unknown Exception\n";
    }

    return result;
}
