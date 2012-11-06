/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ANY_MEASURABLE_2D_HPP
#define ANY_MEASURABLE_2D_HPP

#include <adobe/config.hpp>

#include <boost/concept_check.hpp>

#include <adobe/poly.hpp>

#include "measurable_2dconcept.hpp"

/////////////////////

// This is the "abstract" measurable_2d interface, modeling the Measurable_2dConcept

struct poly_measurable_2d_interface : poly_measurable_interface
{
    virtual double height() const = 0;
};

/*************************************************************************************************/
// This class template implements the abstract measurable_2d interface in terms of a type T that
// Models the Measurable_2dConcept

template <typename T>
struct poly_measurable_2d_instance 
    : adobe::optimized_storage_type<T, poly_measurable_2d_interface>::type
{
    typedef typename adobe::optimized_storage_type<T, poly_measurable_2d_interface>::type base_t;

    BOOST_CLASS_REQUIRE(T, , Measurable_2DConcept);

    poly_measurable_2d_instance(const T& x) 
        : base_t(x) {}

    poly_measurable_2d_instance(adobe::move_from<poly_measurable_2d_instance> x) 
        : base_t(adobe::move_from<base_t>(x.source)) {}

    double size() const
    { 
        return Measurable_2DConcept<T>::size(this->get());
    }

    double height() const
    { 
        return Measurable_2DConcept<T>::height(this->get());
    }
}; 

/*************************************************************************************************/
// Another boilerplate class, measurable_2d serves as a template parameter to the poly<> machinery
// tying together the interface and implementation above.

struct measurable_2d : adobe::poly_base<poly_measurable_2d_interface, poly_measurable_2d_instance>
{
    typedef adobe::poly_base<poly_measurable_2d_interface, poly_measurable_2d_instance> base_t;

    // No delegating constructors (yet), so we call base constructor manually
    template <typename T>
    explicit measurable_2d(const T& s)
        : base_t(s) {}

    measurable_2d(adobe::move_from<measurable_2d> x)
        : base_t(adobe::move_from<base_t>(x.source)) {}

    // No forwarding in C++, so we do it manually
    double size() const
    { return interface_ref().size(); }

    // No forwarding in C++, so we do it manually
    double height() const
    { return interface_ref().height(); }
};

typedef adobe::poly<measurable_2d> poly_measurable2d;

#endif
