/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/
#include <functional>

#include <adobe/adam_evaluate.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/name.hpp>

/**************************************************************************************************/

namespace ph = std::placeholders;

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

void add_cell(adobe::sheet_t& sheet, adobe::adam_callback_suite_t::cell_type_t type,
              adobe::name_t name, const adobe::line_position_t& position,
              const adobe::array_t& init_or_expr) {
    switch (type) {
    case adobe::adam_callback_suite_t::input_k:
        sheet.add_input(name, position, init_or_expr);
        break;

    case adobe::adam_callback_suite_t::output_k:
        sheet.add_output(name, position, init_or_expr);
        break;

    case adobe::adam_callback_suite_t::constant_k:
        sheet.add_constant(name, position, init_or_expr);
        break;

    case adobe::adam_callback_suite_t::logic_k:
        sheet.add_logic(name, position, init_or_expr);
        break;

    case adobe::adam_callback_suite_t::invariant_k:
        sheet.add_invariant(name, position, init_or_expr);
        break;

    default:
        assert(false); // Type not supported
    }
}

/**************************************************************************************************/

void add_relation(adobe::sheet_t& sheet, const adobe::line_position_t& position,
                  const adobe::array_t& conditional,
                  const adobe::adam_callback_suite_t::relation_t* first,
                  const adobe::adam_callback_suite_t::relation_t* last) {
    typedef std::vector<adobe::sheet_t::relation_t> relation_buffer_t;

    relation_buffer_t relations;

    relations.reserve(relation_buffer_t::size_type(last - first));

    /*
        REVISIT (sparent) : It would be nice to find a simple way to handle a transformed copy
        in a generic fashion when multiple members are needed.
    */

    while (first != last) // copy
    {
        relations.push_back(
            adobe::sheet_t::relation_t(first->name_set_m, first->position_m, first->expression_m));
        ++first;
    }

    sheet.add_relation(position, conditional, &relations[0], &relations[0] + relations.size());
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

adam_callback_suite_t bind_to_sheet(sheet_t& sheet) {
    adam_callback_suite_t suite;

    suite.add_cell_proc_m = std::bind(&add_cell, std::ref(sheet), ph::_1, ph::_2, ph::_3, ph::_4);
    suite.add_relation_proc_m = std::bind(&add_relation, std::ref(sheet), ph::_1, ph::_2, ph::_3, ph::_4);
    suite.add_interface_proc_m = [&sheet](name_t name, bool linked, const line_position_t& position1,
                                         const array_t& initializer, const line_position_t& position2,
                                         const array_t& expression, const std::string& brief,
                                         const std::string& detailed) -> void {
        sheet.add_interface(name, linked, position1, initializer, position2, expression);
    };
    return suite;
}

/**************************************************************************************************/

adam_callback_suite_t bind_to_sheet(sheet_t& sheet, external_model_t& external_model) {
    adam_callback_suite_t suite = bind_to_sheet(sheet);

    suite.add_external_proc_m =
        std::bind(&adobe::external_model_t::add_cell, std::ref(external_model), ph::_1);

    return suite;
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
