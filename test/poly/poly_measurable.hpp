/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)
*/

/**************************************************************************************************/

#ifndef ANY_MEASURABLE_HPP
#define ANY_MEASURABLE_HPP

#include <adobe/config.hpp>

#include <boost/concept_check.hpp>

#include <adobe/poly.hpp>

#include "measurable_concept.hpp"

namespace adobe_test {

/////////////////////

// This is the "abstract" measurable interface, modeling the MeasurableConcept

struct poly_measurable_interface : adobe::poly_copyable_interface {
    virtual double size() const = 0;

    virtual ~poly_measurable_interface() {}
};

/**************************************************************************************************/
// This class template implements the abstract measurable interface in terms of a type T that
// Models the MeasurableConcept

template <typename T>
struct poly_measurable_instance
    : adobe::optimized_storage_type<T, poly_measurable_interface>::type {
    typedef typename adobe::optimized_storage_type<T, poly_measurable_interface>::type base_t;

    BOOST_CLASS_REQUIRE(T, adobe_test, MeasurableConcept);

    poly_measurable_instance(const T& x) : base_t(x) {}

    poly_measurable_instance(poly_measurable_instance&& x) noexcept : base_t(std::move(x)) {}

    double size() const { return MeasurableConcept<T>::size(this->get()); }
};

/**************************************************************************************************/
// Another boilerplate class, measurable serves as a template parameter to the poly<> machinery
// tying together the interface and implementation above.

struct measurable : adobe::poly_base<poly_measurable_interface, poly_measurable_instance> {
    typedef adobe::poly_base<poly_measurable_interface, poly_measurable_instance> base_t;

    // No delegating constructors (yet), so we call base constructor manually
    template <typename T>
    explicit measurable(const T& s) : base_t(s) {}

    measurable(measurable&& x) noexcept : base_t(std::move(x)) {}

    // No forwarding in C++, so we do it manually
    double size() const { return interface_ref().size(); }
};

typedef adobe::poly<measurable> poly_measurable;

} // namespace adobe_test

#endif
