/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/eve_parser.hpp>

#include <string>

#include <boost/array.hpp>

#include <adobe/algorithm/sorted.hpp>
#include <adobe/algorithm/binary_search.hpp>
#include <adobe/functional/operator.hpp>
#include <adobe/any_regular.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/name.hpp>
#include <adobe/implementation/token.hpp>

#include <adobe/implementation/expression_parser.hpp>

/*************************************************************************************************/

namespace {

using namespace adobe;

/*************************************************************************************************/

aggregate_name_t     constant_k  = { "constant" };
aggregate_name_t     interface_k = { "interface" };
aggregate_name_t     layout_k    = { "layout" };
aggregate_name_t     logic_k     = { "logic" };
aggregate_name_t     relate_k    = { "relate" };
aggregate_name_t     unlink_k    = { "unlink" };
aggregate_name_t     view_k      = { "view" };
aggregate_name_t     when_k      = { "when" };

aggregate_name_t keyword_table[] = {
    constant_k,
    interface_k,
    layout_k,
    logic_k,
    relate_k,
    unlink_k,
    view_k,
    when_k
};

/*************************************************************************************************/

bool keyword_lookup(const name_t& name)
{
    using namespace adobe;

#ifndef NDEBUG
    static bool inited = false;
    if (!inited) {
        assert(is_sorted(keyword_table));
        inited = true;
    }
#endif
    
    return binary_search(keyword_table, name, less(), constructor<name_t>()) != boost::end(keyword_table);
}

/*************************************************************************************************/

class eve_parser : public expression_parser
{
public:
    typedef eve_callback_suite_t::position_t position_t;

    eve_parser(const eve_callback_suite_t& assembler, std::istream& in,
            const line_position_t& position) :
        expression_parser(in, position), assembler_m(assembler)
    {
        set_keyword_extension_lookup(boost::bind(&keyword_lookup, _1));
    
        assert(assembler_m.add_view_proc_m);
    //  assert(assembler_m.add_cell_proc_m); Only required if you have a sheet state.
    }

    void parse(const position_t&);

private:
    typedef eve_callback_suite_t::relation_t	relation_t;
    typedef std::vector<relation_t>             relation_set_t;

    bool is_layout_specifier(const position_t&);
    bool is_qualified_cell_decl();
	
    bool is_interface_set_decl();
    bool is_constant_set_decl();
	bool is_logic_set_decl();
	
	bool is_interface_cell_decl(const string_t& detailed);
	bool is_constant_cell_decl(const string_t& detailed);
	bool is_logic_cell_decl(const string_t& detailed);
	
	bool is_relate_decl(line_position_t& position, array_t& expression, relation_set_t&, string_t&);
    bool is_relate_expression_decl(relation_t&);
    bool is_named_decl(name_t& cell_name, line_position_t& position, array_t& expression, string_t&);
	
   // bool is_cell_decl(eve_callback_suite_t::cell_type_t);
    bool is_initializer(line_position_t& position, array_t& expression);
	bool is_conditional(line_position_t& position, array_t& expression);
	
	bool is_define_expression(line_position_t&, array_t&);
	
    void require_end_statement(string_t&);
    bool is_view_definition(const position_t&);
    bool is_view_statement_sequence(const position_t&);
    bool is_view_class_decl(name_t& class_name, array_t& arguments);
    void require_view_statement_list(const position_t&);
	
    typedef bool (eve_parser::*set_decl_t)(const string_t& detailed);
    bool is_set_decl(name_t, set_decl_t);

