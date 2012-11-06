/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/implementation/adam_parser_impl.hpp>

#include <utility>
#include <istream>
#include <sstream>
#include <iomanip>
#include <cassert>

#include <boost/bind.hpp>
#include <boost/array.hpp>

#include <adobe/algorithm/binary_search.hpp>
#include <adobe/algorithm/lower_bound.hpp>
#include <adobe/algorithm/sort.hpp>
#include <adobe/algorithm/sorted.hpp>

#include <adobe/array.hpp>
#include <adobe/name.hpp>
#include <adobe/any_regular.hpp>
#include <adobe/dictionary.hpp>

#include <adobe/implementation/token.hpp>

/*************************************************************************************************/

namespace {

using namespace adobe;

/*************************************************************************************************/

aggregate_name_t    constant_k   = { "constant" };
aggregate_name_t    external_k   = { "external" };
aggregate_name_t    input_k      = { "input" };
aggregate_name_t    interface_k  = { "interface" };
aggregate_name_t    invariant_k  = { "invariant" };
aggregate_name_t    logic_k      = { "logic" };
aggregate_name_t    output_k     = { "output" };
aggregate_name_t    relate_k     = { "relate" };
aggregate_name_t    sheet_k      = { "sheet" };
aggregate_name_t    unlink_k     = { "unlink" };
aggregate_name_t    when_k       = { "when" };

aggregate_name_t keyword_table[] = {
    constant_k,
    external_k,
    input_k,
    interface_k,
    invariant_k,
    logic_k,
    output_k,
    relate_k,
    sheet_k,
    unlink_k,
    when_k,
};

/*************************************************************************************************/

bool keyword_lookup(const adobe::name_t& name)
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

} // namespace

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

adam_parser::adam_parser(std::istream& in, const line_position_t& position) :
    expression_parser(in, position)
{
    set_keyword_extension_lookup(boost::bind(&keyword_lookup, _1));
}

/*************************************************************************************************/

adam_parser::adam_parser(std::istream& in, const line_position_t& position, const adam_callback_suite_t& callbacks) :
    expression_parser(in, position), adam_callback_suite_m(callbacks)
{
    set_keyword_extension_lookup(boost::bind(&keyword_lookup, _1));
    
    assert(adam_callback_suite_m.add_cell_proc_m);      // all callbacks are required.
    assert(adam_callback_suite_m.add_relation_proc_m);
    assert(adam_callback_suite_m.add_interface_proc_m);
}

/*************************************************************************************************/

void parse(std::istream& stream, const line_position_t& position, const adam_callback_suite_t& callbacks)
{
    adam_parser(stream, position, callbacks).parse();
}

/*************************************************************************************************/

array_t parse_adam_expression(const std::string& str_expression)
{
    std::stringstream   expression_stream(str_expression);
    
    adobe::expression_parser parser(expression_stream, line_position_t("expression"));
    parser.set_keyword_extension_lookup(boost::bind(&keyword_lookup, _1));

    adobe::array_t expression;
    parser.require_expression(expression);
    
    return expression;
}

/*************************************************************************************************/

/*
    REVISIT (sparent) : sheets need to become copy-on-write and then this should return a list of
    sheets.
*/

void adam_parser::parse()
{
    name_t name;
    while (is_sheet_specifier(name)) ;
    require_token(eof_k);
}

/*************************************************************************************************/

//  sheet_specifier = [lead_comment] "sheet" identifier "{" { qualified_cell_decl } "}" [trail_comment].
bool adam_parser::is_sheet_specifier(name_t& name)
{
/* REVISIT (sparent) : Top level block is ignored. */

    is_token(lead_comment_k);
    if (!is_keyword(sheet_k)) return false;
    if(!is_identifier(name))
        throw_exception("sheet name identifier expected");
    require_token(open_brace_k);
    while (is_qualified_cell_decl()) { }
    require_token(close_brace_k);
    is_token(trail_comment_k);
    return true;
}

/*************************************************************************************************/

// qualified_cell_decl     = interface_set_decl | input_set_decl | output_set_decl
//                             | constant_set_decl | logic_set_decl | invariant_set_decl
//                             | external_set_decl.
bool adam_parser::is_qualified_cell_decl()
{
    if (    is_interface_set_decl()
            || is_input_set_decl()
            || is_output_set_decl()
            || is_constant_set_decl()
            || is_logic_set_decl()
            || is_invariant_set_decl()
            || is_external_set_decl()
        )
    {
        return true;
    }
    
    return false;
}
    
