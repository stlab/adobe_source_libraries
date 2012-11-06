/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/external_model.hpp>

#include <adobe/algorithm/for_each.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/string.hpp>

#include <stdexcept>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

void external_model_t::add_cell(name_t name)
{
    cell_set_m.push_back(cell_t());
    index_m.insert(std::make_pair(name.c_str(), &cell_set_m.back()));
}

/*************************************************************************************************/

std::size_t external_model_t::count(name_t name) const
{ return index_m.count(name.c_str()); }

/*************************************************************************************************/

external_model_t::connection_t external_model_t::monitor(name_t name, const monitor_t& monitor)
{
    cell_t* cell = lookup(name);
    return (cell->monitor_m.connect(monitor));
}

/*************************************************************************************************/

void external_model_t::set(name_t name, const any_regular_t& value)
{
    cell_t* cell = lookup(name);
    cell->model_monitor_m(value);
}

/*************************************************************************************************/

void external_model_t::model_monitor(name_t name, const monitor_t& monitor)
{
    cell_t* cell = lookup(name);
    cell->model_monitor_m = monitor;
}

/*************************************************************************************************/

void external_model_t::model_set(name_t name, const any_regular_t& value)
{
    cell_t* cell = lookup(name);
    cell->monitor_m(value);
}

/*************************************************************************************************/

external_model_t::cell_t* external_model_t::lookup(name_t name)
{
    index_t::iterator iter(index_m.find(name.c_str()));

    if (iter == index_m.end())
    {
        std::string error("external_model_t cell does not exist: ");
        error << name.c_str();
        throw std::logic_error(error);
    }
    
    return iter->second;
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
