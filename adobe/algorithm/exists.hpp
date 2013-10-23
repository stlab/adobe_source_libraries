/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_EXISTS_HPP
#define ADOBE_ALGORITHM_EXISTS_HPP

#include <adobe/config.hpp>

#include <algorithm>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup exists exists
\ingroup container_algorithm
*/
/*************************************************************************************************/
/*!
    \ingroup exists

    \todo (sparent) : If we developed the idea of an associative range then we could generalize this
    to sorted vectors. This would then consume <code>std::binary_search()</code>.
*/

template <typename C> // C models AssociativeContainer
bool exists(const C& c, const typename C::key_type& k)
{ return c.find(k) != c.end(); }

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
