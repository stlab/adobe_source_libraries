/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)
*/

/**************************************************************************************************/

#ifndef MEASURABLE_2DCONCEPT_HPP
#define MEASURABLE_2DCONCEPT_HPP

#include <adobe/config.hpp>

#include "measurable_concept.hpp"

namespace adobe_test {

// define default implementation of size operation to call member function size
template <typename T>
double height(const T& t) {
    return t.height();
}

#ifndef ADOBE_HAS_CPLUS0X_CONCEPTS

template <class T>
struct Measurable_2DConcept : MeasurableConcept<T> {
    // operations
    static double height(const T& t) {
        using ::adobe_test::height;
        return height(t);
    }

    // constraints
    void constraints() {
        d = height(t);
        d = adobe_test_size(t);
    }

    const T t;
    double d;
};

} // namespace adobe_test

#else

#include <concepts>

auto concept Measurable_2DConcept<typename T> : std::CopyConstructible<T>,
                                                std::EqualityComparable<T> {

    // operations
    double height(T t);
};

#endif

#endif
