/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_TYPEINFO_HPP
#define ADOBE_TYPEINFO_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <string>
#include <typeinfo>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

/*!
\ingroup type_info_related
\brief An exception class thrown during ASL failures to cast.

<code>adobe::bad_cast</code> is a decendant of <code>std::exception</code>.
It is intended to provide detailed type information regarding the parameters
into a cast that failed. If no <code>typeid()</code> information was used to
construct the object, it simply relays "bad_cast".
*/
class bad_cast : public std::bad_cast {
public:
    bad_cast();
    bad_cast(const std::type_info& from, const std::type_info& to);
    bad_cast(const bad_cast&);
    bad_cast& operator=(const bad_cast&);
    virtual ~bad_cast() throw();
    /*!
    \return
        Either:
            - The string "bad_cast"
            - A string detailing the source and destination types that could not be cast
    successfully.
    */
    const char* what() const throw();

private:
    std::string what_m;
};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/


#endif

/**************************************************************************************************/
