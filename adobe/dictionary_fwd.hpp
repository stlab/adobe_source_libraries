/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_DICTIONARY_FWD_HPP
#define ADOBE_DICTIONARY_FWD_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/any_regular_fwd.hpp>
#include <adobe/closed_hash_fwd.hpp>
#include <adobe/name.hpp>

/**************************************************************************************************/

namespace adobe {
inline namespace version_1 {

/**************************************************************************************************/

typedef closed_hash_map<name_t, any_regular_t> dictionary_t;

/**************************************************************************************************/

} // namespace version_1

} // namespace adobe

/**************************************************************************************************/

#endif
