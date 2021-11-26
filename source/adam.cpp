/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/adam.hpp>

#include <deque>
#include <string>
#include <utility>
#include <vector>

#include <boost/bind/bind.hpp>
#include <boost/function.hpp>

#include <adobe/algorithm/find.hpp>
#include <adobe/algorithm/for_each.hpp>
#include <adobe/algorithm/sort.hpp>
#include <adobe/algorithm/transform.hpp>
#include <adobe/algorithm/unique.hpp>
#include <adobe/any_regular.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/name.hpp>

#include <adobe/functional.hpp>
#include <adobe/istream.hpp>
#include <adobe/table_index.hpp>
#include <adobe/virtual_machine.hpp>

#ifndef NDEBUG

#include <iostream>

#endif // NDEBUG

/**************************************************************************************************/

using namespace std;
using namespace boost::placeholders;

/**************************************************************************************************/

namespace anonymous_adam_cpp { // can't instantiate templates on types from real anonymous

/**************************************************************************************************/

#ifndef NDEBUG

struct check_reentrancy {
    check_reentrancy(bool& x) : check_m(x) {
        assert(!x && "FATAL (sparent) : Function Not Reentrant.");
        check_m = true;
    }
    ~check_reentrancy() { check_m = false; }

    bool& check_m;
};

#endif // NDEBUG

/**************************************************************************************************/

/*
    REVISIT (sparent) : Move to utility? This is generally useful to provide a copy for
    non-copyable types such as boost::signals2::signal<>.
*/

template <typename T> // T models default constructable
struct empty_copy : T {
    empty_copy() : T() {}
    empty_copy(const empty_copy&) : T() {}
    empty_copy& operator=(const empty_copy&) { return *this; }
};

/**************************************************************************************************/

typedef adobe::sheet_t sheet_t;

// This currently establishes an upperbound on the number of cells in the sheet at 1K.
typedef std::bitset<1024> cell_bits_t;
typedef int priority_t;

struct compare_contributing_t;

enum access_specifier_t {
    access_input,
    access_interface_input,
    access_interface_output,
    access_output,
    access_logic,
    access_constant,
    access_invariant
};

/**************************************************************************************************/

/*
    REVISIT (sparent) : A thought - this could be packaged as a general template function for
    converting exceptions on function object calls.
*/

/*
    REVIST (sparent) : Some version of MSVC didn't like function level try blocks. Need to test.
*/

void evaluate(adobe::virtual_machine_t& machine, const adobe::line_position_t& position,
              const adobe::array_t& expression)
#ifdef BOOST_MSVC
{
#endif
    try {
        machine.evaluate(expression);
    } catch (const std::exception& error) {
        throw adobe::stream_error_t(error, position);
    }
#ifdef BOOST_MSVC
}
#endif

/**************************************************************************************************/

struct scope_count : boost::noncopyable {
    scope_count(std::size_t& x) : value_m(x) { ++value_m; }
    ~scope_count() { --value_m; }

private:
    std::size_t& value_m;
};

template <typename T>
struct scope_value_t : boost::noncopyable {
    scope_value_t(T& x, const T& v) : value_m(x), store_m(x) { x = v; }
    ~scope_value_t() { value_m = store_m; }

private:
    T& value_m;
    T store_m;
};

/**************************************************************************************************/

} // namespace anonymous_adam_cpp
using namespace anonymous_adam_cpp;

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

class sheet_t::implementation_t : boost::noncopyable {
public:
    typedef sheet_t::connection_t connection_t;

    explicit implementation_t(virtual_machine_t& machine);

    any_regular_t inspect(const array_t& expression);

    void set(name_t, const any_regular_t&);   // input cell.
    void touch(const name_t*, const name_t*); // range of input cells.

    any_regular_t get(name_t);
    const any_regular_t& operator[](name_t) const;

    void add_input(name_t, const line_position_t&, const array_t& initializer);
    void add_output(name_t, const line_position_t&, const array_t& expression);
    void add_constant(name_t, const line_position_t&, const array_t& initializer);
    void add_constant(name_t, any_regular_t value);
    void add_logic(name_t, const line_position_t&, const array_t& expression);
    void add_invariant(name_t, const line_position_t&, const array_t& expression);
    void add_interface(name_t, bool linked, const line_position_t&, const array_t& initializer,
                       const line_position_t&, const array_t& expression);
    void add_interface(name_t, any_regular_t initial);

    void add_relation(const line_position_t&, const array_t& conditional, const relation_t* first,
                      const relation_t* last);
    connection_t monitor_value(name_t, const monitor_value_t&); // output only

    // input only
    connection_t monitor_enabled(name_t, const name_t* first, const name_t* last,
                                 const monitor_enabled_t&);


    connection_t monitor_contributing(name_t, const dictionary_t&, const monitor_contributing_t&);

#if 0
    connection_t    monitor_invariant_contributing(name_t invariant, const monitor_invariant_t&); 
// REVISIT (sparent) : UNIMPLEMENTED
#endif

    connection_t monitor_invariant_dependent(name_t invariant, const monitor_invariant_t&);

    bool has_input(name_t) const;
    bool has_output(name_t) const;

    void update();

    void reinitialize();

    void set(const dictionary_t& dictionary);
    // set input cells to corresponding values in dictionary.

    dictionary_t contributing(const dictionary_t&) const;
    // all contributing values that have changed since mark
    dictionary_t contributing_to_cell(name_t) const;

private:
    struct relation_cell_t;
    struct cell_t;

    typedef vector<relation_cell_t*> relation_index_t;
    typedef vector<relation_t> relation_set_t;

    struct relation_cell_t {
        relation_cell_t(const line_position_t& position, const array_t& conditional,
                        const relation_t* first, const relation_t* last)
            : resolved_m(false), position_m(position), conditional_m(conditional),
              terms_m(first, last) {}

        bool resolved_m;

        line_position_t position_m;
        array_t conditional_m;
        relation_set_t terms_m;
        vector<cell_t*> edges_m;

        // REVISIT (sparent) : There should be a function object to set members
        void clear_resolved() { resolved_m = false; }
    };

    struct cell_t {
        typedef boost::function<any_regular_t()> calculator_t;

