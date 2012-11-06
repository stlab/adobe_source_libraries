/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/


#ifndef ADOBE_QUERUABLE_SHEET_HPP
#define ADOBE_QUERUABLE_SHEET_HPP

#include <adobe/config.hpp>

#include <deque>

#include <adobe/any_regular.hpp>
#include <adobe/name.hpp>
#include <adobe/adam.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/adam_evaluate.hpp>
#include <adobe/implementation/adam_parser_impl.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

struct queryable_sheet_t {
    typedef closed_hash_map<name_t, std::size_t> index_t;
 
    queryable_sheet_t(adam_parser& p);        

    void reinitialize();

    void update();

    dictionary_t 
    contributing();

    any_regular_t 
    inspect(const array_t& expression);

    void
    set(const dictionary_t& dict);

    name_t 
    name() const 
    { return sheet_name_m; }


    const std::deque<any_regular_t>&
    cell_value() const { return value_m; }

    const std::deque<name_t>&
    cell_name() const { return name_m; }

    const std::deque<bool>&
    cell_active() const { return active_m; }

    const std::deque<dictionary_t>&
    cell_contributors() const { return contributors_m; }

    const std::deque<bool>&
    cell_priority_accessed() const { return priority_accessed_m; }

    const closed_hash_map<name_t, std::size_t>&
    input_index() const { return input_index_m; }   

    const closed_hash_map<name_t, std::size_t>&
    interface_index() const { return interface_index_m; }   

    const closed_hash_map<name_t, std::size_t>&
    output_index() const { return output_index_m; } 

    const closed_hash_map<name_t, std::size_t>&
    invariant_index() const { return invariant_index_m; } 
  
private:
    void begin_monitoring();
    
    adam_callback_suite_t setup_callbacks();

    void add_interface_trap( 
        adam_callback_suite_t::add_interface_proc_t   original,
        name_t                                        cell_name,
        bool                                          linked,
        const line_position_t&                        position1,
        const array_t&                                initializer,
        const line_position_t&                        position2,
        const array_t&                                expression);

    void add_cell_trap(  
        adam_callback_suite_t::add_cell_proc_t        original,
        adam_callback_suite_t::cell_type_t            type,
        name_t                                        cell_name,
        const line_position_t&                        position,
        const array_t&                                expr_or_init);


    bool                                 no_pure_outputs_m;
    sheet_t                              sheet_m;
    name_t                               sheet_name_m;

    index_t                              input_index_m;
    index_t                              interface_index_m;
    index_t                              output_index_m;
    index_t                              invariant_index_m;
    
    std::deque<any_regular_t>            value_m;
    std::deque<name_t>                   name_m;
    std::deque<bool>                     active_m;
    std::deque<dictionary_t>             contributors_m;
    std::deque<bool>                     priority_accessed_m;

    adam_parser&                         parser_m;
};

/*************************************************************************************************/

}

/*************************************************************************************************/


#endif
