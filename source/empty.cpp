/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/empty.hpp>

#if defined(ADOBE_STD_SERIALIZATION)
#include <ostream>
#endif

/**************************************************************************************************/

namespace adobe {
namespace version_1 {

/**************************************************************************************************/

#if defined(ADOBE_STD_SERIALIZATION)
std::ostream& operator << (std::ostream& stream, const empty_t&)
{
    stream << "empty"; // REVISIT (sparent) : Go through formatter.
    return stream;
}
#endif

/**************************************************************************************************/

} // namespace version_1
} // namespace adobe

/**************************************************************************************************/
