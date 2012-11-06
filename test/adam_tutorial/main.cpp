/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <iostream>
#include <map>

#include <boost/version.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <adobe/adam_evaluate.hpp>
#include <adobe/adam_parser.hpp>
#include <adobe/adam.hpp>
#include <adobe/algorithm/for_each.hpp>
#include <adobe/any_regular.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/implementation/expression_parser.hpp>
#include <adobe/iomanip_asl_cel.hpp>
#include <adobe/name.hpp>
#include <adobe/virtual_machine.hpp>

/****************************************************************************************************/

namespace bfs = boost::filesystem;

/****************************************************************************************************/

typedef std::map<adobe::name_t, adobe::any_regular_t> cell_set_t;

/****************************************************************************************************/

void simple_display(cell_set_t::value_type& cell_m, const adobe::any_regular_t& new_value)
{
    cell_m.second = new_value;

    std::cout << "U \'" << cell_m.first.c_str() << "\' ==> "
              << adobe::begin_asl_cel << cell_m.second << adobe::end_asl_cel
              << std::endl;
}
  

/****************************************************************************************************/

void stream_cell_state(const cell_set_t::value_type& cell)
{
    std::cout << "   '" << cell.first.c_str() << "': "
              << adobe::begin_asl_cel << cell.second << adobe::end_asl_cel
              << std::endl;
}

/****************************************************************************************************/

adobe::dictionary_t parse_input_dictionary(const bfs::path& input_path)
{
    std::string              path_str(input_path.native_file_string());
    std::ifstream            input_stream(path_str.c_str());
    adobe::array_t           token_stream;
    adobe::virtual_machine_t vm;

    if (!adobe::expression_parser(input_stream,
                                  adobe::line_position_t("input_dictionary")).is_expression(token_stream))
        return adobe::dictionary_t();

    vm.evaluate(token_stream);

    const adobe::dictionary_t result(vm.back().cast<adobe::dictionary_t>());

    vm.pop_back();

    std::cout << "--" << std::endl
              << "Initializing sheet with the following input dictionary: "
              << adobe::begin_asl_cel << result << adobe::end_asl_cel
              << std::endl
              << "--" << std::endl;

    return result;
}

/****************************************************************************************************/

struct sheet_tracker
{
    sheet_tracker(const bfs::path& sheet_path,
                  const bfs::path& input_path) :
        callbacks_m(adobe::bind_to_sheet(sheet_m))
    {
        //  attach the VM to the sheet.
        sheet_m.machine_m.set_variable_lookup(boost::bind(&adobe::sheet_t::get, &sheet_m, _1));
    
        std::string     sheet_path_str(sheet_path.native_file_string());
        std::ifstream   sheet_stream(sheet_path_str.c_str());

        callbacks_m.add_cell_proc_m = 
            boost::bind(&sheet_tracker::add_cell_trap, boost::ref(*this), 
                        callbacks_m.add_cell_proc_m, _1, _2, _3, _4);
        callbacks_m.add_interface_proc_m =
            boost::bind(&sheet_tracker::add_interface_trap, boost::ref(*this), 
                        callbacks_m.add_interface_proc_m, _1, _2, _3, _4, _5, _6);

        if (!sheet_stream.is_open())
            std::cerr << "Could not open \"" << sheet_path_str << "\"!\n";

        // set up adam sheet
        adobe::parse(sheet_stream,
                     adobe::line_position_t("property model sheet"),
                     callbacks_m);

        sheet_m.set(parse_input_dictionary(input_path));

        sheet_m.update();
    }

    void loop();

private:
    sheet_tracker();                                    // unimplemented
    sheet_tracker(const sheet_tracker&);                // unimplemented
    sheet_tracker& operator = (const sheet_tracker&);   // unimplemented

    void add_cell_trap( adobe::adam_callback_suite_t::add_cell_proc_t   original,
                        adobe::adam_callback_suite_t::cell_type_t       type,
                        adobe::name_t                                   cell_name,
                        const adobe::line_position_t&                   position,
                        const adobe::array_t&                           expr_or_init);

    void add_interface_trap(adobe::adam_callback_suite_t::add_interface_proc_t  original,
                            adobe::name_t                                       cell_name,
                            bool                                                linked,
                            const adobe::line_position_t&                       position1,
                            const adobe::array_t&                               initializer,
                            const adobe::line_position_t&                       position2,
                            const adobe::array_t&                               expression);

    adobe::sheet_t                          sheet_m;
    adobe::adam_callback_suite_t            callbacks_m;
    std::map<adobe::name_t, adobe::any_regular_t> cell_set_m;
};

/****************************************************************************************************/