        typedef empty_copy<boost::signals2::signal<void(bool)>> monitor_invariant_list_t;
        typedef empty_copy<boost::signals2::signal<void(const any_regular_t&)>>
            monitor_value_list_t;
        typedef empty_copy<boost::signals2::signal<void(const cell_bits_t&)>>
            monitor_contributing_list_t;

        cell_t(access_specifier_t specifier, name_t, const calculator_t& calculator,
               std::size_t cell_set_pos, cell_t*); // output
        cell_t(access_specifier_t specifier, name_t, any_regular_t,
               std::size_t cell_set_pos); // constant

        cell_t(name_t, any_regular_t, std::size_t cell_set_pos); // input cell
        cell_t(name_t, bool linked, const calculator_t& init_expression,
               std::size_t cell_set_pos); // interface cell (input)

#if 0
        // compiler generated.
        cell_t(const cell_t& x);
        cell_t& operator=(const cell_t& x);
#endif

        access_specifier_t specifier_m;
        name_t name_m;

        calculator_t calculator_m;

        bool linked_m;
        bool invariant_m;

        priority_t priority_m; // For linked input cells only - zero otherwise

        bool resolved_m;  // For interface cells only - false if cell hasn't been flowed
        bool evaluated_m; // true if cell has been calculated (or has no calculator).

        std::size_t relation_count_m;
        std::size_t initial_relation_count_m;

        bool dirty_m; // denotes change state_m value

        any_regular_t state_m;
        cell_bits_t contributing_m;
        cell_bits_t init_contributing_m;

        std::size_t cell_set_pos_m; // self index in sheet_t::cell_set_m

        calculator_t term_m;

        // For output half of interface cells this points to corresponding input half. NULL
        // otherwise.
        cell_t* interface_input_m;

        priority_t priority() const {
            assert(
                (specifier_m == access_interface_input || specifier_m == access_interface_output) &&
                "should not read priority of this cell type");
            return interface_input_m ? interface_input_m->priority_m : priority_m;
        }

        // For output half of interface cells this points to any possible connected relations.
        relation_index_t relation_index_m;

        monitor_value_list_t monitor_value_m;
        monitor_contributing_list_t monitor_contributing_m;
        monitor_invariant_list_t monitor_invariant_m;

        void calculate();

        void clear_dirty() {
            dirty_m = false;
            relation_count_m = initial_relation_count_m;
            term_m.clear();
            evaluated_m = specifier_m == access_input ||
                          specifier_m == access_constant /* || calculator_m.empty() */;

            /*
                REVISIT (sparent) : What exactly is the distinction between evaluated and resolved.
            */

            resolved_m = evaluated_m;
        }

        /*
            REVISIT (sparent) : A member wise implementation of swap would be better - but I'm
            going to swap this way for expendiancy since cell_t will likely change a lot when
            I get around to rewriting Adam.
        */
        // friend void swap(cell_t& x, cell_t&y) { std::swap(x, y); }
    };

    friend struct cell_t;
    friend struct compare_contributing_t;

    any_regular_t calculate_expression(const line_position_t& position, const array_t& expression);

    any_regular_t calculate_indexed(const line_position_t& position, const array_t& expression,
                                    std::size_t index) {
        return calculate_expression(position, expression).cast<array_t>()[index];
    }

    dictionary_t contributing_set(const dictionary_t&, const cell_bits_t&) const;

    void initialize_one(cell_t& cell);

    void enabled_filter(const cell_bits_t& touch_set, std::size_t contributing_index_pos,
                        monitor_enabled_t monitor, const cell_bits_t& new_priority_accessed_bits,
                        const cell_bits_t& new_active_bits);

    //    std::size_t cell_set_to_contributing(std::size_t cell_set_pos) const;

    priority_t name_to_priority(name_t name) const;
    void flow(cell_bits_t& priority_accessed);

    /*
        NOTE (sparent) : cell_t contains boost::signals2::signal<> which is not copyable. The cells
       support
        limited copying until they have monitors attached - this allows them to be placed into a
        container prior to any connections being made. A deque is used rather than a vector because
       it
        does not reallocate when it grows.
    */

    typedef std::deque<cell_t> cell_set_t;
    typedef std::deque<relation_cell_t> relation_cell_set_t;

    typedef std::vector<pair<name_t, bool>> get_stack_t;
    typedef std::vector<cell_t*> index_vector_t;

    typedef hash_index<cell_t, std::hash<name_t>, equal_to, mem_data_t<cell_t, const name_t>>
        index_t;

    index_t name_index_m;
    index_t setable_index_m; // input of interface or input;
    index_t input_index_m;
    index_t output_index_m;

    index_vector_t invariant_index_m;

    priority_t priority_high_m;
    priority_t priority_low_m;

    cell_bits_t conditional_indirect_contributing_m;

    virtual_machine_t& machine_m;
    get_stack_t get_stack_m;
    std::size_t get_count_m;

    cell_bits_t init_dirty_m;
    cell_bits_t priority_accessed_m;
    cell_bits_t value_accessed_m;
    cell_bits_t active_m;

    typedef boost::signals2::signal<void(const cell_bits_t&, const cell_bits_t&)>
        monitor_enabled_list_t;
    monitor_enabled_list_t monitor_enabled_m;

    cell_bits_t accumulate_contributing_m;

    bool has_output_m;      // true if there are any output cells.
    bool initialize_mode_m; // true during reinitialize call.

    // Actual cell storage - every thing else is index or state.

    cell_set_t cell_set_m;
    relation_cell_set_t relation_cell_set_m;

#ifndef NEBUG
    bool updated_m;
    bool check_update_reentrancy_m;
#endif
};

/**************************************************************************************************/

void sheet_t::implementation_t::enabled_filter(const cell_bits_t& touch_set,
                                               std::size_t contributing_index_pos,
                                               monitor_enabled_t monitor,
                                               const cell_bits_t& new_priority_accessed_bits,
                                               const cell_bits_t& new_active_bits) {
    cell_bits_t new_priority_accessed_touch = new_priority_accessed_bits & touch_set;
    cell_bits_t old_priority_accessed_touch = priority_accessed_m & touch_set;
    bool unchanged_priority_accessed_touch =
        (new_priority_accessed_touch ^ old_priority_accessed_touch).none();

    cell_t& cell = cell_set_m[contributing_index_pos];

    bool active(active_m.test(contributing_index_pos));
    bool new_active(new_active_bits.test(contributing_index_pos));

    /*
        REVIST <sean_parent@mac.com> : This is check seems to missing a check on value_accessed_m.
        A change there might go unnoticed and cause the control active state to go out of sync.
    */

    if (unchanged_priority_accessed_touch && (active == new_active))
        return;

    monitor(new_active ||
            (value_accessed_m.test(cell.cell_set_pos_m) && new_priority_accessed_touch.any()));
}

