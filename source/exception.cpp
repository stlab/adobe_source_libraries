/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/exception.hpp>

#include <exception>
#include <stdexcept>
#include <string>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

[[noreturn]] void terminate(const char* message) {
    try {
        throw std::logic_error(message);
    } catch (...) {
        std::terminate();
    }
}

[[noreturn]] void terminate(const std::string& message) {
    try {
        throw std::logic_error(message);
    } catch (...) {
        std::terminate();
    }
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
