/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_EXCEPTION_HPP
#define ADOBE_EXCEPTION_HPP

#include <string>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

/// invoke `std::terminate` with the given message.
[[noreturn]] void terminate(const char*);
[[noreturn]] void terminate(const std::string&);

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
