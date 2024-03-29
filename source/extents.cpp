/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#include <adobe/extents.hpp>

#include <adobe/algorithm/equal.hpp>

#if !defined(NDEBUG) && defined(ADOBE_STD_SERIALIZATION)
#include <adobe/any_regular.hpp>
#include <adobe/iomanip.hpp>
#include <adobe/name.hpp>
#include <iostream>
#endif

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

bool operator==(const adobe::extents_t& x, const adobe::extents_t& y) {
    return x.slice_m[adobe::extents_slices_t::vertical] ==
               y.slice_m[adobe::extents_slices_t::vertical] &&
           x.slice_m[adobe::extents_slices_t::horizontal] ==
               y.slice_m[adobe::extents_slices_t::horizontal];
}

/**************************************************************************************************/

#if !defined(NDEBUG) && defined(ADOBE_STD_SERIALIZATION)
std::ostream& operator<<(std::ostream& s, const adobe::extents_t& x) {
    s << adobe::begin_bag("[0]");

    s << adobe::begin_sequence;
    s << adobe::format("vertical"_name);
    s << adobe::format(x.slice_m[adobe::extents_slices_t::vertical]);
    s << adobe::end_sequence;

    s << adobe::begin_sequence;
    s << adobe::format("horizontal"_name);
    s << adobe::format(x.slice_m[adobe::extents_slices_t::horizontal]);
    s << adobe::end_sequence;

    s << adobe::end_bag;

    return s;
}
#endif

/**************************************************************************************************/

bool operator==(const adobe::extents_t::slice_t& x, const adobe::extents_t::slice_t& y) {
    return x.length_m == y.length_m && x.outset_m == y.outset_m && x.frame_m == y.frame_m &&
           x.inset_m == y.inset_m && x.guide_set_m.size() == y.guide_set_m.size() &&
           adobe::equal(x.guide_set_m, y.guide_set_m.begin());
}

/**************************************************************************************************/

#if !defined(NDEBUG) && defined(ADOBE_STD_SERIALIZATION)
std::ostream& operator<<(std::ostream& s, const adobe::extents_t::slice_t& x) {
    adobe::guide_set_t::const_iterator first(x.guide_set_m.begin());
    adobe::guide_set_t::const_iterator last(x.guide_set_m.end());

    s << adobe::begin_bag("[0]") << adobe::begin_sequence << adobe::format("length"_name)
      << adobe::format(x.length_m) << adobe::end_sequence << adobe::begin_sequence
      << adobe::format("outset"_name) << adobe::begin_sequence << adobe::format(x.outset_m.first)
      << adobe::format(x.outset_m.second) << adobe::end_sequence << adobe::end_sequence
      << adobe::begin_sequence << adobe::format("frame"_name) << adobe::begin_sequence
      << adobe::format(x.frame_m.first) << adobe::format(x.frame_m.second) << adobe::end_sequence
      << adobe::end_sequence << adobe::begin_sequence << adobe::format("inset"_name)
      << adobe::begin_sequence << adobe::format(x.inset_m.first) << adobe::format(x.inset_m.second)
      << adobe::end_sequence << adobe::end_sequence << adobe::begin_sequence
      << adobe::format("poi_set"_name) << adobe::begin_sequence;
    for (; first != last; ++first)
        s << adobe::format(*first);
    s << adobe::end_sequence << adobe::end_sequence << adobe::end_bag;

    return s;
}
#endif

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
