/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include "queryable_sheet.hpp"

#include "adam_test_parser.hpp"
#include "adam_test_parser_impl.hpp"

#include <boost/array.hpp>

#include <adobe/adam_evaluate.hpp>
#include <adobe/once.hpp>
#include "iomanip_flat.hpp"
#include <adobe/iomanip_asl_cel.hpp>
#include <adobe/implementation/token.hpp>
#include <adobe/algorithm/find.hpp>
#include <boost/fusion/iterator/deref.hpp>


/*************************************************************************************************/

namespace { void init_keyword_table(); }

ADOBE_ONCE_DECLARATION(adobe_adam_test_parser)
ADOBE_ONCE_DEFINITION(adobe_adam_test_parser, init_keyword_table)

/*************************************************************************************************/

namespace {

/*************************************************************************************************/

typedef boost::array<adobe::name_t, 7> keyword_table_t;

/*************************************************************************************************/

/*
    WARNING (mmarcus) : Initialization of these const_once_name_t items is defered until
    adam_test_parser::adam_test_parser().
*/

keyword_table_t*           keyword_table_g;
adobe::aggregate_name_t    update_k       = { "update" };
adobe::aggregate_name_t    reinitialize_k = { "reinitialize" };
adobe::aggregate_name_t    dump_k         = { "dump" };
adobe::aggregate_name_t    check_k        = { "check" };
adobe::aggregate_name_t    print_k        = { "print" };
adobe::aggregate_name_t    assert_k       = { "assert" };
adobe::aggregate_name_t    contributing_k = { "contributing" };


/*************************************************************************************************/

void init_keyword_table()
{
    static keyword_table_t keyword_table_s =
    {{
        assert_k,
        check_k,
        contributing_k,
        dump_k,
        print_k,
        reinitialize_k,
        update_k
    }};

    adobe::sort(keyword_table_s);

    keyword_table_g = &keyword_table_s;
}

void once_instance()
{
    ADOBE_ONCE_INSTANCE(adobe_adam_test_parser);
}

/*************************************************************************************************/

bool adam_test_keyword_lookup(const adobe::name_t& name)
{
    if(adobe::adam_keyword_lookup(name)) return true;
    keyword_table_t::const_iterator iter(adobe::lower_bound(*keyword_table_g, name));
    return (iter != keyword_table_g->end() && *iter == name);           
}

/*************************************************************************************************/
 
}

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

