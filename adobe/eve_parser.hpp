/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_EVE_PARSER_HPP
#define ADOBE_EVE_PARSER_HPP

#include <adobe/config.hpp>

#include <string>

#include <boost/any.hpp>
#include <boost/function.hpp>

#include <adobe/array.hpp>
#include <adobe/string.hpp>
#include <adobe/istream.hpp>
#include <adobe/name_fwd.hpp>
#include <adobe/vector.hpp>

/*************************************************************************************************/

/*!
\defgroup eve_reference Layout Library Language Reference
\ingroup layout_library

The Layout Library is a runtime library (see \ref eve_engine). The Layout Library Language and \ref
eveparser are facilities provided to make it easier to assemble an adobe::eve_t from a simple
declaration. This manual describes the Layout Library Language.

The language is divided into three parts, \ref expression_lexical_conventions, \ref
expression_expressions, and \ref view_declarations. The first two parts are shared by the Property
Model Language, and describe in the \ref expression_reference.

\section view_declarations Layout Declarations

\subsection eve_grammar Layout Grammar
\verbatim
layout_specifier        = [lead_comment] "layout" identifier "{" { qualified_cell_decl }
                            "view" view_definition "}" [trail_comment].

qualified_cell_decl     = interface_set_decl | constant_set_decl | logic_set_decl.

interface_set_decl      = "interface"   ":" { [lead_comment] interface_cell_decl }.
constant_set_decl       = "constant"    ":" { [lead_comment] constant_cell_decl }.
logic_set_decl          = "logic"       ":" { [lead_comment] logic_cell_decl }.

interface_cell_decl     = ["unlink"] identifier [initializer] [define_expression] end_statement.
constant_cell_decl      = identifier initializer end_statement.
logic_cell_decl         = named_decl | relate_decl.

relate_decl             = [conditional] "relate" "{" relate_expression relate_expression
                            { relate_expression } "}" [trail_comment].
relate_expression       = [lead_comment] identifier { "," identifier } define_expression
                            end_statement.
named_decl              = identifier define_expression end_statement.

initializer             = ":" expression.
conditional             = "when" "(" expression ")".

define_expression       = "<==" expression.

end_statement           = ";" [trail_comment].
    
view_definition         = [lead_comment] view_class_decl ((";" [trail_comment])
                            | ([trail_comment] view_statement_list)).
view_statment_sequence  = { view_definition }.
view_class_decl         = ident "(" [ named_argument_list ] ")".
view_statment_list      = "{" view_statement_sequence "}".

keywords                += "layout" | "view" | "interface" | "constant" | "logic" | "unlink" 
                            | "when" | "relate".
\endverbatim

A layout declaration consists of two parts - a block of cell declarations and a view instance. Here
is an example of a simple layout declaration:

\verbatim
layout simple_dialog
{
 constant:
    dialog_name: "Hello";

    view dialog(name: dialog_name)
    {
        button(name: "World!");
    }
}
\endverbatim

The identifier "simple_dialog" names this layout [NOTE: This identifier is currently unused by the
system but intended for future use].

The first part, beginning with the qualifier "constant:" in the example, declares cells. The cells
are used to construct an adobe::sheet_t. A cell name may be used as a variable
name in any expression after the declaration. Here the cell dialog_name is used as a variable to set
the name of the dialog instance.

The second part of the layout is the view instance, beginning with the keyword "view". This consists
of a nested set of view class declarations. Here "dialog(name: dialog_name)" declares a "dialog"
with the property "name" and the value "Hello". A layout does not ascribe meaning to the view class
or view class properties, the particular set of view classes and properties is dependent on the API
upon which the layout engine and parser are integrated. ASL does provide a common set of view
classes for the Mac OS X Carbon and Win32 APIs which should serve as an example for integrating with
any API. [include reference to widget library here.]

The other type of cell supported by layouts are interface cells. An interface cell can be bound
against from within the view instance. This is a runtime binding (unlink using a cell as a variable
which is evaluated once at parse time). This is useful for maintaining view specific state such as
which panel in a tab group is shown:

\verbatim
layout simple_tab_group
{
 interface:
    visible_panel: @first_panel; // default to first panel

    view dialog(name: "Simple Tab Group")
    {
        tab_group(  bind: @visible_panel,
                    items: [
                        {name: "Panel 1", value: @first_panel},
                        {name: "Panel 2", value: @second_panel}
                    ])
        {
            panel(value: @first_panel, bind: @visible_panel)
            {
                static_text(name: "Contents of first panel.");
            }
    
            panel(value: @second_panel, bind: @visible_panel)
            {
                static_text(name: "Contents of second panel.");
            }
        }
    }
}
\endverbatim

This layout will construct a dialog with a tab group with two panels which can be flipped between.
Which panel is visible is controlled by the interface cell "visible_panel". [NOTE: In a future
release the layout state will be easily persisted including the current window size, if it is
grow-able, and position.].
*/


/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

struct eve_callback_suite_t
{
    enum cell_type_t
    {
        constant_k,
        logic_k,
        interface_k
    };
	
    struct relation_t
    {
        vector<name_t>  name_set_m;
        line_position_t position_m;
        array_t         expression_m;
        string_t		detailed_m;
        string_t		brief_m;
    };
    
    typedef boost::any position_t;

    typedef boost::function<position_t (    const position_t&       parent,
                                            const line_position_t&  parse_location,
                                            name_t                  name,
                                            const array_t&          parameters,
                                            const string_t&			brief,
                                            const string_t&			detailed)>  add_view_proc_t;
                                            
    typedef boost::function<void (  cell_type_t             type,
                                    name_t                  name,
                                    const line_position_t&  position,
                                    const array_t&          initializer,
                                    const string_t&			brief,
								  const string_t&			detailed)>          add_cell_proc_t;
	
    typedef boost::function<void (  const line_position_t&  position,
								  const array_t&			conditional,
								  const relation_t*			first,
								  const relation_t*			last,
								  const string_t&			brief,
								  const string_t&			detailed)>      add_relation_proc_t;
    
    typedef boost::function<void (  name_t                  cell_name,
								  bool						linked,
								  const line_position_t&	position1,
								  const array_t&			initializer,
								  const line_position_t&	position2,
								  const array_t&			expression,
								  const string_t&			brief,
								  const string_t&			detailed)>      add_interface_proc_t;
                                    
    typedef boost::function<void()> finalize_sheet_proc_t;
                                    
                                            
    add_view_proc_t         add_view_proc_m;
    add_cell_proc_t         add_cell_proc_m;
    finalize_sheet_proc_t   finalize_sheet_proc_m;
    add_relation_proc_t     add_relation_proc_m;
    add_interface_proc_t    add_interface_proc_m;
};

line_position_t parse(std::istream& in, const line_position_t&,
    const eve_callback_suite_t::position_t&, const eve_callback_suite_t&);

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif // ADOBE_EVE_PARSER_HPP

/*************************************************************************************************/
