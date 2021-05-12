/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ITERATOR_VALUE_ITERATOR_HPP
#define ADOBE_ITERATOR_VALUE_ITERATOR_HPP

#include <adobe/config.hpp>

#include <cassert>
#include <functional>
#include <iterator>

#include <adobe/functional.hpp>
#include <adobe/type_traits.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

//! \addtogroup adobe_iterator
//! @{


template <typename I,               // I models Incrementable
          typename F = identity<I>> // F models UnaryFunction
class value_iterator {
public:
    using value_type = adobe::invoke_result_t<F, I>;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef ptrdiff_t difference_type;
    typedef std::forward_iterator_tag iterator_category;

private:
    I i;
    F f;

public:
    value_iterator() {}

    value_iterator(const I& x, const F& y) : i(x), f(y) {}

    value_iterator& operator++() {
        ++i;
        return *this;
    }

    value_iterator operator++(int) {
        value_iterator tmp = *this;

        ++*this;

        return tmp;
    }

    const value_type& operator*() const { return f(i); }

    value_type operator*() { return f(i); }

    friend bool operator==(const value_iterator& a, const value_iterator& b) {
        // assert(a.f == b.f);

        return a.i == b.i;
    }

    friend bool operator!=(const value_iterator& a, const value_iterator& b) { return !(a == b); }
};

//! @}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif
// ADOBE_ITERATOR_DISTANCE_HPP
