/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_STORAGE_HPP
#define ADOBE_STORAGE_HPP

#include <adobe/config.hpp>

#include <adobe/container/std_fwd.hpp>

/**************************************************************************************************/

namespace adobe {

template <typename T> // T models Container
struct storage_category;

class block_tag {};
class contiguous_tag : public block_tag {};
class node_tag {};

template <typename T, typename A>
struct storage_category<std::vector<T, A>> {
    typedef contiguous_tag type;
};

template <typename T, typename A>
struct storage_category<std::deque<T, A>> {
    typedef block_tag type;
};

template <typename T, typename A>
struct storage_category<std::list<T, A>> {
    typedef node_tag type;
};

template <typename T, typename C, typename A>
struct storage_category<std::set<T, C, A>> {
    typedef node_tag type;
};

template <typename T, typename C, typename A>
struct storage_category<std::multiset<T, C, A>> {
    typedef node_tag type;
};

template <typename K, typename T, typename C, typename A>
struct storage_category<std::map<K, T, C, A>> {
    typedef node_tag type;
};

template <typename K, typename T, typename C, typename A>
struct storage_category<std::multimap<K, T, C, A>> {
    typedef node_tag type;
};

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
