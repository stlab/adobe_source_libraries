/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/check_container.hpp>

#include <boost/concept_check.hpp>

#include <adobe/implementation/swap.hpp>
#include <adobe/test/check_regular.hpp>
#include <iostream>

namespace adobe {
/*!
\defgroup testing Misc utilities (Testing, Timer, ignore_unused,...)
\ingroup utility
@{
*/

template <typename T>
void check_container(const T& c) {
    adobe::check_traversable(c);

    // Containers add the requirement that each element visited once to traversable
    BOOST_CHECK_MESSAGE(c.size() == std::distance(c.begin(), c.end()), "container size");
    BOOST_CHECK_MESSAGE(d.size() == std::distance(d.begin(), d.end()), "container size");
}

#if 0
//gcc always instantiates this
BOOST_TEST_CASE_TEMPLATE_FUNCTION(check_containers, T)
{
    check_container(arbitrary_container_value<T>());
}
#endif

//!@}
} // namespace adobe