/**************************************************************************************************/

/*
    REVISIT (sparent) : Need to figure out what happens if this is called on an input cell during
    initialization (before it is resolved).
*/

void sheet_t::implementation_t::cell_t::calculate() {
    if (evaluated_m)
        return;
    // REVISIT (sparent) : review resolved_m resolved issue.
    // assert(resolved_m && "Cell in an invalid state?");

    // This is to handle conditionals which refer to cells involved in relate clauses
    if (relation_count_m)
        throw std::logic_error(
            make_string("cell ", name_m.c_str(), " is attached to an unresolved relate clause."));

    any_regular_t result = calculator_m();

    dirty_m = (result != state_m);
    state_m = std::move(result);
    evaluated_m = true;
}

/**************************************************************************************************/

sheet_t::implementation_t::cell_t::cell_t(name_t name, any_regular_t x, std::size_t cell_set_pos)
    : specifier_m(access_input), name_m(name), invariant_m(false), priority_m(0), resolved_m(true),
      evaluated_m(true), relation_count_m(0), initial_relation_count_m(0), dirty_m(false),
      state_m(std::move(x)), cell_set_pos_m(cell_set_pos), interface_input_m(0) {
    init_contributing_m.set(cell_set_pos);
}

/**************************************************************************************************/

sheet_t::implementation_t::cell_t::cell_t(name_t name, bool linked, const calculator_t& initializer,
                                          std::size_t cell_set_pos)
    : specifier_m(access_interface_input), name_m(name), calculator_m(initializer),
      linked_m(linked), invariant_m(false), priority_m(0), resolved_m(true), evaluated_m(true),
      relation_count_m(0), initial_relation_count_m(0), cell_set_pos_m(cell_set_pos),
      interface_input_m(0) {
    contributing_m.set(cell_set_pos);
}

/**************************************************************************************************/

sheet_t::implementation_t::cell_t::cell_t(access_specifier_t specifier, name_t name,
                                          const calculator_t& calculator, std::size_t cell_set_pos,
                                          cell_t* input)
    : specifier_m(specifier), name_m(name), calculator_m(calculator), linked_m(false),
      invariant_m(false), priority_m(0), resolved_m(false), evaluated_m(calculator_m.empty()),
      relation_count_m(0), initial_relation_count_m(0), cell_set_pos_m(cell_set_pos),
      interface_input_m(input) {}

/**************************************************************************************************/

sheet_t::implementation_t::cell_t::cell_t(access_specifier_t specifier, name_t name,
                                          any_regular_t x, std::size_t cell_set_pos)
    : specifier_m(specifier), name_m(name), linked_m(false), invariant_m(false), priority_m(0),
      resolved_m(true), evaluated_m(true), relation_count_m(0), initial_relation_count_m(0),
      state_m(std::move(x)), cell_set_pos_m(cell_set_pos), interface_input_m(0) {}

/**************************************************************************************************/

sheet_t::sheet_t() : object_m(new implementation_t(machine_m)) {}

sheet_t::~sheet_t() { delete object_m; }

any_regular_t sheet_t::inspect(const array_t& expression) { return object_m->inspect(expression); }

void sheet_t::set(name_t input, const any_regular_t& value) { object_m->set(input, value); }

void sheet_t::touch(const name_t* first, const name_t* last) { object_m->touch(first, last); }

void sheet_t::add_input(name_t input, const line_position_t& position, const array_t& initializer) {
    object_m->add_input(input, position, initializer);
}

void sheet_t::add_output(name_t output, const line_position_t& position,
                         const array_t& expression) {
    object_m->add_output(output, position, expression);
}

void sheet_t::add_constant(name_t constant, const line_position_t& position,
                           const array_t& initializer) {
    object_m->add_constant(constant, position, initializer);
}

void sheet_t::add_constant(name_t name, any_regular_t value) {
    object_m->add_constant(name, std::move(value));
}

void sheet_t::add_logic(name_t logic, const line_position_t& position, const array_t& expression) {
    object_m->add_logic(logic, position, expression);
}

void sheet_t::add_invariant(name_t invariant, const line_position_t& position,
                            const array_t& expression) {
    object_m->add_invariant(invariant, position, expression);
}

void sheet_t::add_interface(name_t name, bool linked, const line_position_t& position1,
                            const array_t& initializer, const line_position_t& position2,
                            const array_t& expression) {
    object_m->add_interface(name, linked, position1, initializer, position2, expression);
}

void sheet_t::add_interface(name_t name, any_regular_t initial) {
    object_m->add_interface(name, std::move(initial));
}

void sheet_t::add_relation(const line_position_t& position, const array_t& conditional,
                           const relation_t* first, const relation_t* last) {
    object_m->add_relation(position, conditional, first, last);
}

sheet_t::connection_t sheet_t::monitor_value(name_t output, const monitor_value_t& monitor) {
    return object_m->monitor_value(output, monitor);
}

sheet_t::connection_t sheet_t::monitor_contributing(name_t output, const dictionary_t& mark,
                                                    const monitor_contributing_t& monitor) {
    return object_m->monitor_contributing(output, mark, monitor);
}

sheet_t::connection_t sheet_t::monitor_enabled(name_t input, const name_t* first,
                                               const name_t* last,
                                               const monitor_enabled_t& monitor) {
    return object_m->monitor_enabled(input, first, last, monitor);
}

#if 0
sheet_t::connection_t sheet_t::monitor_invariant_contributing(name_t input, 
                                                              const monitor_invariant_t& monitor)
{ return object_m->monitor_invariant_contributing(input, monitor); }
#endif

sheet_t::connection_t sheet_t::monitor_invariant_dependent(name_t output,
                                                           const monitor_invariant_t& monitor) {
    return object_m->monitor_invariant_dependent(output, monitor);
}

bool sheet_t::has_input(name_t name) const { return object_m->has_input(name); }