/*************************************************************************************************/

//  interface_set_decl      = "interface"   ":" { [lead_comment] interface_cell_decl }.
bool adam_parser::is_interface_set_decl()
{
    return is_set_decl(interface_k, &adam_parser::is_interface_cell_decl);
}
    
/*************************************************************************************************/
    
//  input_set_decl          = "input"       ":" { [lead_comment] input_cell_decl }.
bool adam_parser::is_input_set_decl()
{
    return is_set_decl(input_k, &adam_parser::is_input_cell_decl);
}

/*************************************************************************************************/
    
//  output_set_decl         = "output"      ":" { [lead_comment] output_cell_decl }.
bool adam_parser::is_output_set_decl()
{
    return is_set_decl(output_k, &adam_parser::is_output_cell_decl);
}

/*************************************************************************************************/
        
//  constant_set_decl       = "constant"    ":" { [lead_comment] constant_cell_decl }.
bool adam_parser::is_constant_set_decl()
{
    return is_set_decl(constant_k, &adam_parser::is_constant_cell_decl);
}

/*************************************************************************************************/
    
//  logic_set_decl          = "logic"       ":" { [lead_comment] logic_cell_decl }.
bool adam_parser::is_logic_set_decl()
{
    return is_set_decl(logic_k, &adam_parser::is_logic_cell_decl);
}

/*************************************************************************************************/
    
//  invariant_set_decl      = "invariant"   ":" { [lead_comment] invariant_cell_decl }.
bool adam_parser::is_invariant_set_decl()
{
    return is_set_decl(invariant_k, &adam_parser::is_invariant_cell_decl);
}

/*************************************************************************************************/
    
//  external_set_decl       = "external"    ":" { [lead_comment] identifier end_statement }.
bool adam_parser::is_external_set_decl()
{
    if (!is_keyword(external_k)) return false;
    
    require_token(colon_k);

    while (true)
    {
        string_t    detailed;
        string_t    brief;
        name_t      cell_name;
        
        (void)is_lead_comment(detailed);
        
        line_position_t position = next_position();
        if (!is_identifier(cell_name)) break;
        require_end_statement(brief);
        
        adam_callback_suite_m.add_external_proc_m(cell_name, position, brief, detailed);
    }
    
    return true;
}
    
/*************************************************************************************************/

//  interface_cell_decl     = ["unlink"] identifier [initializer] [define_expression] end_statement.
bool adam_parser::is_interface_cell_decl(const string_t& detailed)
{
    name_t          cell_name;
    array_t         initializer, expression;
    line_position_t initializer_position, expression_position;
    string_t     brief;

    bool linked (!is_keyword(unlink_k));
    
    if (!is_identifier(cell_name)) return false;
    
    (void)is_initializer(initializer_position, initializer);

    (void)is_define_expression(expression_position, expression);
    
    require_end_statement(brief);
        
    adam_callback_suite_m.add_interface_proc_m (cell_name, linked, initializer_position,
            initializer, expression_position, expression, brief, detailed);

    return true;
}
    
/*************************************************************************************************/

//  input_cell_decl         = identifier [initializer] end_statement.
bool adam_parser::is_input_cell_decl(const string_t& detailed)
{
    name_t          cell_name;
    array_t         initializer;
    line_position_t position;
    string_t     brief;
    
    if (!is_identifier(cell_name)) return false;
    
    (void)is_initializer(position, initializer);
    
    require_end_statement(brief);
    
    adam_callback_suite_m.add_cell_proc_m(adam_callback_suite_t::input_k, cell_name,
            position, initializer, brief, detailed);

    return true;
}
    
/*************************************************************************************************/

//  output_cell_decl        = named_decl.
bool adam_parser::is_output_cell_decl(const string_t& detailed)
{
    name_t          cell_name;
    line_position_t position;
    array_t         expression;
    string_t     brief; // REVISIT (fbreret) do something with me

    if (!is_named_decl(cell_name, position, expression, brief)) return false;

    adam_callback_suite_m.add_cell_proc_m(adam_callback_suite_t::output_k, cell_name,
            position, expression, brief, detailed);

    return true;
}

