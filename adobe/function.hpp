/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_FUNCTION_HPP
#define ADOBE_FUNCTION_HPP

#include <functional>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

/// \deprecated Use std::function instead.
template <class Signature>
using function = std::function<Signature>;

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
