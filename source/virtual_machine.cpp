/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <boost/config.hpp>

#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <typeinfo>
#include <vector>

#include <boost/iterator/transform_iterator.hpp>

#include <adobe/algorithm/minmax.hpp>
#include <adobe/any_regular.hpp>
#include <adobe/array.hpp>
#include <adobe/cmath.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/empty.hpp>
#include <adobe/functional.hpp>
#include <adobe/implementation/expression_filter.hpp>
#include <adobe/implementation/token.hpp>
#include <adobe/localization.hpp>
#include <adobe/name.hpp>
#include <adobe/once.hpp>
#include <adobe/static_table.hpp>
#include <adobe/string.hpp>
#include <adobe/virtual_machine.hpp>

#include <adobe/iomanip_asl_cel.hpp>

/**************************************************************************************************/

using namespace std;
using namespace std::placeholders;
using namespace adobe;

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

template <typename ValueType>
struct static_table_traits<const std::type_info*, ValueType> {
    typedef bool result_type;
    typedef const std::type_info* key_type;
    typedef ValueType value_type;
    typedef std::pair<key_type, value_type> entry_type;

    result_type operator()(const entry_type& x, const entry_type& y) const {
        return (*this)(x, y.first);
    }

    // revisit: MM. For debugging purposes, VC 8 requires the definition of
    // this (unnecessary overload) in debug versions.
    result_type operator()(const key_type& x, const entry_type& y) const {
        return !(operator()(y, x)) && !equal(x, y.first);
    }

    result_type operator()(const entry_type& x, const key_type& y) const {
        return x.first->before(*y) != 0;
    }

    result_type equal(const key_type& x, const key_type& y) const { return *x == *y; }
};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

using namespace adobe::literals;

typedef void (adobe::virtual_machine_t::implementation_t::*operator_t)();
using array_function_t = std::function<adobe::any_regular_t(const adobe::array_t&)>;
using dictionary_function_t = std::function<adobe::any_regular_t(const adobe::dictionary_t&)>;

typedef vector<adobe::any_regular_t> stack_type; // REVISIT (sparent) : GCC 3.1 the symbol stack_t
// conflicts with a symbol in signal.h

#if !defined(ADOBE_NO_DOCUMENTATION)

typedef adobe::static_table<adobe::name_t, operator_t, 27> operator_table_t;
using variable_table_t = adobe::static_table<adobe::name_t, adobe::any_regular_t, 9>;
using type_table_t = adobe::static_table<const std::type_info*, adobe::name_t, 8>;

#endif // !defined(ADOBE_NO_DOCUMENTATION)

/**************************************************************************************************/

template <typename Result>
struct make {
    typedef Result result_type;

    template <typename T>
    Result operator()(const T& x) {
        return Result(x);
    }
};

/**************************************************************************************************/

name_t known_type_name(const std::type_info& type) {
    static type_table_t type_table = [] {
        type_table_t result = {
            {type_table_t::entry_type(&typeid(double), "number"_name),
             type_table_t::entry_type(&typeid(bool), "boolean"_name),
             type_table_t::entry_type(&typeid(adobe::empty_t), "empty"_name),
             type_table_t::entry_type(&typeid(string), "string"_name),
             type_table_t::entry_type(&typeid(adobe::array_t), "array"_name),
             type_table_t::entry_type(&typeid(adobe::dictionary_t), "dictionary"_name),
             type_table_t::entry_type(&typeid(adobe::function_t), "function"_name),
             type_table_t::entry_type(&typeid(adobe::name_t), "name"_name)}};

        result.sort();
        return result;
    }();

    adobe::name_t result;
    type_table(&type, result);
    return result;
}

adobe::name_t type_name(const adobe::any_regular_t& val) {
    if (name_t result = known_type_name(val.type_info()); result)
        return result;
    return name_t{val.type_info().name()};
}

/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/

