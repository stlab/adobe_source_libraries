/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/


/*************************************************************************************************/

#include <boost/filesystem/fstream.hpp>
#include <boost/program_options.hpp>

#include <adobe/dictionary.hpp>
#include <adobe/adam.hpp>


#include "adam_test_parser.hpp"

/*************************************************************************************************/

namespace {

std::string grammar = 
"translation_unit       = interaction_list .         \n\
interaction_list        = interaction [ \";\" ] [ interaction_list ].             \n\
interaction             = update_decl | reinitialize_decl               \n\
                                      | dump_decl | check_decl | print_decl | assert_decl \n\
                                      | contributing_decl | trailing_comment | sheet . \n\
update_decl             = \"update\" \"(\" identifier \",\" dictionary \")\".\n\
reinitialize_decl       = \"reinitialize\" \"(\" identifier \",\" dictionary \")\".\n\
dump_decl               = \"dump\" \"(\" identifier \")\"\n\
check_decl              = \"check\" \"(\" identifier \",\" dictionary \")\".\n\
print_decl              = \"print\" \"(\" identifier \",\" expression \")\".\n\
assert_decl             = \"assert\" \"(\" identifier \",\" expression \")\".\n\
contributing_decl       = \"contributing\" \"(\" identifier \")\".\n\
                                                                        \n\
keywords                += \"update\" | \"reinitialize\" | \"check\" | \"dump\" | \"print\" \n\
                           | \"assert\" | \"contributing\" .";

}
/*************************************************************************************************/

namespace po = boost::program_options;

int main(int argc, char* argv[]) 
{
    bool success = true;
    try { 
        po::options_description cmd_only("Command line only options");
        cmd_only.add_options()
            ("help", "produce help message")
            ("version,v", "print version string")
            ;

        po::options_description hidden("Hidden options");
        hidden.add_options()
            ("input-file", po::value< std::vector<std::string> >(), "input file")
            ;

        po::positional_options_description positional;
        positional.add("input-file", -1);

        po::options_description cmdline_options;
        cmdline_options.add(cmd_only).add(hidden);

//        po::options_description visible("Allowed options");
//        visible.add(cmd_only).add(config);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).
                  options(cmdline_options).positional(positional).run(),vm);
        po::notify(vm);    

        if (vm.count("help")) {
            std::cout << "Usage: pmeval [option]... [file]...\n";
            std::cout << cmd_only;
            std::cout << "\n\nGrammar:\n" << grammar << std::endl;

        }

        if (vm.count("version")) {
            std::cout << "Property model evaluator, version " << ADOBE_VERSION << std::endl;
        }

        if(0 == vm.count("input-file")) {
            if(!adobe::parse(std::cin, adobe::line_position_t("standard input"), std::cout))
                success = false;            
            return success == true ? 0 : 1;
        }
        std::vector<std::string> input_files(vm["input-file"].as< std::vector<std::string> >());

        for(std::vector<std::string>::const_iterator 
                i = input_files.begin(), end = input_files.end(); i != end; ++i){
            boost::filesystem::path in_path(*i, boost::filesystem::native);
            std::ifstream in_stream(in_path.native_file_string().c_str());
            if (!in_stream.is_open())
                std::cerr << "Could not open \"" << in_path << "\"!\n";
            if(!adobe::parse(in_stream, adobe::line_position_t(
                                 in_path.native_file_string().c_str()), 
                             std::cout))
                success = false;
        }
    }

    catch(adobe::stream_error_t& e){
        std::cout << adobe::format_stream_error(e) << std::endl;
        success = false;
    }

    return success == true ? 0 : 1;
}

/*************************************************************************************************/