    eve_callback_suite_t     assembler_m;
};

/*************************************************************************************************/
    
void eve_parser::parse(const position_t& position)
{
    if (!is_layout_specifier(position)) throw_exception("layout specifier required");
}

/*************************************************************************************************/

bool eve_parser::is_layout_specifier(const position_t& position)
{
/* REVISIT (sparent) : Top level block is ignored. */
    
    using namespace adobe;
    
    is_token(lead_comment_k);
    
    if (!is_keyword(layout_k)) return false;
    
    require_token(identifier_k);
    require_token(open_brace_k);
    while (is_qualified_cell_decl()) { };
    
    if (assembler_m.finalize_sheet_proc_m) assembler_m.finalize_sheet_proc_m();
    
    require_keyword(view_k);
    if (!is_view_definition(position)) throw_exception("view definition required");
    require_token(close_brace_k);
    is_token(trail_comment_k);
    return true;
}

/*************************************************************************************************/

bool eve_parser::is_qualified_cell_decl()
{
    if (is_interface_set_decl() || is_constant_set_decl() || is_logic_set_decl()) return true;
    return false;
}
    
/*************************************************************************************************/
	
bool eve_parser::is_set_decl(name_t token, set_decl_t set_decl)
{
	if (!is_keyword(token)) return false;
	
	require_token(colon_k);
	
	while (true)
	{
		string_t detailed;
		(void)is_lead_comment(detailed);
		if (!(this->*set_decl)(detailed)) break;
	}
	
	return true;
}

/*************************************************************************************************/

//  output_set_decl         = "output"      ":" { [lead_comment] output_cell_decl }.
bool eve_parser::is_interface_set_decl()
{
    return is_set_decl(interface_k, &eve_parser::is_interface_cell_decl);
}

/*************************************************************************************************/

//  constant_set_decl       = "constant"    ":" { [lead_comment] constant_cell_decl }.
bool eve_parser::is_constant_set_decl()
{
    return is_set_decl(constant_k, &eve_parser::is_constant_cell_decl);
}
	
/*************************************************************************************************/

//  logic_set_decl          = "logic"       ":" { [lead_comment] logic_cell_decl }.
bool eve_parser::is_logic_set_decl()
{
	return is_set_decl(logic_k, &eve_parser::is_logic_cell_decl);
}
    
/*************************************************************************************************/

//  interface_cell_decl     = ["unlink"] identifier [initializer] [define_expression] end_statement.
bool eve_parser::is_interface_cell_decl(const string_t& detailed)
{
	name_t          cell_name;
	array_t         initializer, expression;
	line_position_t initializer_position, expression_position;
	string_t		brief;
	
	bool linked (!is_keyword(unlink_k));
	
	if (!is_identifier(cell_name)) return false;
	
	(void)is_initializer(initializer_position, initializer);
	
	(void)is_define_expression(expression_position, expression);
	
	require_end_statement(brief);
	
	assembler_m.add_interface_proc_m (cell_name, linked, initializer_position,
												initializer, expression_position, expression, brief, detailed);
	
	return true;
}
	
/*************************************************************************************************/

//  constant_cell_decl      = identifier initializer end_statement.
bool eve_parser::is_constant_cell_decl(const string_t& detailed)
{
	name_t          cell_name;
	line_position_t position;
	array_t         initializer;
	string_t		brief;
	
	if (!is_identifier(cell_name)) return false;
	if (!is_initializer(position, initializer)) throw_exception("initializer required");
	require_end_statement(brief);
	
	assembler_m.add_cell_proc_m(eve_callback_suite_t::constant_k, cell_name,
										  position, initializer, brief, detailed);
	
	return true;
}

/*************************************************************************************************/

#if 0
// cell_decl                = [lead_comment] identifier initializer end_statement.
bool eve_parser::is_cell_decl(eve_callback_suite_t::cell_type_t type)
{
    using namespace adobe;

    std::string     detailed;
    std::string     brief;
    name_t          cell_name;
    line_position_t position;
    array_t         initializer;

    (void)is_lead_comment(detailed);
    if (!is_identifier(cell_name)) return false;
    if (!is_initializer(position, initializer)) throw_exception("initializer required");
    require_end_statement(brief);
    
    assembler_m.add_cell_proc_m(type, cell_name, position, initializer, brief, detailed);

    return true;
}
#endif
	
/*************************************************************************************************/

//  logic_cell_decl         = named_decl | relate_decl.
bool eve_parser::is_logic_cell_decl(const string_t& detailed)
{
	name_t          cell_name;
	line_position_t position;
	array_t         expression;
	string_t		brief;
	
	relation_set_t  relations;
	
	if (is_named_decl(cell_name, position, expression, brief))
	{
		assembler_m.add_cell_proc_m(eve_callback_suite_t::logic_k, cell_name,
											  position, expression, brief, detailed);
		return true;
	}
	else if (is_relate_decl(position, expression, relations, brief))
	{
		assembler_m.add_relation_proc_m(  position, expression, &relations.front(),
												  &relations.front() + relations.size(),
												  brief, detailed);
		return true;
	}
	return false;
}
    
/*************************************************************************************************/

//  relate_decl             = [conditional] "relate" "{" relate_expression relate_expression { relate_expression } "}" [trail_comment]
bool eve_parser::is_relate_decl(line_position_t& position, array_t& expression,
								 relation_set_t& relation_set, string_t& brief)
{
	/*
	 REVISIT (sparent) : A relation_set_t needs a position independent of the continitional
	 expression in order to report overconstraints. Here we "fudge" and reuse the conditional
	 expression for this purpose.
	 */
	bool conditional(is_conditional(position, expression));
	
	if (!is_keyword(relate_k))
	{
		if (conditional) throw_exception("relate required");
		return false;
	}
	
	if (!conditional) position = next_position();
	
	require_token(open_brace_k);
	
	relation_t relation_1;
	relation_t relation_2;
	
	if (!is_relate_expression_decl(relation_1) ||
		!is_relate_expression_decl(relation_2))
	{
		throw_exception("minimum two relate_expression required");
	}
	
	relation_set.push_back(relation_1);
	relation_set.push_back(relation_2);
	
	relation_1.expression_m.clear();
	while (is_relate_expression_decl(relation_1))
	{
		relation_set.push_back(relation_1);
		relation_1.expression_m.clear();
	}
	
	require_token(close_brace_k);
	
	(void)is_trail_comment(brief);
	
	return true;
}

/*************************************************************************************************/

//  relate_expression       = [lead_comment] identifier { "," identifier } define_expression
//                              end_statement.
bool eve_parser::is_relate_expression_decl(relation_t& relation)
{
    (void)is_lead_comment(relation.detailed_m);
    
    name_t cell_name;
    if (!is_identifier(cell_name)) return false;
    relation.name_set_m.push_back(cell_name);
    
    while (is_token(comma_k)) {
        if (!is_identifier(cell_name)) throw_exception("identifier required");
        relation.name_set_m.push_back(cell_name);
    }
    
    if (!is_define_expression(relation.position_m, relation.expression_m))
        throw_exception("define_expression required"); 
    require_end_statement(relation.brief_m);
    return true;
}

/*************************************************************************************************/

//  named_decl              = identifier define_expression end_statement.
bool eve_parser::is_named_decl(name_t& cell_name, line_position_t& position, array_t& expression, string_t& brief)
{
	if (!is_identifier(cell_name)) return false;
	if (!is_define_expression(position, expression)) throw_exception("define_expression required"); 
	require_end_statement(brief);
	return true;
}

/*************************************************************************************************/

// initializer              = ":" expression.
bool eve_parser::is_initializer(line_position_t& position, array_t& expression)
{
    using namespace adobe;

    if (!is_token(colon_k)) return false;
    
    position = next_position();
    require_expression(expression);

    return true;
}
	
/*************************************************************************************************/

//  define_expression       = "<==" expression.
bool eve_parser::is_define_expression(line_position_t& position, array_t& expression)
{
	if (!is_token(is_k)) return false;
	
	position = next_position();
	require_expression(expression);
	return true;
}

/*************************************************************************************************/

//  conditional = "when" "(" expression ")".

bool eve_parser::is_conditional(line_position_t& position, array_t& expression)
{
	if (!is_keyword(when_k)) return false;
	
	require_token(open_parenthesis_k);
	
	position = next_position();
	
	require_expression(expression);
	require_token(close_parenthesis_k);
	
	return true;
}

/*************************************************************************************************/

// end_statement            = ";" [trail_comment].
void eve_parser::require_end_statement(string_t& brief)
{
    require_token(semicolon_k);
    (void)is_trail_comment(brief);
}

/*************************************************************************************************/
    
bool eve_parser::is_view_definition(const position_t& location)
{
    using namespace adobe;

    string_t     detailed;
    string_t     brief;
    name_t          class_name;
    array_t         arguments;
    
    (void)is_lead_comment(detailed);
    
    // Report any errors in calling the client at the start of the class decl.
    line_position_t line_position = next_position();
    
    if (!is_view_class_decl(class_name, arguments)) return false;
    
    bool leaf (is_token(semicolon_k));
    
    (void)is_trail_comment(brief);
    
    position_t node_location;
    
    try
    {
        node_location = assembler_m.add_view_proc_m(location, line_position, class_name, arguments,
            brief, detailed);
    }
    catch (const std::exception& error)
    {
        throw stream_error_t(error, line_position);
    }
    
    if (!leaf)
    {
        require_view_statement_list(node_location);
    }
    
    return true;
}

/*************************************************************************************************/
    
bool eve_parser::is_view_statement_sequence(const position_t& location)
{
    while (is_view_definition(location)) { }
    return true;
}

/*************************************************************************************************/
    
bool eve_parser::is_view_class_decl(name_t& class_name, array_t& arguments)
{
    using namespace adobe;
    
    if (!is_identifier(class_name)) return false;
    
    require_token(open_parenthesis_k);
    if (!is_named_argument_list(arguments)) push_back(arguments, dictionary_t());
    require_token(close_parenthesis_k);
    
    return true;
}

/*************************************************************************************************/
    
void eve_parser::require_view_statement_list(const position_t& location)
{
    using namespace adobe;
    
    require_token(open_brace_k);
    is_view_statement_sequence(location);
    require_token(close_brace_k);
}

/*************************************************************************************************/

} // namespace

namespace adobe {

/*************************************************************************************************/

line_position_t parse(std::istream& in,
        const line_position_t& line_position,
        const eve_callback_suite_t::position_t& position,
        const eve_callback_suite_t& assembler)
{
    eve_parser parser(assembler, in, line_position);
    parser.parse(position);
    return parser.next_position();
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
