/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef MEASURABLE_2DCONCEPT_HPP
#define MEASURABLE_2DCONCEPT_HPP

#include <adobe/config.hpp>

// define default implementation of size operation to call member function size
template <typename T> double height(const T& t) { return t.height(); }

#ifndef ADOBE_HAS_CPLUS0X_CONCEPTS

template <class T>
struct Measurable_2DConcept : MeasurableConcept<T>{
// operations
    static double height(const T& t) { using ::height; return height(t); }
    
// constraints
    void constraints() { d = height(t); d = size(t); }

    const T t;
    double d;
};

#else

#include <concepts>

auto concept Measurable_2DConcept<typename T> 
    : std::CopyConstructible<T>, std::EqualityComparable<T> 
{

// operations
    double height(T t);
};

#endif

#endif
