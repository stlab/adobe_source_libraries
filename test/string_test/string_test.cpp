/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#define BOOST_TEST_MAIN

#include <adobe/string.hpp>
#include <adobe/unicode.hpp>

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <string>

/*************************************************************************************************/

template <typename T>
struct test_traits
{
};

/*************************************************************************************************/

template <>
struct test_traits<char>
{
	typedef adobe::string_t		string_type;
	
	static const std::size_t	short_length_k;
	static const char			source_k[];
	static const std::size_t	source_length_k;

	static const char			first_segment_k[];
	static const char			second_segment_k[];
	static const char			full_string_k[];
	static const char			short_full_string_k[];
		
	static int compare(const char* s1, const char* s2)
		{ return std::strcmp(s1, s2); }

	static int compare_n(const char* s1, const char* s2, std::size_t n)
		{ return std::strncmp(s1, s2, n); }
};

const std::size_t	test_traits<char>::short_length_k = 5;
const char			test_traits<char>::source_k[] = "this is a test";
const std::size_t	test_traits<char>::source_length_k = std::strlen(source_k);

const char			test_traits<char>::first_segment_k[] = "This is";
const char			test_traits<char>::second_segment_k[] = " a test";
const char			test_traits<char>::full_string_k[] = "This is a test";
const char			test_traits<char>::short_full_string_k[] = "This is a te";

/*************************************************************************************************/

template <>
struct test_traits<boost::uint16_t>
{
	typedef adobe::string16_t		string_type;
	
	static const std::size_t		short_length_k;
	static const boost::uint16_t	source_k[];
	static const std::size_t		source_length_k;

	static const boost::uint16_t	first_segment_k[];
	static const boost::uint16_t	second_segment_k[];
	static const boost::uint16_t	full_string_k[];
	static const boost::uint16_t	short_full_string_k[];
	
	static int compare(const boost::uint16_t* s1, const boost::uint16_t* s2);
	
	static int compare_n(const boost::uint16_t* s1, const boost::uint16_t* s2, std::size_t n);
};

const std::size_t		test_traits<boost::uint16_t>::short_length_k = 5;
const boost::uint16_t	test_traits<boost::uint16_t>::source_k[] = { 't', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 't', 'e', 's', 't', 0 };
const std::size_t		test_traits<boost::uint16_t>::source_length_k = sizeof(source_k)/sizeof(source_k[0]) - 1;

