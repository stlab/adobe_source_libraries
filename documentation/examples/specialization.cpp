/*
    Copyright 2005 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)
*/

/**************************************************************************************************/

/// start_recommended_specialization

#include <adobe/algorithm.hpp>
#include <adobe/memory.hpp>
#include <algorithm>

namespace my_namespace {

class my_iterator {
    // ...
};

class my_class {
    // ...
};

} // namespace my_namespace

// specialize find for my_iterator - specialize in the std namespace

template <>
my_namespace::my_iterator std::find(my_namespace::my_iterator, my_namespace::my_iterator) {
    // do special find
}

// specialize ptr_traits for my_class

template <>
struct adobe::ptr_traits<my_namespace::my_class> {
    typedef my_namespace::my_class element_type;
    typedef element_type* pointer_type;
    typedef const element_type* const_pointer_type template <class U>
    struct rebind {
        typedef adobe::ptr_traits<U> other;
    };
    enum { is_array = false };

    static void delete_ptr(pointer_type x) throw() { delete x; }
};


/// end_recommended_specialization