adobe::any_regular_t xml_escape_function(const adobe::any_regular_t& arg) {
    const auto& parameters{cast<adobe::array_t>(arg)};
    if (parameters.size() != 1 || parameters[0].type_info() != typeid(string))
        throw std::runtime_error("xml_escape: parameter error");

    return adobe::any_regular_t(adobe::entity_escape(cast<string>(parameters[0])));
}

/**************************************************************************************************/

adobe::any_regular_t xml_unescape_function(const adobe::any_regular_t& arg) {
    const auto& parameters{cast<adobe::array_t>(arg)};
    if (parameters.size() != 1 || parameters[0].type_info() != typeid(string))
        throw std::runtime_error("xml_unescape: parameter error");

    return adobe::any_regular_t(adobe::entity_unescape(cast<string>(parameters[0])));
}

/**************************************************************************************************/

adobe::any_regular_t localize_function(const adobe::any_regular_t& arg) {
    const auto& parameters{cast<adobe::array_t>(arg)};
    if (parameters.size() != 1)
        throw std::runtime_error("localize: parameter error");

    return adobe::any_regular_t(
        adobe::localization_ready()
            ? adobe::localization_invoke(cast<std::string>(parameters.front()))
            : cast<std::string>(parameters.front()));
}

/**************************************************************************************************/

adobe::any_regular_t round_function(const adobe::any_regular_t& arg) {
    const auto& parameters{cast<adobe::array_t>(arg)};
    if (parameters.size() == 0)
        throw std::runtime_error("round: parameter error");

    return adobe::any_regular_t(adobe::round(cast<double>(parameters.front())));
}

/**************************************************************************************************/

adobe::any_regular_t min_function(const adobe::any_regular_t& arg) {
    const auto& parameters{cast<adobe::array_t>(arg)};
    if (parameters.size() == 0)
        throw std::runtime_error("min: parameter error");

    return *adobe::min_element(parameters,
                               std::bind(std::less<double>(),
                                         std::bind(adobe::any_regular_t::transform<double>(), _1),
                                         std::bind(adobe::any_regular_t::transform<double>(), _2)));
}

/**************************************************************************************************/

adobe::any_regular_t max_function(const adobe::any_regular_t& arg) {
    const auto& parameters{cast<adobe::array_t>(arg)};
    if (parameters.size() == 0)
        throw std::runtime_error("max: parameter error");

    return *adobe::max_element(parameters,
                               std::bind(std::less<double>(),
                                         std::bind(adobe::any_regular_t::transform<double>(), _1),
                                         std::bind(adobe::any_regular_t::transform<double>(), _2)));
}

/**************************************************************************************************/

adobe::any_regular_t typeof_function(const adobe::any_regular_t& arg) {
    const auto& parameters{cast<adobe::array_t>(arg)};
    if (parameters.size() == 0)
        throw std::runtime_error("typeof: parameter error");

    return adobe::any_regular_t(type_name(parameters.front()));
}

/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/

adobe::any_regular_t scale_function(const adobe::any_regular_t& arg) {
    const auto& parameters{cast<adobe::dictionary_t>(arg)};
    double m(1.0);
    double x(0.0);
    double b(0.0);

    get_value(parameters, "m"_name, m);
    get_value(parameters, "x"_name, x);
    get_value(parameters, "b"_name, b);

    return adobe::any_regular_t(m * x + b);
}

/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/

[[noreturn]] void throw_function_not_defined(adobe::name_t function_name) {
    throw std::logic_error(
        adobe::make_string("function \'", function_name.c_str(), "\' not defined."));
}

[[noreturn]] void throw_variable_not_defined(adobe::name_t variable_name) {
    throw std::logic_error(
        adobe::make_string("variable \'", variable_name.c_str(), "\' not defined."));
}

/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/

struct bitwise_and_t {
    inline std::uint32_t operator()(std::uint32_t x, std::uint32_t y) const {
        return x & y;
    } // namespace
};

