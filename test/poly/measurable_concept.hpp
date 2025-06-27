/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)
*/

/**************************************************************************************************/

#ifndef MEASURABLE_HPP
#define MEASURABLE_HPP

#include <adobe/config.hpp>

// define default implementation of amount operation to call member function amount
template <typename T>
double adobe_test_size(const T& t) {
    return static_cast<double>(t.size());
}

namespace adobe_test {


template <class T>
struct MeasurableConcept {
    MeasurableConcept() {} // for gcc

    // operations
    static double size(const T& t) { return adobe_test_size(t); }

    // constraints
    void constraints() { d = size(t); };

    const T t;
    double d;
};

} // namespace adobe_test


#endif