bool sheet_t::has_output(name_t name) const { return object_m->has_output(name); }

void sheet_t::update() { object_m->update(); }

void sheet_t::reinitialize() { object_m->reinitialize(); }

void sheet_t::set(const dictionary_t& dictionary) { object_m->set(dictionary); }

any_regular_t sheet_t::get(name_t cell) { return object_m->get(cell); }

const any_regular_t& sheet_t::operator[](name_t x) const { return (*object_m)[x]; }

dictionary_t sheet_t::contributing(const dictionary_t& mark) const {
    return object_m->contributing(mark);
}

dictionary_t sheet_t::contributing() const { return object_m->contributing(dictionary_t()); }

dictionary_t sheet_t::contributing_to_cell(name_t x) const {
    return object_m->contributing_to_cell(x);
}

/**************************************************************************************************/

sheet_t::implementation_t::implementation_t(virtual_machine_t& machine)
    : name_index_m(std::hash<name_t>(), equal_to(), &cell_t::name_m),
      input_index_m(std::hash<name_t>(), equal_to(), &cell_t::name_m),
      output_index_m(std::hash<name_t>(), equal_to(), &cell_t::name_m), priority_high_m(0),
      priority_low_m(0), machine_m(machine), get_count_m(0), has_output_m(false),
      initialize_mode_m(false)
#ifndef NDEBUG
      ,
      updated_m(false), check_update_reentrancy_m(false)
#endif
{
}

/**************************************************************************************************/

any_regular_t sheet_t::implementation_t::inspect(const array_t& expression) {
    machine_m.evaluate(expression);

    any_regular_t result = std::move(machine_m.back());
    machine_m.pop_back();

    return result;
}

/**************************************************************************************************/

void sheet_t::implementation_t::set(name_t n, const any_regular_t& v) {
#ifndef NDEBUG
    assert(!check_update_reentrancy_m &&
           "sheet_t::set() cannot be called during call to sheet_t::update().");
    updated_m = false;
#endif

    index_t::iterator iter(input_index_m.find(n));
    if (iter == input_index_m.end()) {
        throw std::logic_error(make_string("input cell ", n.c_str(), " does not exist."));
    }

    ++priority_high_m;
    iter->state_m = v;
    iter->priority_m = priority_high_m;

    // Leave contributing untouched.

    if (iter->specifier_m == access_input)
        init_dirty_m.set(iter->cell_set_pos_m);
}

/**************************************************************************************************/

void sheet_t::implementation_t::touch(const name_t* first, const name_t* last) {
    // REVISIT (sparent) : This should be constrained to interface cells only.
    // REVISIT (sparent) : This logic is similar to the logic in flow and should be the same.

    // build an index of the cells to touch sorted by current priority.

    typedef table_index<priority_t, cell_t> priority_index_t;

    priority_index_t index(&cell_t::priority_m);

    // REVISIT (sparent) : This loop is transform but the soft condition in the middle breaks that.
    // If we resolve the REVISIT() inside the loop so failure on find is a throw then this loop
    // is transform.

    while (first != last) {
        index_t::iterator iter(input_index_m.find(*first));

        /*
            REVISIT (sparent) : Cells that aren't present are ignored because sheets get "scoped"
            so if a cell isn't touched in a local sheet it might be in a more global scope.
            Perhaps the client should be keeping two lists and this would go back to an error.
        */

        if (iter != input_index_m.end())
            index.push_back(*iter);

        ++first;
    }

    index.sort();

    // Touch the cells - keeping their relative priority

    for (priority_index_t::iterator f(index.begin()), l(index.end()); f != l; ++f) {
        ++priority_high_m;
        f->priority_m = priority_high_m;
    }
}

/**************************************************************************************************/

void sheet_t::implementation_t::add_input(name_t name, const line_position_t& position,
                                          const array_t& initializer) {
    scope_value_t<bool> scope(initialize_mode_m, true);

    any_regular_t initial_value;

    if (initializer.size())
        initial_value = calculate_expression(position, initializer);

    cell_set_m.push_back(cell_t(name, std::move(initial_value), cell_set_m.size()));
    // REVISIT (sparent) : Non-transactional on failure.
    input_index_m.insert(cell_set_m.back());

    if (!name_index_m.insert(cell_set_m.back()).second) {
        throw stream_error_t(make_string("cell named '", name.c_str(), "'already exists."),
                             position);
    }
}


/**************************************************************************************************/
void sheet_t::implementation_t::add_output(name_t name, const line_position_t& position,
                                           const array_t& expression) {
    // REVISIT (sparent) : Non-transactional on failure.
    cell_set_m.push_back(cell_t(access_output, name,
                                boost::bind(&implementation_t::calculate_expression,
                                            boost::ref(*this), position, expression),
                                cell_set_m.size(), nullptr));

    output_index_m.insert(cell_set_m.back());

    if (!name_index_m.insert(cell_set_m.back()).second) {
        throw stream_error_t(make_string("cell named '", name.c_str(), "'already exists."),
                             position);
    }

    has_output_m = true;
}


/**************************************************************************************************/
// REVISIT (sparent) : Hacked glom of input/output pair.

void sheet_t::implementation_t::add_interface(name_t name, bool linked,
                                              const line_position_t& position1,
                                              const array_t& initializer_expression,
                                              const line_position_t& position2,
                                              const array_t& expression) {
    scope_value_t<bool> scope(initialize_mode_m, true);

    if (initializer_expression.size()) {
        cell_set_m.push_back(
            cell_t(name, linked,
                   boost::bind(&implementation_t::calculate_expression, boost::ref(*this),
                               position1, initializer_expression),
                   cell_set_m.size()));
    } else {
        cell_set_m.push_back(cell_t(name, linked, cell_t::calculator_t(), cell_set_m.size()));
    }

    // REVISIT (sparent) : Non-transactional on failure.
    input_index_m.insert(cell_set_m.back());

    if (initializer_expression.size())
        initialize_one(cell_set_m.back());

    if (expression.size()) {
        // REVISIT (sparent) : Non-transactional on failure.
        cell_set_m.push_back(cell_t(access_interface_output, name,
                                    boost::bind(&implementation_t::calculate_expression,
                                                boost::ref(*this), position2, expression),
                                    cell_set_m.size(), &cell_set_m.back()));
    } else {
        cell_set_m.push_back(cell_t(access_interface_output, name,
                                    boost::bind(&implementation_t::get, boost::ref(*this), name),
                                    cell_set_m.size(), &cell_set_m.back()));
    }
    output_index_m.insert(cell_set_m.back());

    if (!name_index_m.insert(cell_set_m.back()).second) {
        throw stream_error_t(make_string("cell named '", name.c_str(), "'already exists."),
                             position2);
    }
}

