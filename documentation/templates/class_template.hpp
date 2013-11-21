/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

namespace adobe {

class my_class_t {
public:
    typedef int my_associated_type_t;

    my_class_t();

    adobe::my_return_t my_function(const adobe::my_parameter_t& a, const adobe::my_parameter_t& b);

    my_associated_type_t my_variable_m;
};
}
