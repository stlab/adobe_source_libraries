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
namespace version_1 {

/**************************************************************************************************/

//!\ingroup abi_container
typedef closed_hash_map<name_t, any_regular_t> dictionary_t;

/**************************************************************************************************/

} // namespace version_1

using version_1::dictionary_t;

#if defined(ADOBE_NO_DOCUMENTATION)
/* REVISIT (mmarcus) : doxygen doesn't seem to understand using
declarartions.  This is a doxygen only hack to keep reference links
from breaking.
*/
//!\ingroup abi_container
typedef version1::closed_hash_map<name_t, any_regular_t> dictionary_t;
#endif

} // namespace adobe

/**************************************************************************************************/

#endif
