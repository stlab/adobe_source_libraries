/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)
*/

/**************************************************************************************************/

#include <cstddef>
#include <new>

#include <adobe/memory.hpp>

namespace {

void* new_s(std::size_t n) { return ::operator new(n, std::nothrow); }

void delete_s(void* p) { return ::operator delete(p, std::nothrow); }

} // namespace

namespace adobe {
inline namespace version_1 {

const new_delete_t local_new_delete_g = {new_s, delete_s};

} // namespace version_1
} // namespace adobe
