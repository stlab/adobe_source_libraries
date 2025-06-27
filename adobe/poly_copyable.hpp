/*
    Copyright 2006-2007 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)

    Author(s): Mat Marcus
*/

/**************************************************************************************************/

#ifndef ADOBE_POLY_COPYABLE_HPP
#define ADOBE_POLY_COPYABLE_HPP

#include <adobe/config.hpp>

#include <boost/concept_check.hpp>

#include <adobe/poly.hpp>

/**************************************************************************************************/

namespace adobe {

/*!
  \ingroup poly_related
*/

/**************************************************************************************************/

/*!
  \ingroup poly_related
  \brief Implementation of a trivial poly interface in terms of types modeling Copyable .
  Used in poly<copyable> implementation.
  \sa copyable, poly_copyable_interface, optimized_storage_type
*/

template <typename T>
struct poly_copyable_instance : optimized_storage_type<T, poly_copyable_interface>::type {
    typedef typename optimized_storage_type<T, poly_copyable_interface>::type base_t;

    /*!
        Check that T models appropriate concept for C++ 2003
    */
    BOOST_CLASS_REQUIRE(T, boost, CopyConstructibleConcept);

    /*!
        Construct from concrete copyable
    */
    poly_copyable_instance(const T& x) : base_t(x) {}

    /*!
        Move constructor
    */
    poly_copyable_instance(poly_copyable_instance&& x) noexcept : base_t(std::move(x)) {}
};

/**************************************************************************************************/

/*!
  \ingroup poly_related
  \brief "Handle" class used in poly<copyable> implementation.
  \sa poly_copyable_instance, poly_copyable_interface, poly_copyable_interface, poly_base
*/

struct copyable : poly_base<poly_copyable_interface, poly_copyable_instance> {
    typedef poly_base<poly_copyable_interface, poly_copyable_instance> base_t;

    /*!
        Construct from concrete copyable
    */
    template <typename T>
    explicit copyable(const T& s) : base_t(s) {}

    /*!
        Move constructor
    */
    copyable(copyable&& x) noexcept : base_t(std::move(x)) {}
};


/**************************************************************************************************/

#if !defined(ADOBE_NO_DOCUMENTATION)

/**************************************************************************************************/

typedef poly<copyable> poly_copyable_t;

/**************************************************************************************************/

#endif

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif
