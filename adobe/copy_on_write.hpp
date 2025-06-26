/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_COPY_ON_WRITE_HPP
#define ADOBE_COPY_ON_WRITE_HPP

/**************************************************************************************************/

#include <stlab/copy_on_write.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

/*!
    An alias to [stlab/copy_on_write](https://stlab.github.io/copy-on-write) for compatibility.
*/

template <class T>
using copy_on_write = stlab::copy_on_write<T>;

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
