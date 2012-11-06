/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ADAM_PARSER_HPP
#define ADOBE_ADAM_PARSER_HPP

#include <adobe/config.hpp>

#include <iosfwd>
#include <string>

#include <boost/function.hpp>

#include <adobe/array.hpp>
#include <adobe/istream.hpp>
#include <adobe/string.hpp>
#include <adobe/vector.hpp>

/*!
\defgroup adam_reference Property Model Language Reference
\ingroup property_model

The Property Model Library is a runtime library (see \ref adam_engine). The Property Model Language
and \ref adamparser are facilities provided to make it easier to assemble an adobe::sheet_t from a
simple declaration. This manual describes the Property Model Language.

The language is divided into three parts, \ref expression_lexical_conventions, \ref
expression_expressions, and \ref sheet_declarations. The first two parts are shared by the Layout
Library Language, and describe in the \ref expression_reference.

\section sheet_declarations Sheet Declarations

\subsection adam_grammar Property Model Grammar
\verbatim
translation_unit        = { sheet_specifier }.
sheet_specifier         = [lead_comment] "sheet" identifier "{" { qualified_cell_decl } "}"
                            [trail_comment].

qualified_cell_decl     = interface_set_decl | input_set_decl | output_set_decl
                            | constant_set_decl | logic_set_decl | invariant_set_decl
                            | external_set_decl.

interface_set_decl      = "interface"   ":" { [lead_comment] interface_cell_decl }.
input_set_decl          = "input"       ":" { [lead_comment] input_cell_decl }.
output_set_decl         = "output"      ":" { [lead_comment] output_cell_decl }.
constant_set_decl       = "constant"    ":" { [lead_comment] constant_cell_decl }.
logic_set_decl          = "logic"       ":" { [lead_comment] logic_cell_decl }.
invariant_set_decl      = "invariant"   ":" { [lead_comment] invariant_cell_decl }.
external_set_decl       = "external"    ":" { [lead_comment] identifier end_statement }.

interface_cell_decl     = ["unlink"] identifier [initializer] [define_expression] end_statement.
input_cell_decl         = identifier [initializer] end_statement.

output_cell_decl        = named_decl.
constant_cell_decl      = identifier initializer end_statement.
logic_cell_decl         = named_decl | relate_decl.
invariant_cell_decl     = named_decl.

relate_decl             = [conditional] "relate" "{" relate_expression relate_expression
                            { relate_expression } "}" [trail_comment].
relate_expression       = [lead_comment] identifier { "," identifier } define_expression
                            end_statement.
named_decl              = identifier define_expression end_statement.

initializer             = ":" expression.
conditional             = "when" "(" expression ")".

define_expression       = "<==" expression.

end_statement           = ";" [trail_comment].

keywords                += "sheet" | "interface" | "input" | "output" | "constant" | "logic"
                             | "invariant" | "unlink" | "when" | "relate" | "external".
\endverbatim

\subsection cell_qualifiers Cell Access Specifiers


\subsection initialization Initialization 

\subsection advanced_features Advanced Features

\subsubsection external_types External Types and Functions
Although there are a set of types provided by the language, the type system is extensible to support
any C++ type through external functions and values introduced from outside of the system. For
example, if an external function were provided to supply an image, other functions could be provided
to operator on that image. External functions can be provided through the virtual machine API [ref].
An external function to get an image from a URL might look this:

\code
struct image_type { }; // some image structure
image_type get_image_for_url(const char* url); // Some external function to get an image

adobe::value_t get_image(const adobe::array_t& parameters)
{
    return adobe::value_t(get_image(parameters[0].get<std::string>().c_str()));
}
\endcode

A function to operate on the image might look like this:

\code
void guassian_blur(image_type& image); // Some external operation

adobe::value_t blur_image(const adobe::array_t& parameters)
{
    image_type result = parameters[0].get<image_type>();
    guassian_blur(result);
    return result;
}
\endcode

After registering these functions with the virtual machine - the following expressions are valid:

\verbatim
get_image("http:\\local\my_image.jpg") == get_image("http:\\local\my_other_image.jpg")
blur_image(get_image("http:\\local\my_image.jpg"))
\endverbatim
*/


/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

struct adam_callback_suite_t
{
    enum cell_type_t
    {
        input_k,            // array_t is an initializer
        output_k,           // array_t is an expression
        constant_k,         // array_t is an initializer
        logic_k,            // array_t is an expression
        invariant_k         // array_t is an expression
    };

    struct relation_t
    {
        vector<name_t>      name_set_m;
        line_position_t     position_m;
        array_t             expression_m;
        string_t            detailed_m;
        string_t            brief_m;
    };

    typedef boost::function<void (  cell_type_t             type,
                                    name_t                  cell_name,
                                    const line_position_t&  position,
                                    const array_t&          expr_or_init,
                                    const std::string&      brief,
                                    const std::string&      detailed)>      add_cell_proc_t;

    typedef boost::function<void (  const line_position_t&  position,
                                    const array_t&          conditional,
                                    const relation_t*       first,
                                    const relation_t*       last,
                                    const std::string&      brief,
                                    const std::string&      detailed)>      add_relation_proc_t; // REVISIT (sparent) where's brief?
    
    typedef boost::function<void (  name_t                  cell_name,
                                    bool                    linked,
                                    const line_position_t&  position1,
                                    const array_t&          initializer,
                                    const line_position_t&  position2,
                                    const array_t&          expression,
                                    const std::string&      brief,
                                    const std::string&      detailed)>      add_interface_proc_t;
                                    
    typedef boost::function<void (  name_t                  cell_name,
                                    const line_position_t&  position,
                                    const std::string&      brief,
                                    const std::string&      detailed)>      add_external_proc_t;

    add_cell_proc_t         add_cell_proc_m;
    add_relation_proc_t     add_relation_proc_m;
    add_interface_proc_t    add_interface_proc_m;
    add_external_proc_t     add_external_proc_m;
};

/*************************************************************************************************/

void parse(std::istream& stream, const line_position_t& position, const adam_callback_suite_t& callbacks);

/*************************************************************************************************/

array_t parse_adam_expression(const std::string& expression);

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif // ADOBE_ADAM_PARSER_HPP

/*************************************************************************************************/