/**************************************************************************************************/

struct bitwise_xor_t {
    inline std::uint32_t operator()(std::uint32_t x, std::uint32_t y) const { return x ^ y; }
};

/**************************************************************************************************/

struct bitwise_or_t {
    inline std::uint32_t operator()(std::uint32_t x, std::uint32_t y) const { return x | y; }
};

/**************************************************************************************************/

struct bitwise_rshift_t {
    inline std::uint32_t operator()(std::uint32_t x, std::uint32_t y) const { return x >> y; }
};

/**************************************************************************************************/

struct bitwise_lshift_t {
    inline std::uint32_t operator()(std::uint32_t x, std::uint32_t y) const { return x << y; }
};

/**************************************************************************************************/

struct bitwise_negate_t {
    inline std::uint32_t operator()(std::uint32_t x) const { return ~x; }
};

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

const char* type_name(const std::type_info& type) {
    if (name_t result = known_type_name(type); result)
        return result.c_str();
    return type.name();
}

/**************************************************************************************************/

class virtual_machine_t::implementation_t {
    typedef std::map<adobe::name_t, binary_op_override_t> binary_op_override_map_t;

public:
    implementation_t();

    void evaluate(const array_t& expression);

    const any_regular_t& back() const;
    any_regular_t& back();
    void pop_back();

    template <class T>
    auto pop_as() -> T {
        if constexpr (std::is_same_v<T, any_regular_t>) {
            auto result = std::move(back());
            pop_back();
            return result;
        } else {
            auto result = std::move(cast<T>(back()));
            pop_back();
            return result;
        }
    }


    vector<variable_scope_t> variable_scope_m;

    void push_scope(variable_scope_t&& scope) { variable_scope_m.push_back(std::move(scope)); }

    variable_lookup_t variable_lookup_m;
#if 0
    array_function_lookup_t array_function_lookup_m;
    dictionary_function_lookup_t dictionary_function_lookup_m;
#endif
    named_index_lookup_t named_index_lookup_m;
    numeric_index_lookup_t numeric_index_lookup_m;

    // override maps
    binary_op_override_map_t binary_op_override_map_m;

    any_regular_t variable_lookup(name_t name) const {

        for (auto iter = variable_scope_m.rbegin(); iter != variable_scope_m.rend(); ++iter) {
            if (auto result = (*iter)(name))
                return *result;
        }

        throw_variable_not_defined(name);
    }

private:
    stack_type value_stack_m;

    static operator_t find_operator(adobe::name_t oper);

public:
    // logical and operator function functions

    template <template <class T> class Operator, class OperandType>
    void unary_operator();

    template <template <class T> class Operator, class OperandType>
    void binary_operator();

    template <typename BitwiseOp>
    void bitwise_unary_operator();

    template <typename BitwiseOp>
    void bitwise_binary_operator();

    bool operator_override(adobe::name_t name);

    void logical_operator(bool do_and);
    void logical_and_operator();
    void logical_or_operator();
    void index_operator();
    void ifelse_operator();
    void variable_operator();
    void function_operator();
    void array_operator();
    void dictionary_operator();

    static operator_table_t* operator_table_g;
    static variable_table_t* variable_table_g;
#if 0
    static array_function_table_t* array_function_table_g;
    static dictionary_function_table_t* dictionary_function_table_g;
#endif
};

/**************************************************************************************************/

operator_table_t* virtual_machine_t::implementation_t::operator_table_g;
variable_table_t* virtual_machine_t::implementation_t::variable_table_g;
#if 0
array_function_table_t* virtual_machine_t::implementation_t::array_function_table_g;
dictionary_function_table_t* virtual_machine_t::implementation_t::dictionary_function_table_g;
#endif

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

