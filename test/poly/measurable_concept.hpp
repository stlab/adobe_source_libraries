/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef MEASURABLE_HPP
#define MEASURABLE_HPP

#include <adobe/config.hpp>

// define default implementation of size operation to call member function size
template <typename T> double size(const T& t) { return static_cast<double>(t.size()); }

template <class T>
struct MeasurableConcept {
    MeasurableConcept() {}  // for gcc
    
// operations
    static double size(const T& t) { using ::size; return size(t); }
    
// constraints
    void constraints() { d = size(t); };

    const T t;
    double d;
};



#endif
