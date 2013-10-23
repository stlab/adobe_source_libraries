/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ADAM_TEST_PARSER_IMPL_HPP
#define ADOBE_ADAM_TEST_PARSER_IMPL_HPP

#include <adobe/config.hpp>

#include <iosfwd>
#include <deque>

#include <adobe/implementation/adam_parser_impl.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/name.hpp>
#include <adobe/any_regular.hpp>
#include <adobe/closed_hash.hpp>
#include <adobe/table_index.hpp>

/*************************************************************************************************/

namespace adobe {

struct queryable_sheet_t;

/*************************************************************************************************/

namespace implementation {

/*************************************************************************************************/

/*

  A simple test language, extending adam and the CEL:

  update: set values from the dictionary, then call update.
  reinitialize: set values from the dictionary, then call reinitialize, then update.
  dump: dumps the current interface and output values to a stream.
  check: requires that the current interface and output values match those in the dictionary.
  print: prints value of an expression
  assert: asserts truth of an expression 


  \code

  translation_unit        = interaction_list .
  interaction_list        = interaction [ ";" ] [ interaction_list ].
  interaction             = update_decl | reinitialize_decl  
  | dump_decl | check_decl | print_decl | assert_decl 
  | contributing_decl | trailing_comment |  sheet .
  update_decl             = "update" "(" identifier "," dictionary ")".
  reinitialize_decl       = "reinitialize" "(" identifier "," dictionary ")".
  dump_decl               = "dump" "(" identifier ")"
  check_decl              = "check" "(" identifier "," dictionary ")".
  print_decl              = "print" "(" identifier "," expression ")".
  assert_decl             = "assert" "(" identifier "," expression ")".
  contributing_decl       = "contributing" "(" identifier ")".

  keywords                += "update" | "reinitialize" | "check" | "dump" | "print" | "assert"
  | "contributing" .

*/

/*************************************************************************************************/

class adam_test_parser : private adam_parser
{
public:
    adam_test_parser(std::istream& in, const line_position_t& position,  
                     std::ostream& out);

// translation_unit        =  interaction_list .
    bool parse();

// sheet
    bool is_sheet();

// interaction_list        = [ lead_comment_decl ] interaction [ ";" ] [ interaction_list ].
    bool is_interaction_list();

// interaction             = update_decl | reinitialize_decl | dump_decl | check_decl | 
//                           print_decl | assert_decl | sheet .
    bool is_interaction();

// update_decl             = "update" "(" identifier "," dictionary ")".
    bool is_update_decl();

// reinitialize_decl       = "reinitialize"  "(" identifier "," dictionary ")".
    bool is_reinitialize_decl();

// dump_decl               = "dump" "(" identifier ")"
    bool is_dump_decl();

// check_decl              = "check" "(" identifier "," dictionary ")".
    bool is_check_decl();

// print_decl              = "print" "(" identifier "," expression ")".
    bool is_print_decl();

// assert_decl             = "assert" "(" identifier "," expression ")".
    bool is_assert_decl();

// contributing_decl       = "contributing" "(" identifier ")".
    bool is_contributing_decl();

private:
    void populate_dict(dictionary_t& dict, 
                       const queryable_sheet_t::index_t& index, 
                       const queryable_sheet_t& qs,
                       bool want_contributors=false,
                       bool want_active=false) const;

    queryable_sheet_t& sheet_from_name(name_t name);

    
    std::ostream&                   out_m;
    std::vector<queryable_sheet_t*> sheets_m;
    bool                            all_checks_passed_m;
};


/*************************************************************************************************/

} // namespace implementation

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif
