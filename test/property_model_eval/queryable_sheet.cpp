/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include "queryable_sheet.hpp"

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

queryable_sheet_t::queryable_sheet_t(adam_parser& p)
    : no_pure_outputs_m(true), parser_m(p) 
{
    //  attach the VM to the sheet.
    sheet_m.machine_m.set_variable_lookup(boost::bind(&adobe::sheet_t::get, &sheet_m, _1));
    
    parser_m.adam_callback_suite_m = setup_callbacks(); 
    (void) parser_m.is_sheet_specifier(sheet_name_m);
    begin_monitoring();
 
}

/*************************************************************************************************/

void queryable_sheet_t::begin_monitoring()
{
    name_t dummy("queryable_sheet_dummy_cell");
    sheet_m.add_interface(dummy, false, line_position_t(), array_t(), 
                            line_position_t(), array_t());

    update();


    for(queryable_sheet_t::index_t::iterator iter=interface_index_m.begin(), 
            e=interface_index_m.end();  iter != e; ++iter)
    {
        std::size_t i(iter->second);
        name_t& name(name_m[i]);
        sheet_m.monitor_enabled(name, NULL, NULL, boost::bind(adobe::assign(), _1, 
                                                 boost::ref(active_m[i]))); 

        std::vector<name_t> v;
        v.push_back(name);
        sheet_m.monitor_enabled(dummy, &v[0], 1 + &v[0], 
                                   boost::bind(adobe::assign(), _1, 
                                               boost::ref(priority_accessed_m[i]))); 

        sheet_m.monitor_value(name, boost::bind(adobe::assign(), _1, 
                                                boost::ref(value_m[i])));

        sheet_m.monitor_contributing(name, dictionary_t(), 
                                     boost::bind(adobe::assign(), _1, 
                                                 boost::ref(contributors_m[i])));
    }

    for(queryable_sheet_t::index_t::iterator iter=output_index_m.begin(), 
            e=output_index_m.end(); iter != e; ++iter)
    {
        std::size_t i(iter->second);
        sheet_m.monitor_value(name_m[i], boost::bind(adobe::assign(), _1, 
                                                     boost::ref(value_m[i])));

        sheet_m.monitor_contributing(name_m[i], dictionary_t(), 
                                     boost::bind(adobe::assign(), _1, 
                                                 boost::ref(contributors_m[i])));
    }
    for(queryable_sheet_t::index_t::iterator iter=invariant_index_m.begin(), 
            e=invariant_index_m.end(); iter != e; ++iter)
    {
        std::size_t i(iter->second);
        sheet_m.monitor_value(name_m[i], boost::bind(adobe::assign(), _1, 
                                                     boost::ref(value_m[i])));
    }
}

/*************************************************************************************************/

void queryable_sheet_t::update()
{
    sheet_m.update();
}


/*************************************************************************************************/

void queryable_sheet_t::reinitialize()
{
    sheet_m.reinitialize();
}

/*************************************************************************************************/

dictionary_t queryable_sheet_t::contributing()
{
    return sheet_m.contributing();
}


/*************************************************************************************************/

void queryable_sheet_t::set(const dictionary_t& d)
{
    
    sheet_m.set(d);
    for(dictionary_t::const_iterator i=d.begin(), e=d.end(); i != e; ++i)
    {
        queryable_sheet_t::index_t::iterator in_iter(input_index_m.find(i->first));
        if(in_iter != input_index_m.end())
            value_m[in_iter->second] = i->second;
    }

}

/*************************************************************************************************/

any_regular_t queryable_sheet_t::inspect(const array_t& expression)
{
    return sheet_m.inspect(expression);
}



/*************************************************************************************************/

adam_callback_suite_t queryable_sheet_t::setup_callbacks()
{
    adam_callback_suite_t callbacks(adobe::bind_to_sheet(sheet_m));
    callbacks.add_cell_proc_m = 
        boost::bind(&queryable_sheet_t::add_cell_trap, this, callbacks.add_cell_proc_m, 
                    _1, _2, _3, _4);
    callbacks.add_interface_proc_m =
        boost::bind(&queryable_sheet_t::add_interface_trap, this, 
                    callbacks.add_interface_proc_m, _1, _2, _3, _4, _5, _6);
    return callbacks;
}

/*************************************************************************************************/

/* 
   Place interface cells in the cache, as they are being added to the sheet.
*/

void queryable_sheet_t::add_interface_trap( 
    adam_callback_suite_t::add_interface_proc_t   original,
    name_t                                        cell_name,
    bool                                          linked,
    const line_position_t&                        position1,
    const array_t&                                initializer,
    const line_position_t&                        position2,
    const array_t&                                expression)
{
    original(cell_name, linked, position1, initializer, position2, expression,
             std::string(), std::string());


    if(interface_index_m.end() != interface_index_m.find(cell_name))
        throw stream_error_t("Duplicate interface cell name", position1);
    name_m.push_back(cell_name);
    interface_index_m[cell_name] = name_m.size() - 1;

    active_m.push_back(false);
    priority_accessed_m.push_back(false);
    value_m.push_back(any_regular_t());
    contributors_m.push_back(dictionary_t());
}

/*************************************************************************************************/

/* 
   Place output cells in the cache, as they are being added to the sheet.
*/

void queryable_sheet_t::add_cell_trap(  
    adam_callback_suite_t::add_cell_proc_t        original,
    adam_callback_suite_t::cell_type_t            type,
    name_t                                        cell_name,
    const line_position_t&                        position,
    const array_t&                                expr_or_init)
{
    original(type, cell_name, position, expr_or_init, std::string(), std::string());

    if(adam_callback_suite_t::input_k == type)
    {
        if(input_index_m.end() != input_index_m.find(cell_name))
            throw stream_error_t("Duplicate pure input cell name", position);
        name_m.push_back(cell_name);
        input_index_m[cell_name] = name_m.size() - 1;
        active_m.push_back(false);
        priority_accessed_m.push_back(false);
        if(expr_or_init.size())
        {
            sheet_m.machine_m.evaluate(expr_or_init);
            value_m.push_back(sheet_m.machine_m.back());
            sheet_m.machine_m.pop_back();
        }
        else
        {
            value_m.push_back(any_regular_t());
        }
        contributors_m.push_back(dictionary_t());

    } 
    else if(adam_callback_suite_t::output_k == type) 
    {
        no_pure_outputs_m = false;
        if(output_index_m.end() != output_index_m.find(cell_name))
            throw stream_error_t("Duplicate output cell name", position);
        name_m.push_back(cell_name);
        output_index_m[cell_name] = name_m.size() - 1;
        active_m.push_back(false);
        priority_accessed_m.push_back(false);
        value_m.push_back(any_regular_t());
        contributors_m.push_back(dictionary_t());
    }
    else if(adam_callback_suite_t::invariant_k == type) 
    {
        if(invariant_index_m.end() != invariant_index_m.find(cell_name))
            throw stream_error_t("Duplicate invariant cell name", position);
        name_m.push_back(cell_name);
        invariant_index_m[cell_name] = name_m.size() - 1;

        active_m.push_back(false);
        priority_accessed_m.push_back(false);
        value_m.push_back(any_regular_t());
        contributors_m.push_back(dictionary_t());
    }
}

/*************************************************************************************************/

}

/*************************************************************************************************/
