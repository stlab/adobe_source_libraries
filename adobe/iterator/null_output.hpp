/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ITERATOR_NULL_OUTPUT_HPP
#define ADOBE_ITERATOR_NULL_OUTPUT_HPP

#include <iterator>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

struct null_output_t {
    using iterator_category = std::output_iterator_tag;

    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    constexpr null_output_t& operator++(int) { return *this; }
    constexpr null_output_t& operator++() { return *this; }
    constexpr null_output_t& operator*() { return *this; }

    template <class T>
    constexpr null_output_t& operator=(const T&) {
        return *this;
    }
};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif
// ADOBE_ITERATOR_NULL_OUTPUT_HPP