void virtual_machine_init_() {
    typedef operator_table_t::entry_type op_entry_type;
    typedef adobe::virtual_machine_t::implementation_t implementation_t;

    static operator_table_t operator_table_s = {
        {op_entry_type(adobe::not_k, &implementation_t::unary_operator<std::logical_not, bool>),
         op_entry_type(adobe::unary_negate_k,
                       &implementation_t::unary_operator<std::negate, double>),
         op_entry_type(adobe::add_k, &implementation_t::binary_operator<std::plus, double>),
         op_entry_type(adobe::subtract_k, &implementation_t::binary_operator<std::minus, double>),
         op_entry_type(adobe::multiply_k,
                       &implementation_t::binary_operator<std::multiplies, double>),
         op_entry_type(adobe::modulus_k, &implementation_t::binary_operator<std::modulus, int>),
         op_entry_type(adobe::divide_k, &implementation_t::binary_operator<std::divides, double>),
         op_entry_type(adobe::less_k, &implementation_t::binary_operator<std::less, double>),
         op_entry_type(adobe::greater_k, &implementation_t::binary_operator<std::greater, double>),
         op_entry_type(adobe::less_equal_k,
                       &implementation_t::binary_operator<std::less_equal, double>),
         op_entry_type(adobe::greater_equal_k,
                       &implementation_t::binary_operator<std::greater_equal, double>),
         op_entry_type(adobe::equal_k,
                       &implementation_t::binary_operator<std::equal_to, adobe::any_regular_t>),
         op_entry_type(adobe::not_equal_k,
                       &implementation_t::binary_operator<std::not_equal_to, adobe::any_regular_t>),
         op_entry_type(adobe::ifelse_k, &implementation_t::ifelse_operator),
         op_entry_type(adobe::index_k, &implementation_t::index_operator),
         op_entry_type(adobe::function_k, &implementation_t::function_operator),
         op_entry_type(adobe::array_k, &implementation_t::array_operator),
         op_entry_type(adobe::dictionary_k, &implementation_t::dictionary_operator),
         op_entry_type(adobe::variable_k, &implementation_t::variable_operator),
         op_entry_type(adobe::and_k, &implementation_t::logical_and_operator),
         op_entry_type(adobe::or_k, &implementation_t::logical_or_operator),
         op_entry_type(adobe::bitwise_and_k,
                       &implementation_t::bitwise_binary_operator<bitwise_and_t>),
         op_entry_type(adobe::bitwise_xor_k,
                       &implementation_t::bitwise_binary_operator<bitwise_xor_t>),
         op_entry_type(adobe::bitwise_or_k,
                       &implementation_t::bitwise_binary_operator<bitwise_or_t>),
         op_entry_type(adobe::bitwise_rshift_k,
                       &implementation_t::bitwise_binary_operator<bitwise_rshift_t>),
         op_entry_type(adobe::bitwise_lshift_k,
                       &implementation_t::bitwise_binary_operator<bitwise_lshift_t>),
         op_entry_type(adobe::bitwise_negate_k,
                       &implementation_t::bitwise_unary_operator<bitwise_negate_t>)}};


    static variable_table_t variable_table_s = {
        {variable_table_t::entry_type("typeof"_name, function_t{&typeof_function}),
         variable_table_t::entry_type("min"_name, function_t{&min_function}),
         variable_table_t::entry_type("max"_name, function_t{&max_function}),
         variable_table_t::entry_type(
             "round"_name, make_function<double(double)>((double (*)(double))std::round)),
         variable_table_t::entry_type("localize"_name, function_t{&localize_function}),
         variable_table_t::entry_type("xml_escape"_name, function_t{&xml_escape_function}),
         variable_table_t::entry_type("xml_unescape"_name, function_t{&xml_unescape_function}),
         variable_table_t::entry_type("scale"_name, function_t{&scale_function}),
         variable_table_t::entry_type(
             "std"_name, dictionary_t{{"typeof"_name, function_t{&typeof_function}},
                                      {"min"_name, function_t{&min_function}},
                                      {"max"_name, function_t{&max_function}},
                                      {"round"_name, function_t{&round_function}},
                                      {"localize"_name, function_t{&localize_function}},
                                      {"xml_escape"_name, function_t{&xml_escape_function}},
                                      {"xml_unescape"_name, function_t{&xml_unescape_function}},
                                      {"scale"_name, function_t{&scale_function}}})}};


    operator_table_s.sort();
    variable_table_s.sort();

    adobe::virtual_machine_t::implementation_t::operator_table_g = &operator_table_s;
    adobe::virtual_machine_t::implementation_t::variable_table_g = &variable_table_s;
}

