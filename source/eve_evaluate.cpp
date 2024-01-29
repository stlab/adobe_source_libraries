/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/eve_evaluate.hpp>

#include <functional>
#include <mutex>

#include <adobe/algorithm/sort.hpp>
#include <adobe/array.hpp>
#include <adobe/cassert.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/name.hpp>
#include <adobe/once.hpp>
#include <adobe/static_table.hpp>
#include <adobe/string.hpp>
#include <adobe/virtual_machine.hpp>

/**************************************************************************************************/

using namespace std;
using namespace std::placeholders;

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

typedef std::pair<adobe::name_t*, adobe::name_t*> reflected_table_range_t;
typedef adobe::static_table<adobe::name_t, adobe::layout_attributes_t::alignment_t, 7>
    alignment_table_t;
typedef adobe::static_table<adobe::name_t, adobe::layout_attributes_placement_t::placement_t, 3>
    placement_table_t;

/**************************************************************************************************/

using namespace adobe::literals;

adobe::static_name_t key_spacing = "spacing"_name;
adobe::static_name_t key_indent = "indent"_name;
adobe::static_name_t key_margin = "margin"_name;

adobe::static_name_t key_placement = "placement"_name;

adobe::static_name_t key_horizontal = "horizontal"_name;
adobe::static_name_t key_vertical = "vertical"_name;

adobe::static_name_t key_child_horizontal = "child_horizontal"_name;
adobe::static_name_t key_child_vertical = "child_vertical"_name;

adobe::static_name_t key_align_left = "align_left"_name;
adobe::static_name_t key_align_right = "align_right"_name;
adobe::static_name_t key_align_top = "align_top"_name;
adobe::static_name_t key_align_bottom = "align_bottom"_name;
adobe::static_name_t key_align_center = "align_center"_name;
adobe::static_name_t key_align_proportional = "align_proportional"_name;
adobe::static_name_t key_align_fill = "align_fill"_name;

adobe::static_name_t key_place_row = "place_row"_name;
adobe::static_name_t key_place_column = "place_column"_name;
adobe::static_name_t key_place_overlay = "place_overlay"_name;


adobe::static_name_t key_guide_mask = "guide_mask"_name;
adobe::static_name_t key_guide_balance = "guide_balance"_name;

adobe::static_name_t key_guide_baseline = "guide_baseline"_name;
adobe::static_name_t key_guide_label = "guide_label"_name;

// blank(guide_attach: {left: @guide_baseline});

alignment_table_t* alignment_table_g;
placement_table_t* placement_table_g;
reflected_table_range_t* reflected_range_g;

/**************************************************************************************************/

void init_once_() {
    static alignment_table_t alignment_table_s = {
        {alignment_table_t::entry_type(key_align_left, adobe::layout_attributes_t::align_left),
         alignment_table_t::entry_type(key_align_right, adobe::layout_attributes_t::align_right),
         alignment_table_t::entry_type(key_align_top, adobe::layout_attributes_t::align_top),
         alignment_table_t::entry_type(key_align_bottom, adobe::layout_attributes_t::align_bottom),
         alignment_table_t::entry_type(key_align_center, adobe::layout_attributes_t::align_center),
         alignment_table_t::entry_type(key_align_proportional,
                                       adobe::layout_attributes_t::align_proportional),
         alignment_table_t::entry_type(key_align_fill, adobe::layout_attributes_t::align_fill)}};

    static placement_table_t placement_table_s = {
        {placement_table_t::entry_type(key_place_row,
                                       adobe::layout_attributes_placement_t::place_row),
         placement_table_t::entry_type(key_place_column,
                                       adobe::layout_attributes_placement_t::place_column),
         placement_table_t::entry_type(key_place_overlay,
                                       adobe::layout_attributes_placement_t::place_overlay)}};

    alignment_table_s.sort();
    placement_table_s.sort();

    alignment_table_g = &alignment_table_s;
    placement_table_g = &placement_table_s;

    static adobe::name_t reflected[] = {key_align_left,   key_align_right,  key_align_top,
                                        key_align_bottom, key_align_center, key_align_proportional,
                                        key_align_fill,   key_place_row,    key_place_column,
                                        key_place_overlay};
    static reflected_table_range_t reflected_table_range_s;

    adobe::sort(reflected);

    reflected_table_range_s.first = boost::begin(reflected);
    reflected_table_range_s.second = boost::end(reflected);

    reflected_range_g = &reflected_table_range_s;
}

