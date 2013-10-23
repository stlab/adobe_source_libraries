/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifdef ADOBE_STD_SERIALIZATION

/*************************************************************************************************/

#ifndef ADOBE_IOMANIP_JAVASCRIPT_HPP
#define ADOBE_IOMANIP_JAVASCRIPT_HPP

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/iomanip.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

//!\ingroup manipulator
class javascript_format : public format_base
{
    typedef format_base inherited_t;

public:
    typedef inherited_t::stream_type stream_type;

    virtual void begin_format(stream_type& os);

    virtual void begin_bag(stream_type& os, const std::string& ident);

    virtual void begin_sequence(stream_type& os);

    virtual void begin_atom(stream_type& os, const any_regular_t&);

private:
    virtual void stack_event(stream_type& os, bool is_push);

    void handle_atom(stream_type& os, bool is_push);
};

/*************************************************************************************************/

//!\ingroup manipulator
inline std::ostream& begin_javascript(std::ostream& os)
{
    replace_pword<format_base, javascript_format>(os, format_base_idx());
    return os << begin_format;
}

/*************************************************************************************************/

//!\ingroup manipulator
inline std::ostream& end_javascript(std::ostream& os)
{ return os << end_format; }

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/

#endif

/*************************************************************************************************/
