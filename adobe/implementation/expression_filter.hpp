/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_EXPRESSION_FILTER_HPP
#define ADOBE_EXPRESSION_FILTER_HPP

#include <adobe/config.hpp>

#include <cstdint>
#include <adobe/string.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

/*!
    Takes a unicode code point and returns the equivalent XML entity name.

    Throws if the code point does not have an XML entity mapping.

    Note: The entity name does not contain the standard
          entity prefix ('&') or suffix (';')
*/
const std::string& entity_map_find(std::uint32_t code_point);

/*!
    Takes an XML entity name and returns the equivalent unicode code point.

    Throws if the XML entity name is invalid.

    Note: The entity name must not contain the standard
          entity prefix ('&') or suffix (';')
*/
std::uint32_t entity_map_find(const std::string& entity);

/*!
    Returns whether or not a string has at least one character in it
    that would be escaped by entity_escape; can be faster than
    escape-and-compare, as it could return true before the entire string
    is scanned.
*/
bool needs_entity_escape(const std::string& value);

/*!
    Takes a string and converts certain ascii characters found within to
    their xml entity equivalents, returning the "entity formatted" string.
    "certain ascii characters" are defined as:
        - apostrophe (') or quote (")
        - characters with an xml entity mapping
        - non-printable ASCII characters
        - characters with the high-bit set
*/
std::string entity_escape(const std::string& value);

/*!
    Returns whether or not a string has at least one entity in it that
    would be unescaped by entity_unescape; can be faster than
    unescape-and-compare, as it could return true before the entire
    string is scanned.
*/
bool needs_entity_unescape(const std::string& value);

/*!
    Takes a string and converts the xml entity declarations found within to
    their ASCII equivalents, returning the "entity flattened" string.
*/
std::string entity_unescape(const std::string& value);

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/
// ADOBE_EXPRESSION_FILTER_HPP
#endif

/**************************************************************************************************/