/**************************************************************************************************/

void sheet_t::implementation_t::add_interface(name_t name, any_regular_t initial) {
    cell_set_m.push_back(cell_t(name, true, cell_t::calculator_t(), cell_set_m.size()));

    cell_t& cell = cell_set_m.back();

    input_index_m.insert(cell);

    cell.state_m = std::move(initial);
    cell.priority_m = ++priority_high_m;

    cell_set_m.push_back(cell_t(access_interface_output, name,
                                boost::bind(&implementation_t::get, boost::ref(*this), name),
                                cell_set_m.size(), &cell));

    output_index_m.insert(cell_set_m.back());

    if (!name_index_m.insert(cell_set_m.back()).second) {
        throw std::logic_error(make_string("cell named '", name.c_str(), "'already exists."));
    }
}

/**************************************************************************************************/

void sheet_t::implementation_t::add_constant(name_t name, const line_position_t& position,
                                             const array_t& initializer) {
    scope_value_t<bool> scope(initialize_mode_m, true);

    cell_set_m.push_back(cell_t(access_constant, name, calculate_expression(position, initializer),
                                cell_set_m.size()));
    // REVISIT (sparent) : Non-transactional on failure.

    if (!name_index_m.insert(cell_set_m.back()).second) {
        throw stream_error_t(make_string("cell named '", name.c_str(), "'already exists."),
                             position);
    }
}

/**************************************************************************************************/

void sheet_t::implementation_t::add_constant(name_t name, any_regular_t value) {
    cell_set_m.push_back(cell_t(access_constant, name, std::move(value), cell_set_m.size()));

    if (!name_index_m.insert(cell_set_m.back()).second) {
        throw std::logic_error(make_string("cell named '", name.c_str(), "'already exists."));
    }
}

/**************************************************************************************************/

void sheet_t::implementation_t::add_logic(name_t logic, const line_position_t& position,
                                          const array_t& expression) {
    cell_set_m.push_back(cell_t(access_logic, logic,
                                boost::bind(&implementation_t::calculate_expression,
                                            boost::ref(*this), position, expression),
                                cell_set_m.size(), nullptr));

    if (!name_index_m.insert(cell_set_m.back()).second) {
        throw stream_error_t(make_string("cell named '", logic.c_str(), "'already exists."),
                             position);
    }
}

/**************************************************************************************************/

void sheet_t::implementation_t::add_invariant(name_t name, const line_position_t& position,
                                              const array_t& expression) {
    // REVISIT (sparent) : Non-transactional on failure.
    cell_set_m.push_back(cell_t(access_invariant, name,
                                boost::bind(&implementation_t::calculate_expression,
                                            boost::ref(*this), position, expression),
                                cell_set_m.size(), nullptr));

    output_index_m.insert(cell_set_m.back());

    if (!name_index_m.insert(cell_set_m.back()).second) {
        throw stream_error_t(make_string("cell named '", name.c_str(), "'already exists."),
                             position);
    }

    invariant_index_m.push_back(&cell_set_m.back());
}

/**************************************************************************************************/

void sheet_t::implementation_t::add_relation(const line_position_t& position,
                                             const array_t& conditional, const relation_t* first,
                                             const relation_t* last) {
    relation_cell_set_m.push_back(relation_cell_t(position, conditional, first, last));
    relation_cell_t& relation = relation_cell_set_m.back();

    // build a unique list of lhs cells

    vector<name_t> cell_set;

    for (; first != last; ++first) {
        cell_set.insert(cell_set.end(), first->name_set_m.begin(), first->name_set_m.end());
    }

    sort(cell_set);
    cell_set.erase(unique(cell_set), cell_set.end());

    for (vector<name_t>::iterator f = cell_set.begin(), l = cell_set.end(); f != l; ++f) {
        index_t::iterator p = output_index_m.find(*f);

        if (p == output_index_m.end() || !p->interface_input_m)
            throw stream_error_t(make_string("interface cell ", f->c_str(), " does not exist."),
                                 position);

        relation.edges_m.push_back(&(*p));
        p->relation_index_m.push_back(&relation);
        ++p->initial_relation_count_m;
    }
}

/**************************************************************************************************/

any_regular_t sheet_t::implementation_t::calculate_expression(const line_position_t& position,
                                                              const array_t& expression) {
    evaluate(machine_m, position, expression);

    any_regular_t result = std::move(machine_m.back());
    machine_m.pop_back();

    return result;
}

/**************************************************************************************************/

sheet_t::connection_t sheet_t::implementation_t::monitor_enabled(name_t n, const name_t* first,
                                                                 const name_t* last,
                                                                 const monitor_enabled_t& monitor) {
    assert(updated_m && "Must call sheet_t::update() prior to monitor_enabled.");
    index_t::iterator iter(input_index_m.find(n));

    if (iter == input_index_m.end())
        throw std::logic_error(make_string("Attempt to monitor nonexistent cell: ", n.c_str()));

    cell_bits_t touch_set;
    while (first != last) {
        index_t::iterator i(input_index_m.find(*first));
        if (i == input_index_m.end())
            throw std::logic_error(
                make_string("Attempt to monitor nonexistent cell: ", first->c_str()));
        touch_set.set(i->cell_set_pos_m);
        ++first;
    }

    /*
        REVISIT <sean_parent@mac.com> : This is a complex test that is duplicated in enabled_filter
        and should be distilled down to a simply the test of active_m.
    */

    monitor(active_m.test(iter->cell_set_pos_m) || (value_accessed_m.test(iter->cell_set_pos_m) &&
                                                    (touch_set & priority_accessed_m).any()));

    return monitor_enabled_m.connect(boost::bind(&sheet_t::implementation_t::enabled_filter, this,
                                                 touch_set, iter->cell_set_pos_m, monitor, _1, _2));
}

