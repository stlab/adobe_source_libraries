/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_ALGORITHM_FOR_EACH_HPP
#define ADOBE_ALGORITHM_FOR_EACH_HPP

#include <adobe/config.hpp>
#include <functional>

#include <algorithm>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>


/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/
/*!
\defgroup for_each for_each
\ingroup non_mutating_algorithm

\note Unlike \c std::for_each(), \c adobe::for_each() does not return a copy of \c f because \c f
may not be a function object (it must model \ref concept_convertible_to_function). If it is
necessary to retrieve information from the function object, pass a \c std::reference_wrapper of
the function object using \c std::ref() instead.
*/
/**************************************************************************************************/
/*!
    \ingroup for_each

    \brief for_each implementation
*/
template <class InputIterator, class UnaryFunction>
inline void for_each(InputIterator first, InputIterator last, UnaryFunction f) {
    std::for_each(first, last, std::bind(f, std::placeholders::_1));
}

/*!
    \ingroup for_each

    \brief for_each implementation
*/
template <class InputRange, class UnaryFunction>
inline void for_each(InputRange& range, UnaryFunction f) {
    adobe::for_each(boost::begin(range), boost::end(range), f);
}

/*!
    \ingroup for_each

    \brief for_each implementation
*/
template <class InputRange, class UnaryFunction>
inline void for_each(const InputRange& range, UnaryFunction f) {
    adobe::for_each(boost::begin(range), boost::end(range), f);
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