/**************************************************************************************************/

once_flag virtual_machine_init_flag;
void virtual_machine_init() { call_once(virtual_machine_init_flag, &virtual_machine_init_); }

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

virtual_machine_t::implementation_t::implementation_t() {
    virtual_machine_init();

    push_scope([this](name_t name) -> optional<any_regular_t> {
        // This section handles the legacy lookup mechanism. The old callback simply failed if not
        // found.

        if (any_regular_t value; (*variable_table_g)(name, value)) {
            // legacy functions sorted by name
            constexpr static_name_t legacy_functions_s[]{
                "localize"_name, "max"_name,    "min"_name,        "round"_name,
                "scale"_name,    "typeof"_name, "xml_escape"_name, "xml_unescape"_name};

            // todo: (sean-parent) This should report the source file/line/character when that
            // information is integrated into the virtual machine.
            if (binary_search(legacy_functions_s, name, less{}, make<name_t>{}) !=
                std::end(legacy_functions_s)) {
                clog << "warning: deprecated legacy function `" << name
                     << "` may hide a variable.\n";
                clog << "note: use `std." << name << "` for the function or rename the variable.\n";
            }
            return value;
        }

#if 0
        // This section handles the legacy function support. Functions were not first class and were
        // looked up after the arguments where parsed. To support the old API, we capture the
        // function name and return a function object that will try and lookup the function when
        // invoked. This has the unfortunate side effect that referencing an undefined variable will
        // fail late with an error "expected `<type>` found `function`."
        if (array_function_lookup_m || dictionary_function_lookup_m) {
            return function_t{[this, name](const any_regular_t& args) {
                if (array_function_lookup_m && args.type_info() == typeid(array_t)) {
                    try {
                        return array_function_lookup_m(name, cast<array_t>(args));
                    } catch (...) {
                    }
                }
                if (dictionary_function_lookup_m && args.type_info() == typeid(dictionary_t)) {
                    try {
                        return dictionary_function_lookup_m(name, cast<dictionary_t>(args));
                    } catch (...) {
                    }
                }
                throw_function_not_defined(name);
            }};
        }
#endif
        // The legacy variable lookup function must be last. It will throw if the variable is not
        // found. Because the throw may have terminated a evaluation within a sheet, we cannot
        // squelch the error and continue.
        if (variable_lookup_m) {
            return variable_lookup_m(name);
        }
        return nullopt;
    });
}

/**************************************************************************************************/

void virtual_machine_t::implementation_t::evaluate(const array_t& expression) {
    std::cout << begin_asl_cel << expression << end_asl_cel << std::endl;
    for (const auto& e : expression) {
        adobe::name_t op_name;

        e.cast(op_name);

        if (op_name && op_name.c_str()[0] == '.') {
            if (!operator_override(op_name))
                ((*this).*(find_operator(op_name)))();
        } else {
            value_stack_m.push_back(e);
        }
    }
}

/**************************************************************************************************/

const any_regular_t& virtual_machine_t::implementation_t::back() const {
    return value_stack_m.back();
}

/**************************************************************************************************/

any_regular_t& virtual_machine_t::implementation_t::back() { return value_stack_m.back(); }

/**************************************************************************************************/

void virtual_machine_t::implementation_t::pop_back() { value_stack_m.pop_back(); }

