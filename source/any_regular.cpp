/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#if defined(ADOBE_STD_SERIALIZATION)

#include <adobe/any_regular.hpp>

#include <cassert>
#include <string>

#ifdef ADOBE_BUILT_WITH_CMAKE
#include <double-conversion/double-conversion.h>
#else
#include <double-conversion/src/double-conversion.h>
#endif

#include <adobe/algorithm/sorted.hpp>
#include <adobe/algorithm/lower_bound.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/iomanip.hpp>
#include <adobe/name.hpp>
#include <adobe/string.hpp>
#include <adobe/table_index.hpp>

#include <ostream>

using namespace std;

/**************************************************************************************************/

namespace {

using namespace adobe;

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

namespace version_1 {

std::ostream& operator<<(std::ostream& out, const dictionary_t& x) {
    typedef table_index<const name_t, const dictionary_t::value_type> index_type;

    index_type index(x.begin(), x.end(), &dictionary_t::value_type::first);
    index.sort();

    out << begin_bag("[0]");

    for (index_type::const_iterator first(index.begin()), last(index.end()); first != last;
         ++first) {
        out << begin_sequence;
        out << format(first->first);
        out << first->second; // format will be called by any_regular_t.
        out << end_sequence;
    }

    out << end_bag;

    return out;
}

} // namespace version_1

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif
