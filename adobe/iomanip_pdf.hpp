/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifdef ADOBE_STD_SERIALIZATION

/**************************************************************************************************/

#ifndef ADOBE_IOMANIP_PDF_HPP
#define ADOBE_IOMANIP_PDF_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/iomanip.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

//!\ingroup manipulator
class pdf_format : public format_base {
    typedef format_base inherited_t;

public:
    typedef inherited_t::stream_type stream_type;

    virtual void begin_format(stream_type& os);

    virtual void begin_bag(stream_type& os, const std::string& ident);

    virtual void begin_sequence(stream_type& os);

    virtual void begin_atom(stream_type& os, const serializable_t&);

private:
    virtual void stack_event(stream_type& os, bool is_push);

    void handle_atom(stream_type& os, bool is_push);
};

/**************************************************************************************************/

//!\ingroup manipulator
std::ostream& begin_pdf(std::ostream& os);

//!\ingroup manipulator
std::ostream& end_pdf(std::ostream& os);

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/

#endif

/**************************************************************************************************/