const boost::uint16_t	test_traits<boost::uint16_t>::first_segment_k[] = { 'T', 'h', 'i', 's', ' ', 'i', 's', 0 };
const boost::uint16_t	test_traits<boost::uint16_t>::second_segment_k[] = { ' ', 'a', ' ', 't', 'e', 's', 't', 0 };
const boost::uint16_t	test_traits<boost::uint16_t>::full_string_k[] = { 'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 't', 'e', 's', 't', 0};
const boost::uint16_t	test_traits<boost::uint16_t>::short_full_string_k[] = { 'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 't', 'e', 0 };

int test_traits<boost::uint16_t>::compare(const boost::uint16_t* s1, const boost::uint16_t* s2)
{
	while (*s1 == *s2 && *s1 != 0 && *s2 != 0)
	{
		++s1;
		++s2;
	}
	
	return (*s1 == *s2 ? 0 :
				(*s1 < *s2 ? -1 : 1));
}

int test_traits<boost::uint16_t>::compare_n(const boost::uint16_t* s1,
											const boost::uint16_t* s2,
											std::size_t n)
{
	while (n != 0 && *s1 == *s2 && *s1 != 0 && *s2 != 0)
	{
		++s1;
		++s2;
		--n;
	}
	
	return (0 == n || *s1 == *s2 ? 0 :
				(*s1 < *s2 ? -1 : 1));
}

/*************************************************************************************************/

template <typename T>
void string_test_basic()
{
	typedef T							string_type;
	typedef typename T::value_type		value_type;
	typedef test_traits<value_type>		traits;

	assert(traits::source_length_k >= traits::short_length_k);
	
	string_type	s1;
	string_type	s2;
	
	BOOST_CHECK(s1 == s2);
	BOOST_CHECK(s1.empty());
	
	s1 = string_type(traits::source_k);
	BOOST_CHECK(!s1.empty());
	BOOST_CHECK_EQUAL(traits::source_length_k, s1.size());
	BOOST_CHECK_EQUAL(0, traits::compare(traits::source_k, s1.c_str()));
	BOOST_CHECK_EQUAL(static_cast<std::size_t>(std::distance(s1.begin(), s1.end())), traits::source_length_k);
	BOOST_CHECK_EQUAL(static_cast<std::size_t>(std::distance(s1.rbegin(), s1.rend())), traits::source_length_k);
	
	s1 = string_type(traits::source_k, traits::short_length_k);
	BOOST_CHECK(!s1.empty());
	BOOST_CHECK_EQUAL(traits::short_length_k, s1.size());
	BOOST_CHECK_EQUAL(0, traits::compare_n(traits::source_k, s1.c_str(), traits::short_length_k));
	
	s1 = string_type(traits::source_k, traits::source_k + traits::short_length_k);
	BOOST_CHECK(!s1.empty());
	BOOST_CHECK_EQUAL(traits::short_length_k, s1.size());
	BOOST_CHECK_EQUAL(0, traits::compare_n(traits::source_k, s1.c_str(), traits::short_length_k));
	
	s1 = string_type(traits::source_k);
	BOOST_CHECK(s2.empty());
	BOOST_CHECK(s1 != s2);
	s2.swap(s1);
	BOOST_CHECK(s1.empty());
	BOOST_CHECK(s1 != s2);
	BOOST_CHECK_EQUAL(traits::source_length_k, s2.size());
	BOOST_CHECK(0 != traits::compare(traits::source_k, s1.c_str()));
	BOOST_CHECK_EQUAL(0, traits::compare(traits::source_k, s2.c_str()));
}

/*************************************************************************************************/

template <typename T>
void string_test_reserve()
{
	typedef T								string_type;
	typedef typename T::value_type			value_type;
	typedef test_traits<value_type>			traits;
	typedef typename string_type::size_type	size_type;

	const size_type test_capacity(25);
	
	string_type s;
	
	s.reserve(0);
	BOOST_CHECK(s.capacity() >= 0);
	
	s.reserve(test_capacity);
	BOOST_CHECK(s.capacity() >= test_capacity);
}

/*************************************************************************************************/

template <typename T>
void string_test_clear()
{
	typedef T								string_type;
	typedef typename T::value_type			value_type;
	typedef test_traits<value_type>			traits;
	
	string_type s(traits::source_k);
	
	BOOST_CHECK(!s.empty());
	
	s.clear();
	BOOST_CHECK(s.empty());
}

/*************************************************************************************************/

template <typename T>
void string_test_push_back()
{
	typedef T								string_type;
	typedef typename T::value_type			value_type;
	typedef test_traits<value_type>			traits;
	typedef typename string_type::size_type	size_type;
	
	const unsigned int num_reps(100);
	
	string_type s;
	
	BOOST_CHECK_EQUAL(s.size(), size_type(0));
	
	for (unsigned int i = 0; i < num_reps; ++i)
	{
		s.push_back('a');
		
		const size_type expected_size(i + 1);
		BOOST_CHECK_EQUAL(s.size(), expected_size);
	}
}

/*************************************************************************************************/

template <typename T>
void string_test_append()
{
	typedef T								string_type;
	typedef typename T::value_type			value_type;
	typedef test_traits<value_type>			traits;
	
	string_type s;
	
	// test append member function
	
	s = string_type(traits::first_segment_k);
	s.append(string_type(traits::second_segment_k));
	BOOST_CHECK(s == string_type(traits::full_string_k));
	
	s = string_type(traits::first_segment_k);
	s.append(traits::second_segment_k);
	BOOST_CHECK(s == string_type(traits::full_string_k));
	
	s = string_type(traits::first_segment_k);
	s.append(traits::second_segment_k, traits::short_length_k);
	BOOST_CHECK(s == string_type(traits::short_full_string_k));
	
	// test operator+=
	
	s = string_type(traits::first_segment_k);
	s += string_type(traits::second_segment_k);
	BOOST_CHECK(s == string_type(traits::full_string_k));
	
	s = string_type(traits::first_segment_k);
	s += traits::second_segment_k;
	BOOST_CHECK(s == string_type(traits::full_string_k));
	
	// test operator+
	
	s = string_type(traits::first_segment_k) + string_type(traits::second_segment_k);
	BOOST_CHECK(s == string_type(traits::full_string_k));
	
	s = string_type(traits::first_segment_k) + traits::second_segment_k;
	BOOST_CHECK(s == string_type(traits::full_string_k));
}

/*************************************************************************************************/

template <typename T>
void string_test_move()
{
	typedef T										string_type;
	typedef typename T::value_type					value_type;
	typedef test_traits<value_type>					traits;
	typedef typename string_type::const_iterator	const_iterator;

	string_type source(traits::source_k);
	const string_type addend(source);
	
	// reserve extra space in the source string to prevent reallocation in += from causing
	// the test to fail
	source.reserve(source.size() + addend.size());
	
	const const_iterator  addr = source.begin();
	
	const string_type dest = adobe::move(source) + addend;
	
	BOOST_CHECK(addr == dest.begin());
}

/*************************************************************************************************/

BOOST_AUTO_TEST_CASE( string_basic )
{
	string_test_basic<adobe::string_t>();
	string_test_basic<adobe::string16_t>();

	// test additional functionality related to std::string
	
	typedef test_traits<char>	traits;
	typedef traits::string_type	string_type;

	string_type	s;
	
	const std::string source(traits::source_k);
	s = string_type(source);
	BOOST_CHECK(!s.empty());
	BOOST_CHECK_EQUAL(source.size(), s.size());
	BOOST_CHECK_EQUAL(0, traits::compare(source.c_str(), s.c_str()));
}

BOOST_AUTO_TEST_CASE( string_reserve )
{
	string_test_reserve<adobe::string_t>();
	string_test_reserve<adobe::string16_t>();
}

BOOST_AUTO_TEST_CASE( string_clear )
{
	string_test_clear<adobe::string_t>();
	string_test_clear<adobe::string16_t>();
}

BOOST_AUTO_TEST_CASE( string_push_back )
{
	string_test_push_back<adobe::string_t>();
	string_test_push_back<adobe::string16_t>();
}

BOOST_AUTO_TEST_CASE( string_append )
{
	string_test_append<adobe::string_t>();
	string_test_append<adobe::string16_t>();

	// test additional functionality related to std::string
	
	typedef test_traits<char>	traits;
	typedef traits::string_type	string_type;
	
	string_type s;
	
	// test assign member function
	
	s = string_type(traits::first_segment_k);
	s.append(std::string(traits::second_segment_k));
	BOOST_CHECK_EQUAL(0, traits::compare(s.c_str(), traits::full_string_k));

	// test operator+=
	
	s = string_type(traits::first_segment_k);
	s += std::string(traits::second_segment_k);
	BOOST_CHECK_EQUAL(0, traits::compare(s.c_str(), traits::full_string_k));
	
	// test operator+
	
	s = string_type(traits::first_segment_k) + std::string(traits::second_segment_k);
	BOOST_CHECK_EQUAL(0, traits::compare(s.c_str(), traits::full_string_k));
}

BOOST_AUTO_TEST_CASE( string_move )
{
	string_test_move<adobe::string_t>();
	string_test_move<adobe::string16_t>();
}

/*************************************************************************************************/

BOOST_AUTO_TEST_CASE( string_unicode )
{
	// This test cases demosntrates that unicode converstion both compiles and runs correctly
	// using only adobe::string_t and/or adobe::string16_t.
	//
	// The test relies on utf8_source_k and utf16_source_k being differently encoded
	// representations of the same data.
	
	const char				utf8_source_k[] = "this is a test";
	const boost::uint16_t	utf16_source_k[] = { 't', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 't', 'e', 's', 't', 0 };

	const adobe::string_t	utf8_exemplar(utf8_source_k);
	const adobe::string16_t	utf16_exemplar(utf16_source_k);

	adobe::string_t		utf8_test;
	adobe::string16_t	utf16_test;
	
	utf8_test.clear();
	BOOST_CHECK(utf8_test.empty());
	utf8_test.reserve(utf16_exemplar.size());
	adobe::to_utf8(utf16_exemplar.begin(), utf16_exemplar.end(), std::back_inserter(utf8_test));
	BOOST_CHECK(utf8_test == utf8_exemplar);

	utf8_test.clear();
	BOOST_CHECK(utf8_test.empty());
	utf8_test.reserve(utf8_exemplar.size());
	adobe::to_utf8(utf8_exemplar.begin(), utf8_exemplar.end(), std::back_inserter(utf8_test));
	BOOST_CHECK(utf8_test == utf8_exemplar);

	utf16_test.clear();
	BOOST_CHECK(utf16_test.empty());
	utf16_test.reserve(utf8_exemplar.size());
	adobe::to_utf16(utf8_exemplar.begin(), utf8_exemplar.end(), std::back_inserter(utf16_test));
	BOOST_CHECK(utf16_test == utf16_exemplar);

	utf16_test.clear();
	BOOST_CHECK(utf16_test.empty());
	utf16_test.reserve(utf16_exemplar.size());
	adobe::to_utf16(utf16_exemplar.begin(), utf16_exemplar.end(), std::back_inserter(utf16_test));
	BOOST_CHECK(utf16_test == utf16_exemplar);
}

/*************************************************************************************************/

