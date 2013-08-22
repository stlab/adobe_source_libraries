/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/name.hpp>

#include <mutex>

#include <adobe/once.hpp>
#include <adobe/implementation/string_pool.hpp>

#if defined(ADOBE_STD_SERIALIZATION)
#include <ostream>
#endif

/**************************************************************************************************/

using namespace std;
using namespace adobe;

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

// Precondition: length only need be non-zero if not copying
// Precondition: if string_name is null - length must be zero
const char* unique_string(const char* string_name)
{
    static const char* empty_string_s = "";

    if (!string_name || !*string_name) return empty_string_s;
    
    ADOBE_THREAD_SAFE static unique_string_pool_t unique_string_s;
    ADOBE_THREAD_SAFE static mutex sync_s;
    
    lock_guard<mutex> lock(sync_s);

    return unique_string_s.add(string_name);
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {
namespace version_1 {

/**************************************************************************************************/

name_t::name_t (const char* x) : name_m(unique_string(x)) { }

/*
  REVISIT (seanparent) : With "gcc version 4.2.1 (Apple Inc. build 5574)" using -O3 this code
  will generate an odd error if inlined in the header. So moved to the .cpp file.
  
  There was a similar error with 4.0.1 (5465) when the function was inlined in the header but
  not directly in the class. Very odd.
*/

name_t::name_t (const char* x, dont_copy_t) : name_m(x) {
  assert(name_m && "WARNING (sparent) : Null string_name in name_t");
}


/**************************************************************************************************/

#if defined(ADOBE_STD_SERIALIZATION)

ostream& operator << (ostream& os, const name_t& t)
{
    os << t.c_str();
    return os;
}

#endif

/**************************************************************************************************/

} // namespace version_1
} // namespace adobe

/**************************************************************************************************/
