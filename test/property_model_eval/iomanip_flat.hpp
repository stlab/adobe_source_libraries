/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifdef ADOBE_STD_SERIALIZATION

/*************************************************************************************************/

#ifndef ADOBE_IOMANIP_FLAT_HPP
#define ADOBE_IOMANIP_FLAT_HPP

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/iomanip.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

//!\ingroup manipulator
class flat_format : public format_base {
    typedef format_base inherited_t;

public:
    typedef inherited_t::stream_type stream_type;

    explicit flat_format(bool safe_strings) : escape_m(safe_strings) {}

    virtual void begin_format(stream_type& os);

    virtual void begin_bag(stream_type& os, const std::string& ident);

    virtual void begin_sequence(stream_type& os);

    virtual void begin_atom(stream_type& os, const serializable_t&);

private:
    virtual void stack_event(stream_type& os, bool is_push);

    void handle_atom(stream_type& os, bool is_push);

    bool escape_m;
};

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/

#endif

/*************************************************************************************************/