/*************************************************************************************************/

//  constant_cell_decl      = identifier initializer end_statement.
bool adam_parser::is_constant_cell_decl(const string_t& detailed)
{
    name_t          cell_name;
    line_position_t position;
    array_t         initializer;
    string_t     brief;

    if (!is_identifier(cell_name)) return false;
    if (!is_initializer(position, initializer)) throw_exception("initializer required");
    require_end_statement(brief);

    adam_callback_suite_m.add_cell_proc_m(adam_callback_suite_t::constant_k, cell_name,
            position, initializer, brief, detailed);

    return true;
}
        
/*************************************************************************************************/

//  logic_cell_decl         = named_decl | relate_decl.
bool adam_parser::is_logic_cell_decl(const string_t& detailed)
{
    name_t          cell_name;
    line_position_t position;
    array_t         expression;
    string_t     brief;

    relation_set_t  relations;
    
    if (is_named_decl(cell_name, position, expression, brief))
    {
        adam_callback_suite_m.add_cell_proc_m(adam_callback_suite_t::logic_k, cell_name,
                position, expression, brief, detailed);
        return true;
    }
    else if (is_relate_decl(position, expression, relations, brief))
    {
        adam_callback_suite_m.add_relation_proc_m(  position, expression, &relations.front(),
                                                    &relations.front() + relations.size(),
                                                    brief, detailed);
        return true;
    }
    return false;
}
    
/*************************************************************************************************/

//  invariant_cell_decl     = named_decl.
bool adam_parser::is_invariant_cell_decl(const string_t& detailed)
{
    name_t          cell_name;
    line_position_t position;
    array_t         expression;
    string_t     brief;

    if (!is_named_decl(cell_name, position, expression, brief)) return false;

    adam_callback_suite_m.add_cell_proc_m(adam_callback_suite_t::invariant_k, cell_name,
        position, expression, brief, detailed);

    return true;
}
    
/*************************************************************************************************/

//  relate_decl             = [conditional] "relate" "{" relate_expression relate_expression { relate_expression } "}" [trail_comment]
bool adam_parser::is_relate_decl(line_position_t& position, array_t& expression,
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

    relation_1 = relation_t();
    while (is_relate_expression_decl(relation_1))
    {
        relation_set.push_back(relation_1);
        relation_1 = relation_t();
    }

    require_token(close_brace_k);

    (void)is_trail_comment(brief);

    return true;
}
    
/*************************************************************************************************/

//  relate_expression       = [lead_comment] identifier { "," identifier } define_expression
//                              end_statement.
bool adam_parser::is_relate_expression_decl(relation_t& relation)
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
bool adam_parser::is_named_decl(name_t& cell_name, line_position_t& position, array_t& expression, string_t& brief)
{
    if (!is_identifier(cell_name)) return false;
    if (!is_define_expression(position, expression)) throw_exception("define_expression required"); 
    require_end_statement(brief);
    return true;
}
    
/*************************************************************************************************/

//  initializer             = ":" expression.
bool adam_parser::is_initializer(line_position_t& position, array_t& expression)
{
    if (!is_token(colon_k)) return false;
    
    position = next_position();
    require_expression(expression);

    return true;
}

/*************************************************************************************************/

//  define_expression       = "<==" expression.
bool adam_parser::is_define_expression(line_position_t& position, array_t& expression)
{
    if (!is_token(is_k)) return false;
    
    position = next_position();
    require_expression(expression);
    return true;
}

/*************************************************************************************************/

//  conditional = "when" "(" expression ")".

bool adam_parser::is_conditional(line_position_t& position, array_t& expression)
{
    if (!is_keyword(when_k)) return false;

    require_token(open_parenthesis_k);
    
    position = next_position();
    
    require_expression(expression);
    require_token(close_parenthesis_k);

    return true;
}

/*************************************************************************************************/

//  end_statement           = ";" [trail_comment].
void adam_parser::require_end_statement(string_t& brief)
{
    require_token(semicolon_k);
    (void)is_trail_comment(brief);
}
    
/*************************************************************************************************/

bool adam_parser::is_set_decl(name_t token, set_decl_t set_decl)
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

bool adam_keyword_lookup(const name_t& name)
{
    return keyword_lookup(name);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
