/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_LEX_STREAM_FWD_HPP
#define ADOBE_LEX_STREAM_FWD_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/implementation/lex_shared_fwd.hpp>

#include <functional>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

typedef bool(keyword_extension_lookup_proc_signature_t)(const name_t&);
using keyword_extension_lookup_proc_t = std::function<keyword_extension_lookup_proc_signature_t>;

/**************************************************************************************************/

class lex_stream_t;

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
