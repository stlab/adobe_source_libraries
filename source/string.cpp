/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#if 0

#include <adobe/string.hpp>

#include <cstring>

#if defined(ADOBE_STD_SERIALIZATION)
#include <ostream>
#endif

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

namespace version_1 {

string_t::string_t(const char* s)
{
	assign(s, s + std::strlen(s));
}

string_t::string_t(const char* s, std::size_t length)
{
	assign(s, s + length);
}

string_t::string_t(const std::string& s)
{
	assign(s.begin(), s.end());
}

string_t::size_type string_t::capacity() const
{
	const storage_type::size_type storage_capacity = storage_m.capacity();
	return (0 == storage_capacity ? 0 : storage_capacity - 1);
}

void string_t::push_back(value_type c)
{
	if (!empty())
		storage_m.pop_back();
	
	storage_m.push_back(c);
	storage_m.push_back(value_type(0));
}

/**************************************************************************************************/

string16_t::string16_t(const std::uint16_t* s, std::size_t length)
{
	assign(s, s + length);
}

string16_t::string16_t(const std::uint16_t* s)
{
	const std::uint16_t* last(s);
	while (0 != *last) ++last;
	assign(s, last);
}

const std::uint16_t* string16_t::c_str() const
{
	static const std::uint16_t empty_string_s(0);
	
	return empty() ? &empty_string_s : &storage_m[0];
}

string16_t::size_type string16_t::capacity() const
{
	const storage_type::size_type storage_capacity = storage_m.capacity();
	return (0 == storage_capacity ? 0 : storage_capacity - 1);
}

void string16_t::push_back(value_type c)
{
	if (!empty())
		storage_m.pop_back();
	
	storage_m.push_back(c);
	storage_m.push_back(value_type(0));
}

void string16_t::append(const std::uint16_t* s)
{
	const std::uint16_t* last(s);
	while (0 != *last) ++last;
	append(s, last);
}

/**************************************************************************************************/

#if defined(ADOBE_STD_SERIALIZATION)

std::ostream& operator<<(std::ostream& os, const string_t& t)
{ return os << t.c_str(); }

#endif

/**************************************************************************************************/

} // namespace version_1

/**************************************************************************************************/

} // namespace adobe

#endif
