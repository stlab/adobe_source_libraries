/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_VIRTUAL_MACHINE_HPP
#define ADOBE_VIRTUAL_MACHINE_HPP

#include <adobe/config.hpp>

#include <bitset>
#include <functional>
#include <vector>

#define BOOST_FUNCTION_NO_DEPRECATED
#include <boost/operators.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/array_fwd.hpp>
#include <adobe/dictionary_fwd.hpp>
#include <adobe/name.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

// todo: temporary hack to get around std::function not being comparable
class function_t : public std::function<any_regular_t(const any_regular_t&)> {
public:
    using base_t = std::function<any_regular_t(const any_regular_t&)>;
    using base_t::function;
    using base_t::operator=;
    using base_t::swap;
    using base_t::operator bool;
    using base_t::target;
    using base_t::target_type;

    function_t() noexcept = default;
    function_t(std::nullptr_t) noexcept : base_t(nullptr) {}
    function_t(const function_t&) = default;
    function_t(function_t&&) noexcept = default;
    template <class F, std::enable_if_t<!std::is_same_v<std::decay_t<F>, function_t>, int> = 0>
    function_t(F&& f) : base_t(std::forward<F>(f)) {}
    function_t& operator=(const function_t&) = default;
    function_t& operator=(function_t&&) noexcept = default;

    bool operator==(const function_t&) const { return true; }
    bool operator!=(const function_t&) const { return false; }
};

/**************************************************************************************************/
/*
    Note: For all bitwise operators the numeric data type (double) will be cast down to a
          std::uint32_t for the operation.
*/
class virtual_machine_t {
public:
    typedef array_t expression_t;

    typedef any_regular_t(variable_lookup_signature_t)(name_t);
    typedef any_regular_t(dictionary_function_lookup_signature_t)(name_t, const dictionary_t&);
    typedef any_regular_t(array_function_lookup_signature_t)(name_t, const array_t&);
    typedef any_regular_t(named_index_lookup_signature_t)(const any_regular_t&, name_t index);
    typedef any_regular_t(numeric_index_lookup_signature_t)(const any_regular_t&,
                                                            std::size_t index);

    using variable_lookup_t = std::function<variable_lookup_signature_t>;
    using dictionary_function_lookup_t = std::function<dictionary_function_lookup_signature_t>;
    using array_function_lookup_t = std::function<array_function_lookup_signature_t>;
    using named_index_lookup_t = std::function<named_index_lookup_signature_t>;
    using numeric_index_lookup_t = std::function<numeric_index_lookup_signature_t>;

    typedef any_regular_t(binary_op_override_signature_t)(const any_regular_t&,
                                                          const any_regular_t&);

    using binary_op_override_t = std::function<binary_op_override_signature_t>;

#if !defined(ADOBE_NO_DOCUMENTATION)
    virtual_machine_t();
    virtual_machine_t(const virtual_machine_t&);

    virtual_machine_t& operator=(const virtual_machine_t& rhs);

    ~virtual_machine_t();
#endif

    void evaluate(const expression_t& expression);
#if 0
    void evaluate_named_arguments(const dictionary_t&);
#endif

    const any_regular_t& back() const;
    any_regular_t& back();
    void pop_back();

    void set_variable_lookup(const variable_lookup_t&);
    void set_array_function_lookup(const array_function_lookup_t&);
    void set_dictionary_function_lookup(const dictionary_function_lookup_t&);
    void set_named_index_lookup(const named_index_lookup_t&);
    void set_numeric_index_lookup(const numeric_index_lookup_t&);

    void override_operator(name_t, const binary_op_override_t&);

    void add_variable(const name_t& name, const any_regular_t& value);

    class implementation_t;

private:
    implementation_t* object_m;
};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
