/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/typeinfo.hpp>

#include <fstream>
#include <iterator>
#include <map>
#include <sstream>

#include <boost/range/as_literal.hpp>

#include <adobe/string.hpp>
#include <adobe/cstring.hpp>

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
    what_m("bad_cast: ")
{
    what_m << from.name() << " -> " << to.name();
}

bad_cast::bad_cast(const type_info_t& from, const type_info_t& to) :
    what_m("bad_cast: ")
{
    std::back_insert_iterator<std::string> out(what_m);
    out = serialize(from, out);
    out = copy(boost::as_literal(" -> "), out);
    out = serialize(to, out);
}

bad_cast& bad_cast::operator=(const bad_cast& error)
{ what_m = error.what_m; return *this; }

bad_cast::~bad_cast() throw()
{ }

const char* bad_cast::what() const throw()
{ return what_m.c_str(); }

/**************************************************************************************************/

#ifndef NDEBUG

namespace version_1 {

std::ostream& operator<<(std::ostream& stream, const type_info_t& x)
{
    std::ostream_iterator<char> out(stream);
    serialize(x, out);
    return stream;
}

} // namespace version_1

#endif

/**************************************************************************************************/

namespace implementation {


/**************************************************************************************************/

const char* cppfilt(const std::type_info& type)
{
#if defined(__GNUC__)
    /*
        WARNING (fbrereto) : This code is not thread safe! iostreams in general
                             are not, so it is the responsibility of the client
                             to assert thread safety for any serialization.

        This bit of code attempts to use std::system to make a call out to
        c++filt in an attempt to demangle the c++ type name found in the
        type_info struct. This code is only active for gcc, as 1) c++filt is
        only part of the GNU development tools, and 2) MSVC already
        pretty-prints its type_info name. There are many things that can go
        wrong with this code, so I've tried to be as conservative as possible in
        making the call out to c++filt. Given any error state the (mangled) name
        found in the std::type_info struct will be used.

        All calls to std::system are synchronous.
    */
    static const bool system_available_s(std::system(0) != 0);
    static const bool cpp_filt_available_s(system_available_s && std::system("c++filt foo > /dev/null") == 0);

    if (!cpp_filt_available_s)
        return type.name();

    typedef std::map<const std::type_info*, std::string> demangle_map_t;

    // We cache the demangled types as we look them up; solves the const char*
    // ownership problem and speeds things up the next time around.
    static demangle_map_t demangle_map_s;

    demangle_map_t::const_iterator found(demangle_map_s.find(&type));

    if (found != demangle_map_s.end()) // cache hit
        return found->second.c_str();

    // This is a bit hacky... std::system returns the error code from the shell,
    // and that's it, so any information produced by c++filt is piped to a temp
    // file and sucked back in using an input file stream.

    static const char* filepath("/tmp/cppfilt_demangle.txt");
    std::stringstream  stream;

    stream << "c++filt -_ -t '" << type.name() << "' > " << filepath;

    if (std::system(stream.str().c_str()) != 0)
        return type.name();

    std::ifstream input(filepath);

    if (input.is_open() == false)
        return type.name();

    std::string result;

    getline(input, result);

    demangle_map_s[&type].assign(result);

    return demangle_map_s[&type].c_str();
#else
    return type.name();
#endif
}

/**************************************************************************************************/

bool type_instance_t::requires_std_rtti() const
{
    if (type_info_m) return true;
    
    for (const type_instance_t* const* xp = &parameter_m[0]; *xp; ++xp) {
        if ((*xp)->requires_std_rtti()) return true;
    }
    
    return false;
}

bool operator==(const type_instance_t& x, const type_instance_t& y)
{
    /*
        NOTE (sparent@adobe.com) : Because we frequently check type info's when we know they should
        be equal and the identity will be equal unless they have been passed through a DLL boundary,
        this is a valid optimization.
    */
    
    if (&x == &y) return true;

    if (x.type_info_m) {
        if (y.type_info_m) return *x.type_info_m == *y.type_info_m;
        return false;
    }
    if (y.type_info_m) return false;
    
    if (adobe::strcmp(x.name_m, y.name_m) != 0) return false;

    const type_instance_t* const* xp = &x.parameter_m[0];
    const type_instance_t* const* yp = &y.parameter_m[0];
    
    while (*xp && *yp) {
        if (**xp != **yp) return false;
        ++xp;
        ++yp;
    }
    return *xp == *yp; // at least one is zero - both zero implies equality.
}

bool before(const type_instance_t& x, const type_instance_t& y)
{
    if (x.type_info_m) {
        if (y.type_info_m) return x.type_info_m->before(*y.type_info_m) != 0;
        return false; // All local types sort after named types
    }
    if (y.type_info_m) return true;
    
    int c = adobe::strcmp(x.name_m, y.name_m);
    if (c != 0) return c < 0 ? true : false;
    
    const type_instance_t* const* xp = &x.parameter_m[0];
    const type_instance_t* const* yp = &y.parameter_m[0];
    
    /*
        REVISIT (sparent) : The two compares in this loop are necessary for a lexicographical
        compare. An alternative would be a single three - way compare. That might end up being
        less work overall. If too much time is spent in this code then it should be considered.
    */
    
    while (*xp && *yp) {
        if (before(**xp, **yp)) return true;
        if (before(**yp, **xp)) return false;
        ++xp;
        ++yp;
    }
    return *yp != 0;
}

/**************************************************************************************************/

} // namespace implementation

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
