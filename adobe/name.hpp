/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_NAME_HPP
#define ADOBE_NAME_HPP

#include <adobe/config.hpp>

#include <adobe/name_fwd.hpp>

#include <adobe/conversion.hpp>
#include <adobe/cstring.hpp>
#include <adobe/fnv.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

namespace version_1 {

/*! \addtogroup abi_string
@{
*/

/*************************************************************************************************/

#if !defined(ADOBE_NO_DOCUMENTATION)

inline name_t::operator bool() const { return *name_m != 0; }

inline bool name_t::operator!() const { return !(*name_m); }

#endif

/*************************************************************************************************/

inline bool operator<(const name_t& x, const name_t& y)
{
    return adobe::strcmp(x.c_str(), y.c_str()) < 0;
}

inline bool operator == (const name_t& x, const name_t& y)
{
    /*
        The test case for equal strings is "optimized" because names are stored in hash tables and
        will often match on a find because the compiler will pool string constants.
    */

    return x.c_str() == y.c_str() ||
           adobe::strcmp(x.c_str(), y.c_str()) == 0;
}

/*************************************************************************************************/

inline const char* name_t::c_str() const
{
    return name_m;
}

/*************************************************************************************************/

class static_name_t : public name_t
{
 public:
    explicit static_name_t (const char* string_name = "") :
        name_t(string_name, dont_copy_t()) { }
};

struct aggregate_name_t
{
    const char* const name_m;
    operator name_t() const { return name_t(name_m, name_t::dont_copy_t()); }
};

//!@}

/*************************************************************************************************/

} // namespace version_1
    
/*************************************************************************************************/

template <> struct promote<static_name_t> { typedef name_t type; };
template <> struct promote<aggregate_name_t> { typedef name_t type; };
    
/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

namespace std {

/*************************************************************************************************/
/**
    Template specialization for std::hash<adobe::name_t>. This way std::hash
    will either use this specialization or fail to compile, instead of
    giving you a maybe-silent-fail-maybe-not result based on argument
    dependent lookup.
*/
template<>
struct hash<adobe::name_t>
{
public:
    std::size_t operator()(adobe::name_t const& name) const 
    {
        const char* first(name.c_str());
        const char* last(first);

        while (*last)
            ++last;

        return adobe::fnv1a<sizeof(std::size_t) * 8>(first, last);
     }
};

/*************************************************************************************************/

} // namespace std

/*************************************************************************************************/

#endif

/*************************************************************************************************/