/**************************************************************************************************/
sheet_t::connection_t
sheet_t::implementation_t::monitor_invariant_dependent(name_t n,
                                                       const monitor_invariant_t& monitor) {
    assert(updated_m && "Must call sheet_t::update() prior to monitor_invariant_dependent.");

    index_t::iterator iter(output_index_m.find(n));

    if (iter == output_index_m.end())
        throw std::logic_error(make_string("Attempt to monitor nonexistent cell: ", n.c_str()));

    monitor(iter->invariant_m);

    return iter->monitor_invariant_m.connect(monitor);
}

/**************************************************************************************************/

sheet_t::connection_t sheet_t::implementation_t::monitor_value(name_t name,
                                                               const monitor_value_t& monitor) {
    assert(updated_m && "Must call sheet_t::update() prior to monitor_value.");

    index_t::iterator iter = output_index_m.find(name);

    if (iter == output_index_m.end()) {
        throw std::logic_error(make_string("Attempt to monitor nonexistent cell: ", name.c_str()));
    }

    monitor(iter->state_m);

    return iter->monitor_value_m.connect(monitor);
}

/**************************************************************************************************/

sheet_t::connection_t
sheet_t::implementation_t::monitor_contributing(name_t n, const dictionary_t& mark,
                                                const monitor_contributing_t& monitor) {
    assert(updated_m && "Must call sheet_t::update() prior to monitor_contributing.");

    index_t::iterator iter(output_index_m.find(n));

    if (iter == output_index_m.end()) {
        throw std::logic_error(make_string("Attempt to monitor nonexistent cell: ", n.c_str()));
    }

    monitor(contributing_set(mark, iter->contributing_m));

    return iter->monitor_contributing_m.connect(
        boost::bind(monitor, boost::bind(&sheet_t::implementation_t::contributing_set,
                                         boost::ref(*this), mark, _1)));
}

/**************************************************************************************************/

inline bool sheet_t::implementation_t::has_input(name_t name) const {
    return input_index_m.find(name) != input_index_m.end();
}

/**************************************************************************************************/

inline bool sheet_t::implementation_t::has_output(name_t name) const {
    return output_index_m.find(name) != output_index_m.end();
}

/**************************************************************************************************/

priority_t sheet_t::implementation_t::name_to_priority(name_t name) const {
    index_t::const_iterator i = input_index_m.find(name);
    assert(i != input_index_m.end() && i->specifier_m == access_interface_input &&
           "interface cell not found, should not be possible - preflight in add_interface.");
    return i->priority_m;
}

/**************************************************************************************************/

void sheet_t::implementation_t::flow(cell_bits_t& priority_accessed) {
    // Generate the set of cells connected to unresolved relations
    vector<cell_t*> cells;
    for (relation_cell_set_t::iterator f(relation_cell_set_m.begin()), l(relation_cell_set_m.end());
         f != l; ++f) {
        if (!f->resolved_m)
            cells.insert(cells.end(), f->edges_m.begin(), f->edges_m.end());
    }
    sort(cells);
    cells.erase(unique(cells), cells.end());

    // sort the cells by priority
    sort(cells, less(), &cell_t::priority);

    // mark that the priority of these cells was accessed for enablement
    // REVISIT <seanparent@google.com> : This is an approximation for enablement that could do
    // better with connected components

    for (vector<cell_t*>::iterator f = cells.begin(), l = cells.end(); f != l; ++f) {
        priority_accessed.set((*f)->interface_input_m->cell_set_pos_m);
    }

    /*
    pop the top cell from the stack
    if the cell is not resolved then resolve as a contributor
    find which relations the cell contributes to if any-
        if there is only one unresolved cell on the relation then
            resolve that cell as derived
                (need to have the cell refer to the deriving term in the relation?)
            push the cell to the top of stack
    loop until stack is empty.
    */

    while (!cells.empty()) {
        cell_t& cell = *cells.back();
        cells.pop_back();

        if (cell.relation_count_m == 0)
            continue;

        cell.resolved_m = true;

        for (relation_index_t::iterator f = cell.relation_index_m.begin(),
                                        l = cell.relation_index_m.end();
             f != l; ++f) {

            if ((*f)->resolved_m)
                continue;

            --cell.relation_count_m;

            const relation_t* term = 0;
            bool at_least_one = false;

            for (relation_set_t::iterator tf((*f)->terms_m.begin()), tl((*f)->terms_m.end());
                 tf != tl; ++tf) {
                // each term has a set of cells. If any cell is resolved then the term is resolved.

                bool resolved = false;
                for (vector<name_t>::iterator fc = tf->name_set_m.begin(),
                                              lc = tf->name_set_m.end();
                     fc != lc; ++fc) {

                    index_t::iterator iter = output_index_m.find(*fc);
                    assert(iter != output_index_m.end());

                    if (iter->resolved_m) {
                        resolved = true;
                        break;
                    }
                }
                if (resolved)
                    continue;

                if (!term) {
                    term = &(*tf);
                    at_least_one = true;
                } else {
                    term = NULL;
                    break;
                }
            }

            // REVISIT (sparent) : Better error reporting here.
            if (!at_least_one) {
                throw std::logic_error("all terms of relation resolve but relation not applied.");
            }

            if (!term)
                continue;

            // Flow out to lhs cells

            (*f)->resolved_m = true;

            for (std::size_t n = 0, count = term->name_set_m.size(); n != count; ++n) {
                index_t::iterator iter = output_index_m.find(term->name_set_m[n]);
                assert(iter != output_index_m.end());

                cell_t& cell = *iter;

                // REVISIT (sparent) : Better error reporting here.
                if (cell.term_m)
                    throw logic_error("over constrained.");

                if (count == 1) {
                    cell.term_m =
                        boost::bind(&implementation_t::calculate_expression, boost::ref(*this),
                                    term->position_m, term->expression_m);
                } else {
                    cell.term_m =
                        boost::bind(&implementation_t::calculate_indexed, boost::ref(*this),
                                    term->position_m, term->expression_m, n);
                }

                --cell.relation_count_m;

                // This will be a derived cell and will have a priority lower than any cell
                // contributing to it

                assert(cell.interface_input_m && "Missing input half of interface cell.");
                if (cell.interface_input_m->linked_m) {
                    cell.interface_input_m->priority_m = --priority_low_m;
                }

                if (cell.relation_count_m)
                    cells.push_back(&cell);
                else
                    cell.resolved_m = true;
            }

            // Remove the relation from any cells to which it is still attached. That is,
            // any cell which is still attached is an "in edge".

            vector<name_t> remaining_cells;

            for (relation_set_t::iterator tf((*f)->terms_m.begin()), tl((*f)->terms_m.end());
                 tf != tl; ++tf) {

                if (&(*tf) == term)
                    continue;

                remaining_cells.insert(remaining_cells.end(), tf->name_set_m.begin(),
                                       tf->name_set_m.end());
            }

            sort(remaining_cells);
            remaining_cells.erase(unique(remaining_cells), remaining_cells.end());

            for (vector<name_t>::iterator fc = remaining_cells.begin(), lc = remaining_cells.end();
                 fc != lc; ++fc) {

                index_t::iterator iter = output_index_m.find(*fc);
                assert(iter != output_index_m.end());

                if (iter->resolved_m)
                    continue;
                --iter->relation_count_m;
            }
        }
        assert(cell.relation_count_m == 0 &&
               "Cell still belongs to relation but all relations resolved.");
    }
}

