/*
    Copyright 2006-2007 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)

    Author(s): Mat Marcus
*/

/**************************************************************************************************/

#ifndef ADOBE_POLY_REGULAR_HPP
#define ADOBE_POLY_REGULAR_HPP

#include <adobe/config.hpp>

#include <utility>

#include <boost/concept_check.hpp>

#include <adobe/empty.hpp>
#include <adobe/poly.hpp>
#include <adobe/regular_concept.hpp>

/**************************************************************************************************/

namespace adobe {

/*!
  \ingroup poly_related
*/

/**************************************************************************************************/
/*!
  \brief Virtual interface for RegularConcept. Used in poly<regular> implementation.
  \sa regular, poly_regular_instance, poly_copyable_interface, RegularConcept
*/

struct poly_regular_interface : poly_copyable_interface {
    virtual bool equals(const poly_regular_interface& new_value) const = 0;
};

/**************************************************************************************************/

/*!
  \ingroup poly_related
  \brief Implementation of a trivial poly interface in terms of types modeling regular .
  Used in poly<regular> implementation.
  \sa regular, poly_regular_interface, poly_copyable_interface, optimized_storage_type
*/

template <typename T>
struct poly_regular_instance : optimized_storage_type<T, poly_regular_interface>::type {
    typedef typename optimized_storage_type<T, poly_regular_interface>::type base_t;

    /*!
        Check that T models appropriate concept for C++ 2003
    */
    BOOST_CLASS_REQUIRE(T, adobe, RegularConcept);

    /*!
        Construct from concrete regular
    */
    poly_regular_instance(const T& x) : base_t(x) {}

    /*!
        Move constructor
    */
    poly_regular_instance(poly_regular_instance&& x) noexcept : base_t(std::move(x)) {}

    bool equals(const poly_regular_interface& x) const {
        return this->type_info() == x.type_info() &&
               this->get() == *static_cast<const T*>(x.cast());
    }
};

/**************************************************************************************************/

/*!
  \ingroup poly_related
  \brief "Handle" class used in poly<regular> implementation.
  \sa poly_regular_instance, poly_regular_interface, poly_copyable_interface, poly_base
*/

struct regular : poly_base<poly_regular_interface, poly_regular_instance> {
    typedef poly_base<poly_regular_interface, poly_regular_instance> base_t;

    /*!
        Construct from concrete regular
    */
    template <typename T>
    explicit regular(const T& s) : base_t(s) {}

    regular() : base_t(empty_t()) {}

    /*!
        Move constructor
    */
    regular(regular&& x) noexcept : base_t(std::move(x)) {}
};


/**************************************************************************************************/

#if !defined(ADOBE_NO_DOCUMENTATION)

/**************************************************************************************************/

typedef poly<regular> poly_regular_t;

/**************************************************************************************************/

#endif

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif
