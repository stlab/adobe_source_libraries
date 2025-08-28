/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_ENUM_OPS_HPP
#define ADOBE_ENUM_OPS_HPP

#include <stlab/enum_ops.hpp>

/**************************************************************************************************/

/// \file enum_ops.hpp
/// \brief Bitwise and arithmetic operator helpers for enums.
/// \ingroup utility

/**************************************************************************************************/

/// \deprecated Use stlab_enable_bitmask_enum instead.
#define ADOBE_DEFINE_BITSET_OPS(EnumType) auto stlab_enable_bitmask_enum(EnumType)->std::true_type;

/// \deprecated Use stlab_enable_arithmetic_enum instead.
#define ADOBE_DEFINE_ARITHMETIC_OPS(EnumType)                                                      \
    auto stlab_enable_arithmetic_enum(EnumType)->std::true_type;

/**************************************************************************************************/

#endif

/**************************************************************************************************/