adobe::name_t cell_type_to_name(adobe::adam_callback_suite_t::cell_type_t type)
{
    switch (type)
    {
        case adobe::adam_callback_suite_t::input_k:     
            return adobe::static_name_t("input");       break;
        case adobe::adam_callback_suite_t::output_k:    
            return adobe::static_name_t("output");      break;
        case adobe::adam_callback_suite_t::constant_k:  
            return adobe::static_name_t("constant");    break;
        case adobe::adam_callback_suite_t::logic_k:     
            return adobe::static_name_t("logic");       break;
        case adobe::adam_callback_suite_t::invariant_k: 
            return adobe::static_name_t("invariant");   break;
        default: 
            return adobe::static_name_t("unknown"); break;
    }
}

/****************************************************************************************************/

void sheet_tracker::add_cell_trap(  adobe::adam_callback_suite_t::add_cell_proc_t   original,
                                    adobe::adam_callback_suite_t::cell_type_t       type,
                                    adobe::name_t                                   cell_name,
                                    const adobe::line_position_t&                   position,
                                    const adobe::array_t&                           expr_or_init)
{
    original(type, cell_name, position, expr_or_init, std::string(), std::string());

    std::cout << "A \'" << cell_name.c_str() << "\' (" << cell_type_to_name(type) << ")" << std::endl;

    if (cell_type_to_name(type) == adobe::static_name_t("output"))
        cell_set_m[cell_name] = adobe::any_regular_t();
}

/****************************************************************************************************/

void sheet_tracker::add_interface_trap( adobe::adam_callback_suite_t::add_interface_proc_t  original,
                                        adobe::name_t                                       cell_name,
                                        bool                                                linked,
                                        const adobe::line_position_t&                       position1,
                                        const adobe::array_t&                               initializer,
                                        const adobe::line_position_t&                       position2,
                                        const adobe::array_t&                               expression)
{
    original(cell_name, linked, position1, initializer, position2, expression,
             std::string(), std::string());

    std::cout << "A \'" << cell_name.c_str() << "\' (interface)" <<  std::endl;

    cell_set_m[cell_name] = adobe::any_regular_t();
}

/****************************************************************************************************/

void sheet_tracker::loop()
{
    std::string              cell_name_buffer;
    std::vector<char>        new_value_buffer;

    cell_name_buffer.reserve(1024);
    new_value_buffer.reserve(1024);
    new_value_buffer.resize(1); 

	sheet_m.update();
    for(cell_set_t::iterator iter=cell_set_m.begin(), end = cell_set_m.end(); iter!= end; ++iter)
    {
        sheet_m.monitor_value(iter->first, 	boost::bind(&simple_display, boost::ref(*iter), _1));
    }

    while (true)
    {
        std::cout << "--" << std::endl;

        sheet_m.update();

        std::cout << "sheet:" << std::endl;
        adobe::for_each(cell_set_m, stream_cell_state);
        std::cout << "Enter in the name of a cell for which you would like to change the value: ";
        std::cin.getline(&cell_name_buffer[0], 1024);

        if (cell_name_buffer[0] == 0)
            break;

        std::cout << "Enter in the new value for cell \'" << &cell_name_buffer[0] << "\': ";
        std::cin.getline(&new_value_buffer[0], 1024);

        if (new_value_buffer[0] == 0)
            break;

        adobe::any_regular_t new_value(sheet_m.inspect(adobe::parse_adam_expression(&new_value_buffer[0])));

        adobe::name_t cell(&cell_name_buffer[0]);

        std::cout << "Setting cell " << cell << " to " << adobe::begin_asl_cel << new_value << adobe::end_asl_cel << std::endl;

        sheet_m.set(cell, new_value);
    }
}

/****************************************************************************************************/

int main(int argc, char* argv[])
{
    try
    {
        std::cout << "Adobe Adam Tutorial" << std::endl;
        std::cout << "Compiler: " << BOOST_COMPILER << std::endl;
        std::cout << "     ASL: v" << ADOBE_VERSION_MAJOR << "." << ADOBE_VERSION_MINOR << "." 
                  << ADOBE_VERSION_SUBMINOR << std::endl;
        std::cout << "   Boost: v" << BOOST_VERSION / 100000 << "." << BOOST_VERSION / 100 % 1000 
                  << "." << BOOST_VERSION % 100 << std::endl;
        std::cout << "--" << std::endl;

        std::string sheet_pathname(argc > 1 ? (argv[1]) : ("default.adm"));
        bfs::path   sheet_filepath(sheet_pathname.c_str(), bfs::native);
        std::string input_pathname(argc > 2 ? (argv[2]) : ("input.cel"));
        bfs::path   input_filepath(input_pathname.c_str(), bfs::native);

        sheet_tracker(sheet_filepath, input_filepath).loop();
    }
    catch (const std::exception& error)
    {
        std::cerr << "Exception: " << error.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Unknown Exception\n";
    }

    std::cout << "Bye!" << std::endl;
}