/**************************************************************************************************/

void sheet_t::implementation_t::update() {
#ifndef NDEBUG
    check_reentrancy checker(check_update_reentrancy_m);
#endif

    conditional_indirect_contributing_m.reset();

    value_accessed_m.reset();

    for_each(cell_set_m, &cell_t::clear_dirty);
    for_each(relation_cell_set_m, &relation_cell_t::clear_resolved);

    // Solve the conditionals.

    accumulate_contributing_m.reset();

    for (relation_cell_set_t::iterator current_cell(relation_cell_set_m.begin()),
         last_cell(relation_cell_set_m.end());
         current_cell != last_cell; ++current_cell) {
        if (current_cell->conditional_m.empty())
            continue;

        if (!calculate_expression(current_cell->position_m, current_cell->conditional_m)
                 .cast<bool>()) {
            for (vector<cell_t*>::iterator f = current_cell->edges_m.begin(),
                                           l = current_cell->edges_m.end();
                 f != l; ++f) {
                --(*f)->relation_count_m;
            }
            current_cell->resolved_m = true;
        }
    }

    conditional_indirect_contributing_m = accumulate_contributing_m;

    cell_bits_t priority_accessed;

    flow(priority_accessed);

#ifndef NDEBUG
    for (relation_cell_set_t::iterator first(relation_cell_set_m.begin()),
         last(relation_cell_set_m.end());
         first != last; ++first) {
        if (first->resolved_m)
            continue;


        std::clog << "(warning) relation unnecessary and ignored\n" << first->position_m;
    }
#endif

    // calculate the output/interface_output/invariant cells and apply.

    for (index_t::const_iterator iter(output_index_m.begin()), last(output_index_m.end());
         iter != last; ++iter) {
        cell_t& cell(*iter);

        // REVISIT (sparent) : This is a copy/paste of get();

        if (!cell.evaluated_m) {
            accumulate_contributing_m.reset();

            if (cell.specifier_m == access_interface_output)
                get_stack_m.push_back(std::make_pair(cell.name_m, false));

            cell.calculate();

            if (cell.specifier_m == access_interface_output)
                get_stack_m.pop_back();

            cell.contributing_m = accumulate_contributing_m;

            cell.contributing_m |= conditional_indirect_contributing_m;
        }

        /*
            REVISIT (sparent) : This would be slightly more efficient if I moved the link flag
            to the output side.
        */

        // Apply the interface output to interface inputs of linked cells.
        if (cell.interface_input_m && cell.interface_input_m->linked_m) {
            cell.interface_input_m->state_m = cell.state_m;
        }
    }

    // Then we can check the invariants -

    cell_bits_t poison;

    for (index_vector_t::const_iterator iter(invariant_index_m.begin()),
         last(invariant_index_m.end());
         iter != last; ++iter) {
        cell_t& cell(**iter);

        if (!cell.state_m.cast<bool>())
            poison |= cell.contributing_m;
    }


    /*
        REVISIT (sparent) : Shoule we report conditional_indirect_contributing with the invariants?
    */

    /*
        REVISIT (sparent): Monitoring a value should return all of -
            value
            contributing
            invariant_dependent

        Otherwise the client risks getting out of sync.
    */

    /*
        REVISIT (sparent) : enabling everything with priority_accessed is to granular. Need
        connected components.
    */

    cell_bits_t active = priority_accessed;

    // REVIST (sparent) : input monitor should recieve priority_accessed and poison bits.

    for (index_t::const_iterator iter(output_index_m.begin()), last(output_index_m.end());
         iter != last; ++iter) {
        cell_t& cell(*iter);
        bool invariant((poison & cell.contributing_m).none());

        if (invariant != cell.invariant_m)
            cell.monitor_invariant_m(invariant);
        cell.invariant_m = invariant;

        if (cell.dirty_m)
            cell.monitor_value_m(cell.state_m);

        /*
            REVISIT (sparent) : Is there any way to prune this down a
            bit? Calculating the contributing each time is expensive.
        */

        if (!cell.monitor_contributing_m.empty()) {
            // REVISIT (sparent) : no need to notify if contributing didn't change...
            cell.monitor_contributing_m(cell.contributing_m);
        }

        if ((cell.specifier_m == access_output) ||
            (!has_output_m && cell.specifier_m == access_interface_output)) {
            active |= cell.contributing_m;
        }
    }

    // update
    monitor_enabled_m(priority_accessed, active);
    priority_accessed_m = priority_accessed;
    active_m = active;

#ifndef NDEBUG
    updated_m = true;
#endif
}

/**************************************************************************************************/

void sheet_t::implementation_t::initialize_one(cell_t& cell) {
    /*
        REVISIT (sparent) : Should have more checking here - detecting cycles (and forward
        references?)
    */

    accumulate_contributing_m.reset();
    cell.state_m = cell.calculator_m();
    cell.priority_m = ++priority_high_m;
    cell.init_contributing_m |= accumulate_contributing_m;
}

/**************************************************************************************************/

