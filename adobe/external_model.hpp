/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_EXTERNAL_MODEL_HPP
#define ADOBE_EXTERNAL_MODEL_HPP

#include <adobe/config.hpp>

#include <deque>
#include <functional>
#include <map>

#include <boost/signals2/signal.hpp>

#include <adobe/any_regular_fwd.hpp>
#include <adobe/name.hpp>
#include <adobe/string.hpp>

/**************************************************************************************************/

namespace adobe {
/*!
\defgroup external_model External Model
\ingroup property_model
 */

/**************************************************************************************************/

/*!
\ingroup external_model
 */
class external_model_t : boost::noncopyable {
public:
    using connection_t = boost::signals2::connection;
    using monitor_t = std::function<void(const any_regular_t&)>;

    void add_cell(name_t);

    std::size_t count(name_t) const;
    connection_t monitor(name_t name, const monitor_t& monitor);
    void set(name_t, const any_regular_t&);

    void model_monitor(name_t name, const monitor_t& monitor);
    void model_set(name_t, const any_regular_t&);

private:
    typedef boost::signals2::signal<void(const any_regular_t&)> monitor_list_t;

    struct cell_t {
        // empty copy and assignment - we don't move connections.
        cell_t() {}
        cell_t(const cell_t&) {}
        cell_t& operator=(const cell_t&) { return *this; }

        monitor_list_t monitor_m;
        monitor_t model_monitor_m;
    };

    typedef std::map<const char*, cell_t*, str_less_t> index_t;

    cell_t* lookup(name_t);

    index_t index_m;
    std::deque<cell_t> cell_set_m;
};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