/**************************************************************************************************/

once_flag flag;
void init_once() { call_once(flag, &init_once_); }

/**************************************************************************************************/

adobe::dictionary_t evaluate_named_arguments(adobe::virtual_machine_t& evaluator,
                                             const adobe::array_t& arguments) {
    evaluator.evaluate(arguments);

    adobe::dictionary_t result(std::move(evaluator.back().cast<adobe::dictionary_t>()));

    evaluator.pop_back();
    return result;
}

/**************************************************************************************************/

void add_cell(adobe::sheet_t& sheet, adobe::eve_callback_suite_t::cell_type_t type,
              adobe::name_t name, const adobe::line_position_t& position,
              const adobe::array_t& init_or_expr) {
    switch (type) {
    case adobe::eve_callback_suite_t::constant_k:
        sheet.add_constant(name, position, init_or_expr);
        break;
    case adobe::eve_callback_suite_t::logic_k:
        sheet.add_logic(name, position, init_or_expr);
        break;
    default:
        ADOBE_ASSERT(false && "cell type not supported.");
    }
}

/**************************************************************************************************/

// REVISIT (sparent) : Copy & Paste from adam_evaluate.cpp

void add_relation(adobe::sheet_t& sheet, const adobe::line_position_t& position,
                  const adobe::array_t& conditional,
                  const adobe::eve_callback_suite_t::relation_t* first,
                  const adobe::eve_callback_suite_t::relation_t* last) {
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

eve_callback_suite_t bind_layout(const bind_layout_proc_t& proc, sheet_t& sheet,
                                 virtual_machine_t& evaluator) {
    init_once();

    eve_callback_suite_t suite;

    suite.add_view_proc_m = std::bind(
        proc, _1, _3, std::bind(&evaluate_named_arguments, std::ref(evaluator), _4));
    suite.add_cell_proc_m = std::bind(&add_cell, std::ref(sheet), _1, _2, _3, _4);
    suite.add_relation_proc_m = std::bind(&add_relation, std::ref(sheet), _1, _2, _3, _4);
    suite.add_interface_proc_m = [&sheet](name_t name, bool linked, const line_position_t& position1,
                                            const array_t& initializer, const line_position_t& position2,
                                            const array_t& expression, const std::string& brief,
                                            const std::string& detailed) -> void {
        sheet.add_interface(name, linked, position1, initializer, position2, expression);
    };
    suite.finalize_sheet_proc_m = std::bind(&sheet_t::update, std::ref(sheet));

    return suite;
}

/**************************************************************************************************/

void apply_layout_parameters(layout_attributes_t& data, const dictionary_t& parameters) {
    init_once();

    get_value(parameters, key_indent, data.indent_m);

    if (parameters.count(key_horizontal)) {
        (*alignment_table_g)(get_value(parameters, key_horizontal).cast<name_t>(),
                             data.slice_m[extents_slices_t::horizontal].alignment_m);
    }

    if (parameters.count(key_vertical)) {
        (*alignment_table_g)(get_value(parameters, key_vertical).cast<name_t>(),
                             data.slice_m[extents_slices_t::vertical].alignment_m);
    }

    // REVISIT (sparent) If we had named guides then named guide suppression would go here.

    if (parameters.count(key_guide_mask)) // an empty array would allow baselines out.
    {
        // Turn on all guides - then selectively suppress
        data.slice_m[extents_slices_t::vertical].suppress_m = false;
        data.slice_m[extents_slices_t::horizontal].suppress_m = false;

        array_t guide_mask(get_value(parameters, key_guide_mask).cast<array_t>());

        for (array_t::const_iterator iter(guide_mask.begin()), last(guide_mask.end()); iter != last;
             ++iter) {
            if (iter->cast<name_t>() == key_guide_baseline)
                data.slice_m[extents_slices_t::vertical].suppress_m = true;
            if (iter->cast<name_t>() == key_guide_label)
                data.slice_m[extents_slices_t::horizontal].suppress_m = true;
        }
    }

    // REVISIT (sparent) : If we had named guides then named guide balancing would go here...

    /*
        Balanced guides must be supressed to avoid having them attach to outside guides which could
        overconstrain the system.
    */

    if (parameters.count(key_guide_balance)) {
        data.slice_m[extents_slices_t::vertical].balance_m = false;
        data.slice_m[extents_slices_t::horizontal].balance_m = false;

        array_t guide_balance(get_value(parameters, key_guide_balance).cast<array_t>());

        for (array_t::const_iterator iter(guide_balance.begin()), last(guide_balance.end());
             iter != last; ++iter) {
            if (iter->cast<name_t>() == key_guide_baseline) {
                data.slice_m[extents_slices_t::vertical].balance_m = true;
                data.slice_m[extents_slices_t::vertical].suppress_m = true;
            }
            if (iter->cast<name_t>() == key_guide_label) {
                data.slice_m[extents_slices_t::horizontal].balance_m = true;
                data.slice_m[extents_slices_t::horizontal].suppress_m = true;
            }
        }
    }


    // REVISIT (sparent) : I'm seeing a pattern here - with three cases this could be factored

    {
        dictionary_t::const_iterator iter(parameters.find(key_placement));

        if (iter != parameters.end()) {
            (*placement_table_g)(iter->second.cast<name_t>(), data.placement_m);
        }

        // Adjust defaults

        // Specifying a row from the parameters implies enabling baselines unless otherwise
        // specified.

        if (iter != parameters.end() &&
            data.placement_m == layout_attributes_placement_t::place_row &&
            !parameters.count(key_guide_mask)) {
            data.slice_m[extents_slices_t::vertical].suppress_m = false;
        }
    }

    {
        dictionary_t::const_iterator iter(parameters.find(key_child_horizontal));

        if (iter != parameters.end()) {
            (*alignment_table_g)(iter->second.cast<name_t>(),
                                 data.slice_m[extents_slices_t::horizontal].child_alignment_m);
        }
    }

    {
        dictionary_t::const_iterator iter(parameters.find(key_child_vertical));

        if (iter != parameters.end()) {
            (*alignment_table_g)(iter->second.cast<name_t>(),
                                 data.slice_m[extents_slices_t::vertical].child_alignment_m);
        }
    }

    // spacing
    {
        dictionary_t::const_iterator iter(parameters.find(key_spacing));
        if (iter != parameters.end()) {
            if (iter->second.type_info() == typeid(array_t)) {
                const array_t& spacing_array = iter->second.cast<array_t>();
                data.spacing_m.resize(spacing_array.size() + 1);

                layout_attributes_t::spacing_t::iterator dest_iter(data.spacing_m.begin() + 1);

                for (array_t::const_iterator iter(spacing_array.begin());
                     iter != spacing_array.end(); ++iter) {
                    *dest_iter = iter->cast<int>();
                    ++dest_iter;
                }
            } else {
                double tmp(data.spacing_m[1]);
                iter->second.cast(tmp); // Try getting as number
                data.spacing_m[1] = static_cast<int>(tmp);
            }
        }
    }

    // margin
    {
        dictionary_t::const_iterator iter(parameters.find(key_margin));
        if (iter != parameters.end()) {
            if (iter->second.type_info() == typeid(array_t)) {
                const array_t& margin_array = iter->second.cast<array_t>();

                data.vertical().margin_m.first = margin_array[0].cast<int>();
                data.horizontal().margin_m.first = margin_array[1].cast<int>();
                data.vertical().margin_m.second = margin_array[2].cast<int>();
                data.horizontal().margin_m.second = margin_array[3].cast<int>();
            } else {
                int margin = iter->second.cast<int>();

                data.vertical().margin_m.first = margin;
                data.horizontal().margin_m.first = margin;
                data.vertical().margin_m.second = margin;
                data.horizontal().margin_m.second = margin;
            }
        }
    }
}

/**************************************************************************************************/

adobe::any_regular_t layout_variables(adobe::sheet_t& layout_sheet, adobe::name_t name) {
    adobe::name_t* found(
        std::lower_bound(reflected_range_g->first, reflected_range_g->second, name));

    if (found != reflected_range_g->second && *found == name)
        return adobe::any_regular_t(name);

    return layout_sheet.get(name);
}


/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
