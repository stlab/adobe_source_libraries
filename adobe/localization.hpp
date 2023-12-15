/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_LOCALIZATION_HPP
#define ADOBE_LOCALIZATION_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <functional>
#include <string>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

/*!
\defgroup localization localization
\ingroup parsing
@{
*/

/*!
    The proc used here can expect utf-8 coming in and should output utf-8 encoded text in kind.
*/

using localization_lookup_proc_t = std::function<std::string(const std::string&)>;

void localization_register(const localization_lookup_proc_t& proc);

std::string localization_invoke(const std::string& source);

bool localization_ready();

//! @}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
