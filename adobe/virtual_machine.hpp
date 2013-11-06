/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_VIRTUAL_MACHINE_HPP
#define ADOBE_VIRTUAL_MACHINE_HPP

#include <adobe/config.hpp>

#include <bitset>
#include <vector>

#define BOOST_FUNCTION_NO_DEPRECATED
#include <boost/function.hpp>
#include <boost/operators.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/array_fwd.hpp>
#include <adobe/name.hpp>
#include <adobe/dictionary_fwd.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*
    Note: For all bitwise operators the numeric data type (double) will be cast down to a
          boost::uint32_t for the operation.
*/
class virtual_machine_t {
public:
    typedef array_t expression_t;

    typedef any_regular_t(variable_lookup_signature_t)(name_t);
    typedef any_regular_t(dictionary_function_lookup_signature_t)(name_t, const dictionary_t &);
    typedef any_regular_t(array_function_lookup_signature_t)(name_t, const array_t &);
    typedef any_regular_t(named_index_lookup_signature_t)(const any_regular_t &, name_t index);
    typedef any_regular_t(numeric_index_lookup_signature_t)(const any_regular_t &,
                                                            std::size_t index);

    typedef boost::function<variable_lookup_signature_t> variable_lookup_t;
    typedef boost::function<dictionary_function_lookup_signature_t> dictionary_function_lookup_t;
    typedef boost::function<array_function_lookup_signature_t> array_function_lookup_t;
    typedef boost::function<named_index_lookup_signature_t> named_index_lookup_t;
    typedef boost::function<numeric_index_lookup_signature_t> numeric_index_lookup_t;

    typedef any_regular_t(binary_op_override_signature_t)(const any_regular_t &,
                                                          const any_regular_t &);

    typedef boost::function<binary_op_override_signature_t> binary_op_override_t;

#if !defined(ADOBE_NO_DOCUMENTATION)
    virtual_machine_t();
    virtual_machine_t(const virtual_machine_t &);

    virtual_machine_t &operator=(const virtual_machine_t &rhs);

    ~virtual_machine_t();
#endif

    void evaluate(const expression_t &expression);
#if 0
    void evaluate_named_arguments(const dictionary_t&);
#endif

    const any_regular_t &back() const;
    any_regular_t &back();
    void pop_back();

    void set_variable_lookup(const variable_lookup_t &);
    void set_array_function_lookup(const array_function_lookup_t &);
    void set_dictionary_function_lookup(const dictionary_function_lookup_t &);
    void set_named_index_lookup(const named_index_lookup_t &);
    void set_numeric_index_lookup(const numeric_index_lookup_t &);

    void override_operator(name_t, const binary_op_override_t &);

    class implementation_t;

private:
    implementation_t *object_m;
};

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
