/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/typeinfo.hpp>

using namespace std;

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

bad_cast::bad_cast() : what_m("bad_cast") { }

bad_cast::bad_cast(const bad_cast& error) : what_m(error.what_m) { }

/*
REVISIT (sparent) : This is only for debugging, but a reliable way to map a type to a name would
be a useful addition.
*/

bad_cast::bad_cast(const std::type_info& from, const std::type_info& to) :
    what_m(string() + "bad_cast: " + from.name() + " -> " + to.name())
{
}

bad_cast& bad_cast::operator=(const bad_cast& error)
{ what_m = error.what_m; return *this; }

bad_cast::~bad_cast() throw()
{ }

const char* bad_cast::what() const throw()
{ return what_m.c_str(); }

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
