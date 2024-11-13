/*
    Copyright 2012 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_MACRO_UTILITIES_HPP

/// turns `x` into a string literal.
#define ADOBE_STRINGIZE(x)                                                                         \
    ADOBE_DETAIL_STRINGIZE2(x)        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define ADOBE_DETAIL_STRINGIZE2(x) #x // NOLINT(cppcoreguidelines-macro-usage)


/// a string literal for __LINE__.
#define ADOBE_LINE_STRING() ADOBE_STRINGIZE(__LINE__) // NOLINT(cppcoreguidelines-macro-usage)

/// Expands to a [GNU error formatted](https://www.gnu.org/prep/standards/standards.html#Errors)
/// string literal reporting `message` at the line where it is invoked.
///
/// - Requires: `message` is a string literal.
#define ADOBE_MESSAGE(message)                                                                     \
    __FILE__ ":" ADOBE_LINE_STRING() ": " message // NOLINT(cppcoreguidelines-macro-usage)

/**************************************************************************************************/

#endif

/**************************************************************************************************/
