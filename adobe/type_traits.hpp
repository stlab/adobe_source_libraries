/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/


#ifndef ADOBE_TYPE_TRAITS_HPP
#define ADOBE_TYPE_TRAITS_HPP


namespace adobe {

#if __cplusplus < 201703L

template <class F, class... Args>
using invoke_result_t = std::result_of_t<F(Args...)>;

#else

template <class F, class... Args>
using invoke_result_t = std::invoke_result_t<F, Args...>;

#endif

} // namespace adobe

#endif // include guard
