#include <iostream>

#include <boost/function.hpp>

#include <adobe/implementation/expression_filter.hpp>
#include <adobe/implementation/expression_parser.hpp>
#include <adobe/implementation/expression_formatter.hpp>
#include <adobe/iomanip_asl_cel.hpp>
#include <adobe/virtual_machine.hpp>

void test_expression(const char*                                                    expression_string,
                     const boost::function<adobe::array_t (const adobe::array_t&)>& process_func)
{
    adobe::array_t    expression;
    std::stringstream stream(expression_string);

    adobe::expression_parser(stream, adobe::line_position_t("expression_string")).require_expression(expression);
    
    std::cout << std::endl << "-=- testing expression -=- " << std::endl << std::endl;

    std::cout << "Raw token stream:" << std::endl;
    std::cout << adobe::begin_asl_cel_unsafe << expression << adobe::end_asl_cel_unsafe << std::endl;
    
    std::cout << "Round-trip expression:" << std::endl;
    std::cout << adobe::format_expression(expression) << std::endl;
    
    adobe::array_t processed(process_func(expression));
    
    std::cout << "Raw Processed expression:" << std::endl;
    std::cout << adobe::begin_asl_cel_unsafe << processed << adobe::end_asl_cel_unsafe << std::endl;
    
    std::cout << "Processed expression:" << std::endl;
    std::cout << adobe::format_expression(processed) << std::endl;

    static adobe::virtual_machine_t vm;

    vm.evaluate(expression);

    std::cout << "VM Round-trip expression:" << std::endl;
    std::cout << adobe::begin_asl_cel_unsafe << vm.back().value_m << adobe::end_asl_cel_unsafe << std::endl;

    vm.pop_back();
    vm.evaluate(processed);

    std::cout << "VM processed expression:" << std::endl;
    std::cout << adobe::begin_asl_cel_unsafe << vm.back().value_m << adobe::end_asl_cel_unsafe << std::endl;

    vm.pop_back();
}

int main()
{
    std::cout << adobe::entity_unescape("Hello, world!") << std::endl;
    std::cout << adobe::entity_unescape("&quot;Hello, world!&apos;") << std::endl;

    std::cout << adobe::entity_escape("Hello, world!") << std::endl;
    std::cout << adobe::entity_escape("\"Hello, world!'") << std::endl;

    std::cout << "<char_rountrip_test>" << std::endl;

    for (long i(0); i < 256; ++i)
    {
        adobe::string_t cur;

        cur.push_back(static_cast<char>(i));

        adobe::string_t xml(adobe::entity_escape(cur));

        adobe::string_t ascii(adobe::entity_unescape(xml));

        if (ascii.empty() ||
            static_cast<unsigned char>(*ascii.begin()) != static_cast<unsigned char>(i))
            throw std::runtime_error(adobe::make_string("character mismatch: ", xml.c_str()));
    }

    std::cout << "</char_rountrip_test>" << std::endl;

    test_expression("[ \"contains a \" '\"' \" character\" ]",
                    &adobe::entity_escape_expression);

    test_expression("[ \"contains a &quot; character\" ]",
                    &adobe::entity_unescape_expression);

    test_expression("{ name: \"contains a \" '\"' \" character\" }",
                    &adobe::entity_escape_expression);

    test_expression("{ name: \"contains a &quot; character\" }",
                    &adobe::entity_unescape_expression);
}
