/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_ERASE_IF_HPP
#define ADOBE_ALGORITHM_ERASE_IF_HPP

#include <adobe/config.hpp>

#include <cassert>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/next_prior.hpp>

#include <adobe/algorithm/remove.hpp>
#include <adobe/algorithm/find.hpp>
#include <adobe/container/storage.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
\defgroup erase erase [review]
\ingroup container_algorithm

The current \c erase member function on containers does not have a uniform interface. The \c erase
algorithm here provides a normalized interface to erase on all containers.

The \c erase_if algorithm erases any value from the container (within the optionally supplied range)
which satisfies the predicate. The erase is handled efficiently for any container type.

\complexity
    Linear. Exactly <code>last - first</code> applications of \c pred.
*/
/*************************************************************************************************/

namespace implementation {


template <typename T> // T models Container
typename T::iterator erase(T& x, typename T::iterator f, typename T::iterator l, block_tag)
{ return x.erase(f, l); }

template <typename T> // T models Container
typename T::iterator erase(T& x, typename T::iterator f, typename T::iterator l, node_tag)
{ x.erase(f, l); return l; }

} // implementation

/*************************************************************************************************/

/*!
    \ingroup erase
*/
template <typename T> // T models Container
typename T::iterator erase(T& x, typename T::iterator f, typename T::iterator l)
{
    return implementation::erase(x, f, l, typename storage_category<T>::type());
}

/*************************************************************************************************/

/*!
    \ingroup erase
*/
template <  typename T, // T models Container
            typename R> // R models Range(iterator(T), iterator(T))
typename T::iterator erase(T& x, const R& r)
{
    return erase(x, boost::begin(r), boost::end(r));
}

/*!
    \ingroup erase
*/

template <typename T> // T models Container
typename T::iterator erase(T& x, typename T::iterator f)
{
    assert(f != end(x) && "FATAL (sparent) : Attempt to erase the end of a container.");
    return erase(x, f, boost::next(f));
}

/*************************************************************************************************/

namespace implementation {

template <  typename T, // T models Container
            typename P> // P models UnaryPredicate
void erase_if(T& x, typename T::iterator f, typename T::iterator l, P p, block_tag)
{ x.erase(adobe::remove_if(f, l, p), l); }

template <  typename T, // T models Container
            typename P> // P models UnaryPredicate
void erase_if(T& x, typename T::iterator f, typename T::iterator l, P p, node_tag)
{
    while (f != l) {
        f = adobe::erase(x, find_range_if(f, l, p));
    }
}

} // implementation

/*************************************************************************************************/

/*!
    \ingroup erase
*/

template <  typename T, // T models Container
            typename P> // P models UnaryPredicate
void erase_if(T& x, typename T::iterator f, typename T::iterator l, P p)
{
    implementation::erase_if(x, f, l, p, typename storage_category<T>::type());
}

/*!
    \ingroup erase
*/

template <  typename T, // T models Container
            typename P> // P models UnaryPredicate
void erase_if(T& x, P p)
{
    erase_if(x, boost::begin(x), boost::end(x), p);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
