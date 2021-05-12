/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_EVE_EVALUATE_HPP
#define ADOBE_EVE_EVALUATE_HPP

#include <adobe/config.hpp>

#include <boost/function.hpp>

#include <adobe/dictionary_fwd.hpp>
#include <adobe/name.hpp>

#include <adobe/adam.hpp>
#include <adobe/eve_parser.hpp>
#include <adobe/layout_attributes.hpp>
#include <adobe/virtual_machine.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

typedef boost::function<eve_callback_suite_t::position_t(
    const eve_callback_suite_t::position_t& parent, name_t name, dictionary_t arguments)>
    bind_layout_proc_t;

eve_callback_suite_t bind_layout(const bind_layout_proc_t& proc, sheet_t& layout_sheet,
                                 virtual_machine_t& evaluator);

void apply_layout_parameters(layout_attributes_t& data, const dictionary_t& parameters);

adobe::any_regular_t layout_variables(adobe::sheet_t& layout_sheet, adobe::name_t name);

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
