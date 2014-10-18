/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifdef ADOBE_STD_SERIALIZATION

/*************************************************************************************************/

#include <string>

#include <boost/cstdint.hpp>

#include <adobe/iomanip_javascript.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>

using namespace std;

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

constexpr static_name_t javascript_name_k = "javascript"_name;

/*************************************************************************************************/

std::ostream& begin_javascript(std::ostream& os) {
    replace_pword<format_base, javascript_format>(os, format_base_idx());
    return os << begin_format;
}

/*************************************************************************************************/

std::ostream& end_javascript(std::ostream& os) { return os << end_format; }

/*************************************************************************************************/

void javascript_format::begin_format(stream_type& os) {
    push_stack(os, format_element_t(javascript_name_k));
}

/*************************************************************************************************/

void javascript_format::begin_bag(stream_type& os, const std::string& ident) {
    push_stack(os, format_element_t(bag_name_g, ident));
}

/*************************************************************************************************/

void javascript_format::begin_sequence(stream_type& os) {
    push_stack(os, format_element_t(seq_name_g));
}

/*************************************************************************************************/

void javascript_format::begin_atom(stream_type& os, const serializable_t& value) {
    push_stack(os, format_element_t(atom_name_g, value));
}

/*************************************************************************************************/

void javascript_format::stack_event(stream_type& os, bool is_push) {
    const format_element_t& top(stack_top());
    name_t self(top.tag());
    name_t parent(stack_depth() >= 2 ? stack_n(1).tag() : name_t());

    if (self == atom_name_g) {
        handle_atom(os, is_push);
    } else if (is_push) {
        if (self == javascript_name_k) {
        } else if (self == bag_name_g) {
            os << '{';

            up();
        } else if (self == seq_name_g && parent != bag_name_g) {
            os << "[ ";
        }
    } else {
        if (self == javascript_name_k) {
        } else if (self == bag_name_g) {
            down();

            if (top.num_out_m > 0)
                os << '\n' << indents(depth());
            else
                os << ' ';

            os << '}';
        } else if (self == seq_name_g && parent != bag_name_g) {
            if (top.num_out_m > 0)
                os << ' ';

            os << ']';
        }
    }
}

/*************************************************************************************************/

void javascript_format::handle_atom(stream_type& os, bool is_push) {
    const format_element_t& top(stack_top());
    name_t parent(stack_depth() >= 2 ? stack_n(1).tag() : name_t());
    name_t grandparent(stack_depth() >= 3 ? stack_n(2).tag() : name_t());
    const serializable_t& value(top.value());
    bool outputting_bag(parent == seq_name_g && grandparent == bag_name_g);
    std::size_t& num_out(outputting_bag ? stack_n(2).num_out_m : stack_n(1).num_out_m);
    bool named_argument(outputting_bag && num_out % 2 == 0);

    if (is_push) {
        // if this is not the first item in the element, add a comma and set up a newline
        if (num_out > 0) {
            if (!outputting_bag) {
                os << ", ";
            } else if (named_argument) {
                os << ",\n" << indents(depth());
            }
        } else if (outputting_bag) {
            os << '\n' << indents(depth());
        }

        if (value.type_info() == typeid(string)) {
            os << '\'' << value.cast<string>() << '\'';
        } else if (value.type_info() == typeid(name_t)) {
            if (!named_argument)
                os << "new name(\'";

            os << value.cast<name_t>();

            if (outputting_bag && named_argument)
                os << ": ";
            else
                os << "\')";
        } else if (value.type_info() == typeid(bool)) {
            os << (value.cast<bool>() ? "true" : "false");
        } else if (value.type_info() == typeid(double)) {
            double dbl_val(value.cast<double>());
            boost::int64_t int_val(static_cast<boost::int64_t>(dbl_val));

            if (dbl_val == int_val) {
                os << int_val;
            } else {
                // For javascript, we want to output floating-point values in decimal-based
                // fixed-point notation (asl_cel doesn't support any other format) with
                // a very high precision for accceptable roundtrip values.

                os.setf(std::ios_base::dec, std::ios_base::basefield);
                os.setf(std::ios_base::fixed, std::ios_base::floatfield);
                os.precision(16);

                os << dbl_val;
            }
        } else if (value.type_info() == typeid(empty_t)) {
            os << "nil";
        } else if (value.type_info() == typeid(dictionary_t)) {
            os << value.cast<dictionary_t>();
        } else if (value.type_info() == typeid(array_t)) {
            os << value.cast<array_t>();
        } else {
            os << "'javascript_unknown: " << value.type_info().name() << "'";
        }
    } else {
        // up the number of outputted items for the parent to this atom
        ++num_out;
    }
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
