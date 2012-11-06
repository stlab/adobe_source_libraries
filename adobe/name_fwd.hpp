/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_NAME_FWD_HPP
#define ADOBE_NAME_FWD_HPP

#include <adobe/config.hpp>

#include <cstddef>
#include <cassert>

#include <boost/operators.hpp>
#include <boost/type_traits/is_pod.hpp>
#include <boost/type_traits/has_nothrow_constructor.hpp>

#include <adobe/implementation/swap.hpp>

#include <adobe/empty.hpp>

#if defined(ADOBE_STD_SERIALIZATION)
#include <iosfwd>
#endif

/*************************************************************************************************/

namespace adobe {

namespace version_1 {

class name_t;

inline bool operator < (const name_t& x, const name_t& y);
inline bool operator == (const name_t& x, const name_t& y);

/*************************************************************************************************/

class name_t : boost::totally_ordered<name_t, name_t>
{
 private:
    operator int () const;
 public:

    explicit name_t (const char* string_name = "");
                
    const char* c_str() const;

#if !defined(ADOBE_NO_DOCUMENTATION)
    operator bool() const;
    bool operator!() const;
    
    friend void swap(name_t& x, name_t& y) { std::swap(x.name_m, y.name_m); }
#endif

 private:
    friend class static_name_t;
    friend struct aggregate_name_t;
    
    struct dont_copy_t { };
    struct dont_initialize_t { };
    
    name_t(const char* x, dont_copy_t);
    
    name_t (dont_initialize_t) { }
    
    const char* name_m;
};
    
/*************************************************************************************************/

class static_name_t;
struct aggregate_name_t;

/*************************************************************************************************/

#if defined(ADOBE_STD_SERIALIZATION)
std::ostream& operator << (std::ostream& os, const name_t& t);
#endif

/*************************************************************************************************/

} // namespace version_1

using version_1::name_t;
using version_1::static_name_t;
using version_1::aggregate_name_t;

} // namespace adobe

/*************************************************************************************************/

namespace boost {

template <> struct is_pod<adobe::name_t> : boost::mpl::true_ { };
    // implies has_nothrow_constructor and has_nothrow_copy

}

/*************************************************************************************************/

#endif // ADOBE_NAME_FWD_HPP

/*************************************************************************************************/
