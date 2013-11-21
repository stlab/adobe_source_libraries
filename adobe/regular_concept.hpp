/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_REGULAR_HPP
#define ADOBE_REGULAR_HPP

/*************************************************************************************************/

#ifdef ADOBE_HAS_CPLUS0X_CONCEPTS

/*************************************************************************************************/

#include <concepts>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

auto concept RegularConcept<typename T> : std::CopyConstructible<T>,
                                          std::Assignable<T>,
                                          std::EqualityComparable<T>,
                                          std::Swappable<T>,
                                          std::DefaultConstructible<T> // not yet
                                          {};

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#else

/*************************************************************************************************/

#include <algorithm>

#include <boost/concept_check.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

template <class T>
struct RegularConcept {

    // Concept checking:

    void constraints() {
        // refinement of:
        boost::function_requires<boost::CopyConstructibleConcept<T>>();
        boost::function_requires<boost::AssignableConcept<T>>();
        boost::function_requires<boost::EqualityComparableConcept<T>>();
        //        boost::function_requires<boost::SwappableConcept<T> >();

        using std::swap;
        swap(t, t);
    }
#if !defined(ADOBE_NO_DOCUMENTATION)
    T t;
#endif
};

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/

#endif