void sheet_t::implementation_t::reinitialize() {
    scope_value_t<bool> scope(initialize_mode_m, true);

    for (index_t::iterator f = output_index_m.begin(), l = output_index_m.end(); f != l; ++f) {

        if (!f->interface_input_m)
            continue;

        cell_t& cell = *f->interface_input_m;

        if ((init_dirty_m & cell.init_contributing_m).none())
            continue;

        initialize_one(cell);
    }

    init_dirty_m.reset();
}

/**************************************************************************************************/

dictionary_t sheet_t::implementation_t::contributing(const dictionary_t& mark) const {
    cell_bits_t contributing;

    for (index_t::const_iterator iter(output_index_m.begin()), last(output_index_m.end());
         iter != last; ++iter) {
        if ((iter->specifier_m == access_output) ||
            (!has_output_m && iter->specifier_m == access_interface_output)) {
            contributing |= iter->contributing_m;
        }
    }

    return contributing_set(mark, contributing);
}

/**************************************************************************************************/

dictionary_t sheet_t::implementation_t::contributing_to_cell(name_t x) const {
    index_t::iterator iter = output_index_m.find(x);

    if (iter == output_index_m.end())
        throw std::logic_error(make_string("No monitorable cell: ", x.c_str()));

    return contributing_set(dictionary_t(), iter->contributing_m);
}

/**************************************************************************************************/
/*
    NOTE (sparent) : A mark containes a dictionary of contributing values.

    If a contributing value has changed or been added since the mark
    then it will be reported in the dictionary result.

    If a value is contributing, and the set of values which are
    contributing has changed, then the value will be reported as
    having been touched.
*/

dictionary_t sheet_t::implementation_t::contributing_set(const dictionary_t& mark,
                                                         const cell_bits_t& contributing) const {
    dictionary_t changed;
    dictionary_t touched;
    bool include_touched(false);

    for (std::size_t index(0), last(cell_set_m.size()); index != last; ++index) {
        if (contributing[index]) {
            const cell_t& cell = cell_set_m[index];
            const name_t& name(cell.name_m);
            const any_regular_t& value(cell.state_m);
            bool priority_accessed(priority_accessed_m.test(cell.cell_set_pos_m));

            if (!mark.count(name)) {
                include_touched = true;
                changed.insert(make_pair(name, value));
            } else if (get_value(mark, name) != value)
                changed.insert(make_pair(name, value));
            else if (priority_accessed)
                touched.insert(make_pair(name, value));
        }
    }

    if (include_touched) {
        changed.insert(touched.begin(), touched.end());
    }
    return changed;
}

/**************************************************************************************************/

void sheet_t::implementation_t::set(const dictionary_t& dict) {
    for (dictionary_t::const_iterator iter(dict.begin()), last(dict.end()); iter != last; ++iter) {
        set(iter->first, iter->second);
    }
}


/**************************************************************************************************/

const any_regular_t& sheet_t::implementation_t::operator[](name_t variable_name) const {
    assert(updated_m && "Must call sheet_t::update() prior to operator[].");

    index_t::iterator iter = name_index_m.find(variable_name);

    if (iter == name_index_m.end()) {
        throw std::logic_error(make_string("variable ", variable_name.c_str(), " not found."));
    }

    assert(iter->evaluated_m && "Cell was not evaluated!");

    return iter->state_m;
}

/**************************************************************************************************/

any_regular_t sheet_t::implementation_t::get(name_t variable_name) {
#if 0
    // REVISIT (sparent) : I can't currently turn this assert on because of inspect.
    // However, it would be good to seperate out this get from the operator[] and
    // only use this one for updates. The problem is that inspect shares the same VM
    // so enabling this assert has to wait until I seperate the VM from the sheet_t.
    
    assert(check_update_reentrancy_m
           && "sheet_t::get() can only be called from sheet_t::update().");
#endif

    if (initialize_mode_m) {
        index_t::iterator iter(input_index_m.find(variable_name));

        if (iter == input_index_m.end()) {
            iter = name_index_m.find(variable_name);
            if (iter == name_index_m.end() || iter->specifier_m != access_constant) {
                throw std::logic_error(
                    make_string("variable ", variable_name.c_str(), " not found."));
            }
        }

        cell_t& cell = *iter;

        accumulate_contributing_m |= cell.init_contributing_m;
        return cell.state_m;
    }

    scope_count scope(get_count_m);

    /*
        REVISIT (sparent) - If we go to three pass on interface cells then the max count is
        number of cells plus number of interface cells.
    */

    if (get_count_m > cell_set_m.size()) {
        throw std::logic_error(std::string("cycle detected, consider using a relate { } clause."));
    }

    index_t::iterator iter = name_index_m.find(variable_name);

    if (iter == name_index_m.end()) {
        throw std::logic_error(make_string("variable ", variable_name.c_str(), " not found."));
    }

    cell_t& cell = *iter;

    // If the variable is on the top of the stack then we assume it
    // must refer to an input cell.

    if (get_stack_m.size() && (get_stack_m.back().first == variable_name)) {
        assert(cell.interface_input_m &&
               "FATAL (sparent) : Only interface cells should be on the get stack.");

        if (!get_stack_m.back().second && !cell.term_m.empty()) {
            get_stack_m.back().second = true;
            return cell.term_m();
        } else {
            value_accessed_m.set(cell.interface_input_m->cell_set_pos_m);
            accumulate_contributing_m |= cell.interface_input_m->contributing_m;
            return cell.interface_input_m->state_m;
        }
    }

    if (cell.specifier_m == access_interface_output)
        get_stack_m.push_back(std::make_pair(variable_name, false));

    // REVISIT (sparent) : paired call should be ctor/dtor

    try {
        // REVISIT (sparent) : First pass getting the logic correct.

        if (cell.evaluated_m)
            accumulate_contributing_m |= cell.contributing_m;
        else {
            cell_bits_t old = accumulate_contributing_m;
            accumulate_contributing_m.reset();

            cell.calculate();

            cell.contributing_m = accumulate_contributing_m;
            accumulate_contributing_m |= old;
        }

        if (cell.specifier_m != access_input && cell.specifier_m != access_constant)
            cell.contributing_m |= conditional_indirect_contributing_m;
    } catch (...) {
        if (cell.specifier_m == access_interface_output)
            get_stack_m.pop_back();
        throw;
    }

    if (cell.specifier_m == access_interface_output)
        get_stack_m.pop_back();

    return cell.state_m;
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