/**************************************************************************************************/

operator_t virtual_machine_t::implementation_t::find_operator(adobe::name_t oper) {
    virtual_machine_init();

    return (*operator_table_g)(oper);
}

/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/

template <template <class T> class Operator, class OperandType>
void virtual_machine_t::implementation_t::binary_operator() {
    typedef OperandType operand_t;
    typedef Operator<operand_t> operator_class;

    stack_type::iterator iter(
        value_stack_m.end()); // REVISIT (sparent) : GCC 3.1 requires :: qualifier

    adobe::any_regular_t& operand1 = *(iter - 2);
    adobe::any_regular_t& operand2 = *(iter - 1);

    operand1.assign(operator_class()(cast<operand_t>(operand1), cast<operand_t>(operand2)));

    pop_back();
}

/**************************************************************************************************/

bool virtual_machine_t::implementation_t::operator_override(adobe::name_t name) {
    binary_op_override_map_t::iterator binary_op_found(binary_op_override_map_m.find(name));
    bool overridden(false);

    if (binary_op_found != binary_op_override_map_m.end()) {
        stack_type::iterator iter(
            value_stack_m.end()); // REVISIT (sparent) : GCC 3.1 requires :: qualifier

        adobe::any_regular_t& operand1 = *(iter - 2);
        adobe::any_regular_t& operand2 = *(iter - 1);

        operand1.assign(binary_op_found->second(operand1, operand2));

        pop_back();

        overridden = true;
    }

    return overridden;
}

/**************************************************************************************************/

template <template <class T> class Operator, class OperandType>
void virtual_machine_t::implementation_t::unary_operator() {
    typedef OperandType operand_t;
    typedef Operator<operand_t> operator_class;

    stack_type::iterator iter(value_stack_m.end());

    adobe::any_regular_t& operand1 = *(iter - 1);

    operand1.assign(operator_class()(cast<operand_t>(operand1)));
}

/**************************************************************************************************/

void virtual_machine_t::implementation_t::logical_operator(bool do_and) {
    array_t operand_exp(pop_as<array_t>());
    bool operand1 = cast<bool>(back());

    if (operand1 == do_and) {
        pop_back();
        evaluate(operand_exp);

        any_regular_t& operand2(value_stack_m.back());

        if (operand2.type_info() != typeid(bool))
            throw std::bad_cast(); // todo: better error
    }
}

/**************************************************************************************************/

void virtual_machine_t::implementation_t::logical_and_operator() { logical_operator(true); }

/**************************************************************************************************/

void virtual_machine_t::implementation_t::logical_or_operator() { logical_operator(false); }

/**************************************************************************************************/

void virtual_machine_t::implementation_t::index_operator() {
    stack_type::iterator iter(value_stack_m.end());

    any_regular_t& operand1(*(iter - 2));
    any_regular_t& operand2(*(iter - 1));

    adobe::any_regular_t result;

    if (operand2.type_info() == typeid(adobe::name_t)) {
        adobe::name_t index(operand2.cast<adobe::name_t>());

        if (named_index_lookup_m) {
            result = named_index_lookup_m(operand1, index);
        } else {
            result = get_value(operand1.cast<adobe::dictionary_t>(), index);
        }
    } else {
        std::size_t index(static_cast<std::size_t>(operand2.cast<double>()));

        if (numeric_index_lookup_m) {
            result = numeric_index_lookup_m(operand1, index);
        } else {
            const array_t& array = operand1.cast<array_t>();

            if (!(index < array.size()))
                throw std::runtime_error("index: array index out of range");

            result = array[index];
        }
    }

    operand1 = result;

    pop_back();
}

/**************************************************************************************************/

void virtual_machine_t::implementation_t::ifelse_operator() {
    adobe::array_t else_exp(back().cast<adobe::array_t>());
    pop_back();
    adobe::array_t then_exp(back().cast<adobe::array_t>());
    pop_back();

    bool predicate(back().cast<bool>());
    pop_back();

    evaluate(predicate ? then_exp : else_exp);
}

