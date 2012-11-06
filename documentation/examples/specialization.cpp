/*
    Copyright 2005 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

/// start_recommended_specialization

#include <algorithm>
#include <adobe/algorithm.hpp>
#include <adobe/memory.hpp>

namespace my_namespace {

class my_iterator
{
// ...
};

class my_class
{
// ...
};

} // my_namespace

// specialize find for my_iterator - specialize in the std namespace

template <>
my_namespace::my_iterator std::find(my_namespace::my_iterator, my_namespace::my_iterator)
{
    // do special find
}

// specialize ptr_traits for my_class

template <>
struct adobe::ptr_traits<my_namespace::my_class>
{
    typedef my_namespace::my_class  element_type;
    typedef element_type*           pointer_type;
    typedef const element_type*     const_pointer_type
    
    template <class U> struct rebind { typedef adobe::ptr_traits<U> other; };
    enum { is_array = false };
    
    static void delete_ptr(pointer_type x) throw() { delete x; }
};


/// end_recommended_specialization
