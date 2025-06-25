/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/empty.hpp>

#if defined(ADOBE_STD_SERIALIZATION)
#include <ostream>
#endif

/**************************************************************************************************/

namespace adobe {
inline namespace version_1 {

/**************************************************************************************************/

#if defined(ADOBE_STD_SERIALIZATION)
std::ostream& operator<<(std::ostream& stream, const empty_t&) {
    stream << "empty"; // REVISIT (sparent) : Go through formatter.
    return stream;
}
#endif

/**************************************************************************************************/

} // namespace version_1
} // namespace adobe

/**************************************************************************************************/