namespace implementation {

/*************************************************************************************************/

adam_test_parser::adam_test_parser(std::istream& in_stream, const line_position_t& position,  
                                   std::ostream& out) 
    : adam_parser(in_stream, position),
      out_m(out), all_checks_passed_m(true)  
{
    once_instance();
    set_keyword_extension_lookup(&adam_test_keyword_lookup);    
}


/*************************************************************************************************/

// namespace {

// inline name_t qs_name(const queryable_sheet_t* qs) { return qs->name(); }
//         adobe::find(sheets_m, boost::bind(&qs_name, _1) == sheet_name);
// }

queryable_sheet_t& adam_test_parser::sheet_from_name(name_t sheet_name)
{
    for(std::vector<queryable_sheet_t*>::iterator i=sheets_m.begin(), e=sheets_m.end(); 
        i != e; ++i) 
    {
        if((*i)->name() == sheet_name) return **i;
    }

    throw std::logic_error(make_string("Couldn't find sheet named ", sheet_name.c_str()));

}

/*************************************************************************************************/

// translation_unit        = interaction_list .
bool adam_test_parser::parse() 
{
    is_interaction_list();
    require_token(eof_k);
    return all_checks_passed_m;
}

/*************************************************************************************************/

// sheet .
bool adam_test_parser::is_sheet()
{
    if(!is_keyword(name_t("sheet"))) return false;
    putback();
    sheets_m.push_back(new queryable_sheet_t(*this));
    return true;

}

/*************************************************************************************************/

// interaction_list        = [ lead_comment_decl ] interaction [ ";" ] [ interaction_list ].
bool adam_test_parser::is_interaction_list()
{
// REVISIT (mmarcus) : fix up grammar here
    adobe::string_t comment;
    bool result(true);
    while(result)
    {
        while(is_lead_comment(comment))
            ;
        result = is_interaction();
        while(is_lead_comment(comment) || is_token(semicolon_k))
            ;
    }
    return result;
}

/*************************************************************************************************/

// interaction             = update_decl | reinitialize_decl | dump_decl | check_decl | 
//                           print_decl | assert_decl | contributing_decl |.
bool adam_test_parser::is_interaction()
{
    return is_update_decl() || is_reinitialize_decl() || is_dump_decl() || is_check_decl() ||
        is_print_decl() || is_assert_decl() || is_contributing_decl() || is_sheet();
}

/*************************************************************************************************/

// update_decl             = "update" "(" identifier "," dictionary ")".
bool adam_test_parser::is_update_decl()
{
    if(is_keyword(update_k)) {
        require_token(open_parenthesis_k);
        name_t name;
        if(!is_identifier(name))
            throw_exception("sheet name expected");
        queryable_sheet_t& qs(sheet_from_name(name));
        require_token(comma_k);
        array_t expression;
        if(!is_dictionary(expression))
            throw_exception("dictionary expected");
        require_token(close_parenthesis_k);        
        any_regular_t dict(qs.inspect(expression));
        dictionary_t d(dict.cast<adobe::dictionary_t>());
        qs.set(d);
        qs.update();
        return true;
    }
    return false;
}

/*************************************************************************************************/

// reinitialize_decl       = "reinitialize" "(" identifier "," dictionary ")".

bool adam_test_parser::is_reinitialize_decl()
{
    if(is_keyword(reinitialize_k)) {
        require_token(open_parenthesis_k);
        name_t name;
        if(!is_identifier(name))
            throw_exception("sheet name expected");
        queryable_sheet_t& qs(sheet_from_name(name));
        require_token(comma_k);
        array_t expression;
        if(!is_dictionary(expression))
            throw_exception("dictionary expected");
        require_token(close_parenthesis_k);
        any_regular_t dict = qs.inspect(expression);
        dictionary_t d(dict.cast<adobe::dictionary_t>());
        qs.set(d);
        qs.reinitialize();
        qs.update();
        return true;
    }
    return false;
}

/*************************************************************************************************/

void 
adam_test_parser::populate_dict(dictionary_t& dict, 
                                const queryable_sheet_t::index_t& index,
                                const queryable_sheet_t& qs,
                                bool want_contributors,
                                bool want_active) const
{
    for(queryable_sheet_t::index_t::iterator iter=index.begin(),  e=index.end(); iter != e; ++iter)
    {
        std::size_t i(iter->second);
            
        if(want_contributors)
        {
            dictionary_t cell_dict;
            cell_dict[name_t("contributors")] = any_regular_t(qs.cell_contributors()[i]);
            if(want_active)
            {
                cell_dict[name_t("active")] = any_regular_t(qs.cell_active()[i]);
                cell_dict[name_t("priority_accessed")] = any_regular_t(qs.cell_priority_accessed()[i]);
            }
            cell_dict[name_t("_value")] =  qs.cell_value()[i];
            dict[qs.cell_name()[i]] = any_regular_t(cell_dict);
            
        }
        else
            dict[qs.cell_name()[i]] =  qs.cell_value()[i];
    }
}

/*************************************************************************************************/

// dump_decl               = "dump" "(" identifier ")"
bool adam_test_parser::is_dump_decl()
{
    if(is_keyword(dump_k)) {
        require_token(open_parenthesis_k);
        name_t name;
        if(!is_identifier(name))
            throw_exception("sheet name expected");
        queryable_sheet_t& qs(sheet_from_name(name));        
        require_token(close_parenthesis_k);
        any_regular_t result;
#if defined(ADOBE_STD_SERIALIZATION)
        out_m << "\n### dump\nsheet " << name << " {\n";        
        out_m << "input:\n";
        
        {
            dictionary_t input_dict;
            populate_dict(input_dict, qs.input_index(), qs);
            out_m << begin_asl_cel << input_dict << end_asl_cel << std::endl;
        }

 
        out_m  << "interface:\n";
        {
            dictionary_t interface_dict;
            populate_dict(interface_dict, qs.interface_index(), qs, true, true);
            out_m << begin_asl_cel << interface_dict << end_asl_cel << std::endl;
        }


        out_m  << "output:\n";
        {
            dictionary_t output_dict;
            populate_dict(output_dict, qs.output_index(), qs, true);
            out_m << begin_asl_cel << output_dict << end_asl_cel << std::endl;
        }

        out_m  << "invariant:\n";
        {
            dictionary_t invariant_dict;
            populate_dict(invariant_dict, qs.invariant_index(), qs);
            out_m << begin_asl_cel << invariant_dict << end_asl_cel << std::endl;
        }
    
        out_m << "}" << std::endl;
#endif
        return true;
    }
    return false;
}

/*************************************************************************************************/

// check_decl              = "check" "(" identifier "," dictionary ")".
bool adam_test_parser::is_check_decl()
{
    if(is_keyword(check_k)) {
        require_token(open_parenthesis_k);
        name_t name;
        if(!is_identifier(name))
            throw_exception("sheet name expected");
        queryable_sheet_t& qs(sheet_from_name(name));
        require_token(comma_k);
        array_t expression;
        if(!is_dictionary(expression))
            throw_exception("dictionary expected");
        require_token(close_parenthesis_k);
        any_regular_t reg_dict = qs.inspect(expression);
        dictionary_t dict = reg_dict.cast<adobe::dictionary_t>();
        dictionary_t expected;
        populate_dict(expected, qs.output_index(), qs);
        populate_dict(expected, qs.interface_index(), qs);

        bool success = (expected == dict);
        if(success){
            out_m << "\n### check of sheet " << name << "  succeeded ###\n";
#if defined(ADOBE_STD_SERIALIZATION)
            out_m << "with: " << begin_asl_cel << dict << end_asl_cel << std::endl;
#endif
        } else {
            out_m << "\n### check of sheet " << name << " failed\n";
#if defined(ADOBE_STD_SERIALIZATION)
            out_m << "check expected: " << begin_asl_cel << dict << end_asl_cel << std::endl;
            out_m << "sheet cache contains: " << begin_asl_cel << expected << end_asl_cel 
                  << std::endl;
#endif
            all_checks_passed_m = false;
        }
        return true;
    }
    return false;
}

/*************************************************************************************************/

// print_decl              = "print" "(" identifier "," expression ")".
bool adam_test_parser::is_print_decl()
{
    if(is_keyword(print_k)) {
        require_token(open_parenthesis_k);
        name_t name;
        if(!is_identifier(name))
            throw_exception("sheet name expected");
        queryable_sheet_t& qs(sheet_from_name(name));
        require_token(comma_k);
        array_t expression;
        if(!is_expression(expression))
            throw_exception("expression expected");
        require_token(close_parenthesis_k);
        any_regular_t result = qs.inspect(expression);
#if defined(ADOBE_STD_SERIALIZATION)
        out_m << begin_asl_cel << expression << end_asl_cel << std::endl;
#endif
        return true;
    }
    return false;
}

/*************************************************************************************************/

// assert_decl             = "assert" "(" identifier "," expression ")".
bool adam_test_parser::is_assert_decl()
{
    if(is_keyword(assert_k)) {
        require_token(open_parenthesis_k);
        name_t name;
        if(!is_identifier(name))
            throw_exception("sheet name expected");
        queryable_sheet_t& qs(sheet_from_name(name));
        require_token(comma_k);
        array_t expression;
        if(!is_expression(expression))
            throw_exception("expression expected");
        require_token(close_parenthesis_k);
        any_regular_t result = qs.inspect(expression);
        bool success = result.cast<bool>();
        out_m <<"\n### assert " << (success ? std::string("succeeded: ") : std::string("failed: ")) 
#if defined(ADOBE_STD_SERIALIZATION)
              << begin_asl_cel << expression << end_asl_cel 
#endif        
              << " in sheet " << name << std::endl;
        if(!success) all_checks_passed_m = false;
        return true;
    }
    return false;
}

/*************************************************************************************************/

// contributing_decl       = "contributing" "(" identifier ")".
bool adam_test_parser::is_contributing_decl()
{
    if(is_keyword(contributing_k)) {
        require_token(open_parenthesis_k);
        name_t name;
        if(!is_identifier(name))
            throw_exception("sheet name expected");
        queryable_sheet_t& qs(sheet_from_name(name));        
        require_token(close_parenthesis_k);
        any_regular_t result;
        out_m << "\n### contributing of sheet " << name << "  ###\n";
#if defined(ADOBE_STD_SERIALIZATION)
        out_m << begin_asl_cel << qs.contributing()  << end_asl_cel << std::endl;
#endif
        return true; 
    }
    return false;
}

/*************************************************************************************************/

} //namespace implementation

/*************************************************************************************************/

bool parse(std::istream& in_stream, line_position_t line_pos, std::ostream& out)
{
    adobe::implementation::adam_test_parser p(in_stream, line_pos, out);
    return p.parse();
}

/*************************************************************************************************/

} //namespace adobe

/*************************************************************************************************/
