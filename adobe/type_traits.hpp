/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)
*/

/**************************************************************************************************/


#ifndef ADOBE_TYPE_TRAITS_HPP
#define ADOBE_TYPE_TRAITS_HPP

#include <adobe/config.hpp>

#include <type_traits>

/**************************************************************************************************/

namespace adobe {

#if ASL_CPP_VERSION_LESS_THAN(17)

template <class F, class... Args>
using invoke_result_t = std::result_of_t<F(Args...)>;

#else

template <class F, class... Args>
using invoke_result_t = std::invoke_result_t<F, Args...>;

#endif

} // namespace adobe

/**************************************************************************************************/

#endif // include guard