/**************************************************************************************************/

void virtual_machine_t::implementation_t::variable_operator() {

    adobe::name_t variable(back().cast<adobe::name_t>());
    pop_back();

    value_stack_m.push_back(variable_lookup(variable));
}

/**************************************************************************************************/

#if 0

// This is good code - it was just overkill for this problem - we ended up generated a fair
// amount of code for this.

template <typename Iterator, typename UnaryFunction>
typename std::pair<boost::transform_iterator<UnaryFunction, Iterator>,
    boost::transform_iterator<UnaryFunction, Iterator> >
        make_transform_range(const Iterator& first, const Iterator& last, UnaryFunction f)
{
    typedef typename boost::transform_iterator<UnaryFunction, Iterator> transform_type;
    
    return std::make_pair(transform_type(first, f), transform_type(last, f));
}

template <class InputRange, class T, class BinaryOperation>
T accumulate(const InputRange& range, T init, BinaryOperation binary_op)
{
    return std::accumulate(boost::begin(range), boost::end(range), init, binary_op);
}
#endif

/**************************************************************************************************/

void virtual_machine_t::implementation_t::array_operator() {
    stack_type::difference_type count =
        static_cast<stack_type::difference_type>(back().cast<double>());

    pop_back();

    adobe::array_t result;

    for (stack_type::iterator first(value_stack_m.end() - count), last(value_stack_m.end());
         first != last; ++first) {
        result.push_back(std::move(*first));
    }

    value_stack_m.resize(value_stack_m.size() - count);
    value_stack_m.push_back(any_regular_t(std::move(result)));
}

/**************************************************************************************************/

void virtual_machine_t::implementation_t::dictionary_operator() {
    stack_type::difference_type count =
        2 * static_cast<stack_type::difference_type>(back().cast<double>());

    pop_back();

    adobe::dictionary_t result;

    stack_type::iterator first(value_stack_m.end() - count), last(value_stack_m.end());

    while (first != last) {
        name_t name = first->cast<adobe::name_t>();
        ++first;
        result.insert(make_pair(name, std::move(*first)));
        ++first;
    }

    value_stack_m.resize(value_stack_m.size() - count);
    value_stack_m.push_back(any_regular_t(std::move(result)));
}

/**************************************************************************************************/

template <typename BitwiseOp>
void virtual_machine_t::implementation_t::bitwise_binary_operator() {
    // We might be able to make some performance improvements here by referencing
    // our stack values by const ref and then being smart about when to pop the
    // values to which they point... when and if the vm becomes big enough of a
    // bottleneck to warrant the work.

    std::uint32_t operand2_value(0); // something reasonable

    if (back().type_info() == typeid(adobe::array_t)) {
        adobe::array_t operand2_exp(back().cast<adobe::array_t>());
        pop_back(); // pop operand2_exp

        evaluate(operand2_exp);
        adobe::any_regular_t operand2 = back();
        operand2_value = static_cast<std::uint32_t>(operand2.cast<double>());
    } else if (back().type_info() == typeid(double)) {
        operand2_value = static_cast<std::uint32_t>(back().cast<double>());
    } else {
        throw std::logic_error("unknown type");
    }

    pop_back(); // pop operand2

    adobe::any_regular_t& operand1 = back();
    std::uint32_t operand1_value(static_cast<std::uint32_t>(operand1.cast<double>()));

    std::uint32_t result(BitwiseOp()(operand1_value, operand2_value));
    operand1.assign(static_cast<double>(result)); // assign operand1 in-place
}

/**************************************************************************************************/

template <typename BitwiseOp>
void virtual_machine_t::implementation_t::bitwise_unary_operator() {
    adobe::any_regular_t& operand1 = back();
    std::uint32_t operand1_value(static_cast<std::uint32_t>(operand1.cast<double>()));
    std::uint32_t result(BitwiseOp()(operand1_value));

    operand1.assign(static_cast<double>(result)); // assign operand1 in-place
}

