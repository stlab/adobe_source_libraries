/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_STD_FWD_HPP
#define ADOBE_STD_FWD_HPP

#include <adobe/config.hpp>

/**************************************************************************************************/

// REVISIT (sparent) : Forward declarations not working for C++11. Remove or fix this
// file.

#if 1 || (BOOST_WORKAROUND(__GNUC__, >= 3) && defined(_GLIBCXX_DEBUG))

#include <deque>
#include <list>
#include <map>
#include <set>
#include <vector>

#else

namespace std {
template <typename, typename>
class pair;
template <typename>
class allocator;
template <typename T, typename = std::allocator<T>>
class vector;
template <typename T, typename = std::allocator<T>>
class deque;
template <typename T, typename = std::allocator<T>>
class list;
template <typename T, typename, typename = std::allocator<T>>
class set;
template <typename T, typename, typename = std::allocator<T>>
class multiset;
template <typename K, typename T, typename, typename = std::allocator<std::pair<const K, T>>>
class map;
template <typename K, typename T, typename, typename = std::allocator<std::pair<const K, T>>>
class multimap;

} // namespace std

#endif

/**************************************************************************************************/

#endif

/**************************************************************************************************/
