/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_LAYOUT_ATTRIBUTES_HPP
#define ADOBE_LAYOUT_ATTRIBUTES_HPP

#include <adobe/config.hpp>
#include <adobe/extents.hpp>

#include <array>

/*!
\defgroup layout_attributes Layout Attributes
\ingroup layout_library
*/


namespace adobe {

/**************************************************************************************************/

/*!
\ingroup layout_attributes
*/
struct layout_attributes_alignment_t {
    enum alignment_t {
        align_forward = 0, // Used as index for guide array.
        align_reverse = 1, // Used as index for guide array.
        align_center,
        align_proportional,
        align_forward_fill,
        align_reverse_fill,
        align_default,
        align_fill = align_forward_fill,
        align_left_fill = align_forward_fill,
        align_right_fill = align_reverse_fill,
        align_top_fill = align_forward_fill,
        align_bottom_fill = align_reverse_fill,
        align_left = align_forward,
        align_right = align_reverse,
        align_top = align_forward,
        align_bottom = align_reverse
    };
};

/*!
\ingroup layout_attributes
*/
struct layout_attributes_placement_t {
    enum placement_t { place_leaf, place_column, place_row, place_overlay };
};

/*!
\ingroup layout_attributes
*/
struct layout_attributes_t
#if !defined(ADOBE_NO_DOCUMENTATION)
    : public extents_slices_t,
      public layout_attributes_alignment_t,
      public layout_attributes_placement_t
#endif
{
    layout_attributes_t() : indent_m(0), create_m(true), spacing_m(2, 0), placement_m(place_leaf) {
        spacing_m[1] = 10; /* REVISIT FIXED VALUE container_spacing */
    }

    typedef std::vector<int> spacing_t;

    struct slice_t {
        slice_t()
            : alignment_m(align_default), suppress_m(false), balance_m(false),
              child_alignment_m(align_forward) {}

        alignment_t alignment_m;
        bool suppress_m;
        bool balance_m;

        // containers only
        pair_long_t margin_m;
        alignment_t child_alignment_m;
    };

    extents_t extents_m;
    int indent_m;
    bool create_m;
    spacing_t spacing_m;
    std::array<slice_t, 2> slice_m;

    // containers only
    placement_t placement_m;

    slice_t& vertical() { return slice_m[extents_slices_t::vertical]; }
    slice_t& horizontal() { return slice_m[extents_slices_t::horizontal]; }

    const slice_t& vertical() const { return slice_m[extents_slices_t::vertical]; }
    const slice_t& horizontal() const { return slice_m[extents_slices_t::horizontal]; }


    int& height() { return extents_m.height(); }
    int& width() { return extents_m.width(); }

    const int& height() const { return extents_m.height(); }
    const int& width() const { return extents_m.width(); }
};

/**************************************************************************************************/

/*!
\ingroup layout_attributes
*/
struct place_data_t
#if !defined(ADOBE_NO_DOCUMENTATION)
    : extents_slices_t
#endif
{
    struct slice_t {
#if !defined(ADOBE_NO_DOCUMENTATION)
        slice_t();
#endif

        int length_m;
        int position_m;
        pair_long_t outset_m;
        guide_set_t guide_set_m;
    };

    std::array<slice_t, 2> slice_m;

    slice_t& vertical() { return slice_m[extents_slices_t::vertical]; }
    slice_t& horizontal() { return slice_m[extents_slices_t::horizontal]; }

    const slice_t& vertical() const { return slice_m[extents_slices_t::vertical]; }
    const slice_t& horizontal() const { return slice_m[extents_slices_t::horizontal]; }
};


/*!
\ingroup layout_attributes
*/
inline int top(const place_data_t& place_data) { return place_data.vertical().position_m; }

/*!
\ingroup layout_attributes
*/
inline int left(const place_data_t& place_data) { return place_data.horizontal().position_m; }

/*!
\ingroup layout_attributes
*/
inline int& top(place_data_t& place_data) { return place_data.vertical().position_m; }

/*!
\ingroup layout_attributes
*/
inline int& left(place_data_t& place_data) { return place_data.horizontal().position_m; }

/*!
\ingroup layout_attributes
*/
inline int height(const place_data_t& place_data) { return place_data.vertical().length_m; }

/*!
\ingroup layout_attributes
*/
inline int width(const place_data_t& place_data) { return place_data.horizontal().length_m; }

/*!
\ingroup layout_attributes
*/
inline int& height(place_data_t& place_data) { return place_data.vertical().length_m; }

/*!
\ingroup layout_attributes
*/
inline int& width(place_data_t& place_data) { return place_data.horizontal().length_m; }

/*!
\ingroup layout_attributes
*/
inline int bottom(const place_data_t& place_data) { return top(place_data) + height(place_data); }

/*!
\ingroup layout_attributes
*/
inline int right(const place_data_t& place_data) { return left(place_data) + width(place_data); }

/**************************************************************************************************/
} // namespace adobe


#endif