/**************************************************************************************************/

void virtual_machine_t::implementation_t::function_operator() {
    virtual_machine_init();

    auto argument{pop_as<any_regular_t>()};
    auto function{pop_as<function_t>()};
    value_stack_m.push_back(function(argument));

#if 0
    if (back().type_info() == typeid(adobe::array_t)) {
        // handle unnamed parameter functions
        array_function_t array_func;
        adobe::array_t arguments(back().cast<adobe::array_t>());

        // handle function lookup

        if ((*array_function_table_g)(function_name, array_func))
            value_stack_m.back() = array_func(arguments);
        else if (array_function_lookup_m)
            value_stack_m.back() = array_function_lookup_m(function_name, arguments);
        else
            throw_function_not_defined(function_name);
    } else {
        // handle named parameter functions
        dictionary_function_t dictionary_func;
        adobe::dictionary_t arguments(back().cast<adobe::dictionary_t>());

        if ((*dictionary_function_table_g)(function_name, dictionary_func))
            value_stack_m.back() = dictionary_func(arguments);
        else if (dictionary_function_lookup_m)
            value_stack_m.back() = dictionary_function_lookup_m(function_name, arguments);
        else
            throw_function_not_defined(function_name);
    }
#endif
}

/**************************************************************************************************/

#if 0
#pragma mark -
#endif

/**************************************************************************************************/
#if !defined(ADOBE_NO_DOCUMENTATION)

virtual_machine_t::virtual_machine_t() : object_m(new implementation_t) {}

virtual_machine_t::virtual_machine_t(const virtual_machine_t& rhs)
    : object_m(new implementation_t(*rhs.object_m)) {}

virtual_machine_t& virtual_machine_t::operator=(const virtual_machine_t& rhs) {
    *object_m = *(rhs.object_m);

    return *this;
}

virtual_machine_t::~virtual_machine_t() { delete object_m; }
#endif // !defined(ADOBE_NO_DOCUMENTATION)

/**************************************************************************************************/

void virtual_machine_t::set_variable_lookup(const variable_lookup_t& lookup) {
    object_m->variable_lookup_m = lookup;
}

/**************************************************************************************************/

void virtual_machine_t::push_scope(variable_scope_t&& scope) {
    object_m->push_scope(std::move(scope));
}

/**************************************************************************************************/

#if 0
void virtual_machine_t::set_array_function_lookup(const array_function_lookup_t& function) {
    object_m->array_function_lookup_m = function;
}
#endif

/**************************************************************************************************/

#if 0
void virtual_machine_t::set_dictionary_function_lookup(
    const dictionary_function_lookup_t& function) {
    object_m->dictionary_function_lookup_m = function;
}
#endif

/**************************************************************************************************/

void virtual_machine_t::set_named_index_lookup(const named_index_lookup_t& function) {
    object_m->named_index_lookup_m = function;
}

/**************************************************************************************************/

void virtual_machine_t::set_numeric_index_lookup(const numeric_index_lookup_t& function) {
    object_m->numeric_index_lookup_m = function;
}

/**************************************************************************************************/

void virtual_machine_t::override_operator(name_t operator_name,
                                          const binary_op_override_t& override) {
    object_m->binary_op_override_map_m[operator_name] = override;
}

/**************************************************************************************************/

void virtual_machine_t::evaluate(const expression_t& expression) { object_m->evaluate(expression); }

/**************************************************************************************************/

const any_regular_t& virtual_machine_t::back() const { return object_m->back(); }

/**************************************************************************************************/

any_regular_t& virtual_machine_t::back() { return object_m->back(); }

/**************************************************************************************************/

void virtual_machine_t::pop_back() { object_m->pop_back(); }

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
