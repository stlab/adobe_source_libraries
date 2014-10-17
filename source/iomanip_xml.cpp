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

#include <adobe/iomanip_xml.hpp>
#include <adobe/implementation/expression_filter.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>

using namespace std;

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

constexpr static_name_t xml_name_k = "xml"_name;

/*************************************************************************************************/

//!\ingroup manipulator
std::ostream& begin_xml(std::ostream& os) {
    replace_pword<format_base, xml_format>(os, format_base_idx());
    return os << begin_format;
}

/*************************************************************************************************/

//!\ingroup manipulator
std::ostream& end_xml(std::ostream& os) { return os << end_format; }

/*************************************************************************************************/

void xml_format::begin_format(stream_type& os) { push_stack(os, format_element_t(xml_name_k)); }

/*************************************************************************************************/

void xml_format::begin_bag(stream_type& os, const std::string& ident) {
    push_stack(os, format_element_t(bag_name_g, ident));
}

/*************************************************************************************************/

void xml_format::begin_sequence(stream_type& os) { push_stack(os, format_element_t(seq_name_g)); }

/*************************************************************************************************/

void xml_format::begin_atom(stream_type& os, const serializable_t& value) {
    push_stack(os, format_element_t(atom_name_g, value));
}

/*************************************************************************************************/

void xml_format::stack_event(stream_type& os, bool is_push) {
    const format_element_t& top(stack_top());
    name_t self(top.tag());
    name_t parent(stack_depth() >= 2 ? stack_n(1).tag() : name_t());

    if (is_push) {
        if (self == xml_name_k) {
            os << indents(depth()) << "<xml>\n";
        } else if (self == bag_name_g) {
            os << "<dict>\n";
            up();
        } else if (self == seq_name_g) {
            if (parent != bag_name_g) {
                os << "<array>\n";
                up();
            }
        } else if (self == atom_name_g) {
            handle_atom(os, is_push);
        }
    } else {
        if (self == xml_name_k) {
            os << indents(depth()) << "</xml>";
        } else if (self == bag_name_g) {
            down();
            os << indents(depth()) << "</dict>";
            if (parent != atom_name_g)
                os << "\n";
        } else if (self == seq_name_g) {
            if (parent != bag_name_g) {
                down();
                os << indents(depth()) << "</array>";
                if (parent != atom_name_g)
                    os << "\n";
            }
        } else if (self == atom_name_g) {
            handle_atom(os, is_push);
        }
    }
}

/*************************************************************************************************/

void xml_format::handle_atom(stream_type& os, bool is_push) {
    const format_element_t& top(stack_top());
    name_t self(top.tag());
    name_t parent(stack_depth() >= 2 ? stack_n(1).tag() : name_t());
    name_t grandparent(stack_depth() >= 3 ? stack_n(2).tag() : name_t());
    const serializable_t& value(top.value());

    if (is_push) {
        if (self != seq_name_g)
            os << indents(depth());

        if (parent == seq_name_g && grandparent == bag_name_g && stack_n(1).num_out_m == 0) {
            os << "<key>" << value.cast<adobe::name_t>().c_str() << "</key>";
        } else if (value.type_info() == typeid(string)) {
            os << "<string>" << entity_escape(value.cast<string>()) << "</string>";
        } else if (value.type_info() == typeid(name_t)) {
            os << "<ident>" << value.cast<adobe::name_t>() << "</ident>";
        } else if (value.type_info() == typeid(bool)) {
            os << "<bool>" << (value.cast<bool>() ? "true" : "false") << "</bool>";
        } else if (value.type_info() == typeid(double)) {
            double dbl_val(value.cast<double>());
            boost::int64_t int_val(static_cast<boost::int64_t>(dbl_val));

            if (dbl_val == int_val)
                os << "<integer>" << int_val << "</integer>";
            else
                os << "<double>" << dbl_val << "</double>";
        } else if (value.type_info() == typeid(empty_t)) {
            os << "<" << value.cast<empty_t>() << "/>";
        } else if (value.type_info() == typeid(dictionary_t)) {
            os << value.cast<dictionary_t>();
        } else if (value.type_info() == typeid(array_t)) {
            os << value.cast<array_t>();
        } else {
            os << "<xml_unknown>" << value.type_info().name() << "</xml_unknown>";
        }
    } else {
        if (parent == seq_name_g)
            stack_n(1).num_out_m++;

        if (self != seq_name_g)
            os << "\n";
    }
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
