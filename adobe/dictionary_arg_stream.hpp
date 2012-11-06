/*
    Copyright 2005-2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

#ifndef ADOBE_DICTIONARY_ARG_STREAM_HPP
#define ADOBE_DICTIONARY_ARG_STREAM_HPP

#include <adobe/arg_stream.hpp>

#include <adobe/is_range.hpp>
#include <adobe/dictionary.hpp>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/value_type.hpp>
#include <boost/array.hpp>
#include <boost/utility/value_init.hpp>

#include <new> // std::nothrow_t

namespace adobe
{


/*!
\ingroup arg_stream
\ingroup dictionary_arg_stream

\brief dictionary_arg_stream requires specializations of get_dictionary_entry for the dictionary.  For example, the adobe::dictionary_t specializations.

\par we use 'get_dictionary_entry' instead of the standard adobe::get_value because
\code
	get_value(dict, key).cast<R>();
\endcode
	Seems a bit too specific of an interface to ask someone to implement
*/
template <typename R>
R get_dictionary_entry(adobe::dictionary_t const & dict, adobe::name_t const & key)
{
	// throws bad_cast if result is not castable to R
	return get_value(dict, key).cast<R>();
}
template <typename R>
bool get_dictionary_entry(adobe::dictionary_t const & dict, adobe::name_t const & key, R & r)
{
	// returns false (and leaves r alone) if key not in dictionary
	return get_value(dict, key, r);
}


template <typename Dictionary, typename InputRange /*to get the keys*/, typename NoThrow = void /* or std::nothrow_t */>
struct dictionary_arg_stream_base
{
	typedef Dictionary dictionary_type;
	typedef InputRange range_type;
	typedef typename boost::range_iterator<InputRange const>::type iterator;

	dictionary_type dict;
	range_type range;
	iterator curr;

	dictionary_arg_stream_base(dictionary_type & d, range_type const & key_range)
		: dict(d), range(key_range), curr(boost::begin(key_range))
	{
	}

	bool eof() const
	{
		return curr == boost::end(range);
	}

	void throw_if_eof() const
	{
		if (eof())
		{
			throw arg_stream::no_more_args();
		}
	}

	void reset()
	{
		curr = boost::begin(range);
	}
};

/*!
\ingroup arg_stream

\brief dictionary_arg_stream implements the arg_stream interface

\par This works with any dictionary type that implements \c get_dictionary_entry<R>(dict, key)

\template_parameters
    - \c Dictionary - A dictionary which implements adobe::get_entry
	- \c Inputrange - models \ref stldoc_InputRange
	- \c NoThrow - if \ref std::nothrow_t, then the arg_stream will NOT throw if the key is not in the dictionary (it will instead return a default initialized argument)
	             - if it is NOT std::nothrow_t, it will likely throw if the key is not in the dictionary
*/
template <typename Dictionary, typename InputRange /*to get the keys*/, typename NoThrow = void /* or std::nothrow_t */>
struct dictionary_arg_stream : dictionary_arg_stream_base<Dictionary, InputRange>
{
// we use this-> because otherwise the compiler doesn't actually know that it is in the base class
// (becaue the base is a template, so if specialized, it might not be there!
	typedef dictionary_arg_stream_base<Dictionary, InputRange> _inherited;
	using _inherited::dictionary_type;
	using _inherited::range_type;
	using _inherited::throw_if_eof;
	using _inherited::dict;
	using _inherited::curr;

	/*!
		d - dictionary from with to retrieve arguments
		key_range define a range of keys
		each key (in order) is used to retrieve an argument of the arg_stream
	*/
	dictionary_arg_stream(Dictionary & d, InputRange const & key_range)
		: dictionary_arg_stream_base<Dictionary, InputRange>(d, key_range)
	{
	}

	template <typename R>
	R get_next_arg()
	{
		throw_if_eof();

		// can throw if the result of get_value is not castable to R
		return get_dictionary_entry<R>(dict, *curr++);
	}
};
template <typename Dictionary, typename InputRange /*to get the keys*/>
struct dictionary_arg_stream<Dictionary, InputRange, std::nothrow_t>
	: dictionary_arg_stream_base<Dictionary, InputRange>
{
	dictionary_arg_stream(Dictionary & d, InputRange const & key_range)
		: dictionary_arg_stream_base<Dictionary, InputRange>(d, key_range)
	{
	}

	template <typename R>
	R get_next_arg()
	{
		this->throw_if_eof();

		R r = boost::initialized_value;
		get_dictionary_entry(this->dict, *this->curr++, r);
		return r;
	}
};


template <typename Dictionary, typename InputRange /*to get the keys*/>
dictionary_arg_stream<Dictionary, InputRange, void>
	make_dictionary_arg_stream(
		Dictionary & dict, InputRange const & key_range)
{
	return dictionary_arg_stream<Dictionary, InputRange, void>(dict, key_range);
}

template <typename Dictionary, typename InputRange /*to get the keys*/>
dictionary_arg_stream<Dictionary, InputRange, std::nothrow_t>
	make_dictionary_arg_stream(
		Dictionary & dict, InputRange const & key_range, std::nothrow_t)
{
	return dictionary_arg_stream<Dictionary, InputRange, std::nothrow_t>(dict, key_range);
}


namespace dictionary_arg_stream_detail
{
	template<size_t>
	struct member_test_helper
	{
	};

	struct function_not_present {};
	template <typename R>
	function_not_present get_dictionary_entry(...);

	template <typename Dictionary, typename Key>
	struct has_get_dictionary_entry_template
	{
		static Dictionary * dict;
		static Key * key;
		static adobe::detail::yes_struct SFINAE(int);
		static adobe::detail::no_struct SFINAE(function_not_present const &);
		static const bool value = sizeof(SFINAE(get_dictionary_entry<int>(*dict, *key))) == sizeof(adobe::detail::yes_struct);
	};
	template <typename Dictionary, typename Key, typename R>
	struct has_get_dictionary_entry_function
	{
		static Dictionary * dict;
		static Key * key;
		static R * r;
		static adobe::detail::yes_struct SFINAE(int);
		static adobe::detail::no_struct SFINAE(function_not_present const &);
		static const bool value = sizeof(SFINAE(get_dictionary_entry(*dict, *key, *r))) == sizeof(adobe::detail::yes_struct);
	};

	template <typename Dictionary, typename Range>
	struct is_range_for_dictionary
	{
		static const bool value = has_get_dictionary_entry_template<Dictionary, typename boost::range_value<Range>::type>::value;
	};

	static const int test = is_range_for_dictionary<adobe::dictionary_t, boost::array<adobe::name_t, 3> >::value;

	ADOBE_HAS_TYPE_IMPL(key_type);

	template <typename Dictionary, typename DefaultKey, bool has_type = ADOBE_HAS_TYPE(Dictionary, key_type)>
	struct key_type;

	template <typename Dictionary, typename DefaultKey>
	struct key_type<Dictionary, DefaultKey, false>
	{
		typedef DefaultKey type;
	};
	template <typename Dictionary, typename DefaultKey>
	struct key_type<Dictionary, DefaultKey, true>
	{
		typedef typename Dictionary::key_type type;
	};


	// for the single arg case (single arg after dict arg)
	// is that arg a key to the dictionary representing an arg,
	// or is it a range of keys for the dictionary
	template <typename NoThrow = std::nothrow_t, bool isRange = true>
	struct if_range_base
	{
		template <typename F, typename Dictionary, typename InputRange>
			static typename arg_stream::result_type<F>::type
				call(F f, Dictionary const & dict, InputRange const & range)
		{
			return arg_stream::call(f, make_dictionary_arg_stream(dict, range, std::nothrow));
		}
	};
	template <>
	struct if_range_base<void, true>
	{
		template <typename F, typename Dictionary, typename InputRange>
			static typename arg_stream::result_type<F>::type
				call(F f, Dictionary const & dict, InputRange const & range)
		{
			return arg_stream::call(f, make_dictionary_arg_stream(dict, range));
		}
	};

	template <>
	struct if_range_base<std::nothrow_t, false>
	{
		template <typename F, typename Dictionary, typename Key>
			static typename arg_stream::result_type<F>::type
				call(F f, Dictionary const & dict, Key const & keyable)
		{
			typedef typename key_type<Dictionary, Key>::type key_type;
			key_type key(keyable);
			std::pair<key_type const *, key_type const *> key_range(&key, &key + 1);
			return arg_stream::call(f, make_dictionary_arg_stream(dict, key_range, std::nothrow));
		}
	};
	template <>
	struct if_range_base<void, false>
	{
		template <typename F, typename Dictionary, typename Key>
			static typename arg_stream::result_type<F>::type
				call(F f, Dictionary const & dict, Key const & keyable)
		{
			typedef typename key_type<Dictionary, Key>::type key_type;
			key_type key(keyable);
			std::pair<key_type const *, key_type const *> key_range(&key, &key + 1);
			return arg_stream::call(f, make_dictionary_arg_stream(dict, key_range));
		}
	};

	template <typename NoThrow, typename Dictionary, typename T>
	struct if_range_else_key
		: if_range_base<NoThrow, is_range_for_dictionary<Dictionary, T>::value>
	{
	};
}

/*!
\ingroup arg_stream
\ingroup dictionary_arg_stream

\brief call the function/callable-object \c f with args pulled from dictionary \c dict via keys from key_range 
*/
template <typename F, typename Dictionary, typename SingleArg>
	typename arg_stream::result_type<F>::type
		call_with_dictionary(F f, Dictionary const & dict, SingleArg const & key_or_key_range)
{
	return dictionary_arg_stream_detail::if_range_else_key<void, Dictionary, SingleArg>::call(f, dict, key_or_key_range);
}

// nothrow case:
template <typename F, typename Dictionary, typename SingleArg>
	typename arg_stream::result_type<F>::type
		call_with_dictionary(F f, std::nothrow_t, Dictionary const & dict, SingleArg const & key_or_key_range)
{
	return dictionary_arg_stream_detail::if_range_else_key<std::nothrow_t, Dictionary, SingleArg>::call(f, dict, key_or_key_range);
}

// member function cases
template <class T, typename F, typename Dictionary, typename SingleArg>
	typename arg_stream::result_type<F>::type
		call_member_with_dictionary(T * that, F f, Dictionary const & dict, SingleArg const & key_or_key_range)
{
	return dictionary_arg_stream_detail::if_range_else_key<void, Dictionary, SingleArg>::call(that, f, dict, key_or_key_range);
}

template <class T, typename F, typename Dictionary, typename SingleArg>
	typename arg_stream::result_type<F>::type
		call_member_with_dictionary(T* that, F f, std::nothrow_t, Dictionary const & dict, SingleArg const & key_or_key_range)
{
	return dictionary_arg_stream_detail::if_range_else_key<std::nothrow_t, Dictionary, SingleArg>::call(that, f, dict, key_or_key_range);
}


/////// 2 params
template <typename F, typename Dictionary, typename T1, typename T2 >
	typename arg_stream::result_type<F>::type 
		call_with_dictionary(F f, Dictionary const & dict,
			T1 const & key1, T2 const & key2)
{
	typedef typename dictionary_arg_stream_detail::key_type<Dictionary, T1>::type key_type;
	boost::array<key_type, 2> arr = { key_type(key1), key_type(key2) };
	return call_with_dictionary(f, dict, arr);
}

template <typename X, typename F, typename Dictionary, typename T1, typename T2 >
	typename arg_stream::result_type<F>::type 
		call_member_with_dictionary(X & x, F f, Dictionary const & dict,
			T1 const & key1, T2 const & key2)
{
	typedef typename dictionary_arg_stream_detail::key_type<Dictionary, T1>::type key_type;
	boost::array<key_type, 2> arr = { key_type(key1), key_type(key2) };
	return call_member_with_dictionary(x, f, dict, arr);
}

template <typename F, typename Dictionary, typename T1, typename T2 >
	typename arg_stream::result_type<F>::type 
		call_with_dictionary(F f, std::nothrow_t, Dictionary const & dict,
			T1 const & key1, T2 const & key2)
{
	typedef typename dictionary_arg_stream_detail::key_type<Dictionary, T1>::type key_type;
	boost::array<key_type, 2> arr = { key_type(key1), key_type(key2) };
	return call_with_dictionary(f, std::nothrow, dict, arr);
}

template <typename X, typename F, typename Dictionary, typename T1, typename T2 >
	typename arg_stream::result_type<F>::type 
		call_member_with_dictionary(X x, F f, std::nothrow_t, Dictionary const & dict,
			T1 const & key1, T2 const & key2)
{
	typedef typename dictionary_arg_stream_detail::key_type<Dictionary, T1>::type key_type;
	boost::array<key_type, 2> arr = { key_type(key1), key_type(key2) };
	return call_member_with_dictionary(x, f, std::nothrow, dict, arr);
}

/////// 3 params
template <typename F, typename Dictionary, typename T1, typename T2, typename T3>
	typename arg_stream::result_type<F>::type 
		call_with_dictionary(F f, Dictionary const & dict,
			T1 const & key1, T2 const & key2, T3 const & key3)
{
	typedef typename dictionary_arg_stream_detail::key_type<Dictionary, T1>::type key_type;
	boost::array<key_type, 3> arr = { key_type(key1), key_type(key2), key_type(key3) };
	return call_with_dictionary(f, dict, arr);
}

template <typename X, typename F, typename Dictionary, typename T1, typename T2, typename T3>
	typename arg_stream::result_type<F>::type 
		call_member_with_dictionary(X & x, F f, Dictionary const & dict,
			T1 const & key1, T2 const & key2, T3 const & key3)
{
	typedef typename dictionary_arg_stream_detail::key_type<Dictionary, T1>::type key_type;
	boost::array<key_type, 3> arr = { key_type(key1), key_type(key2), key_type(key3) };
	return call_member_with_dictionary(x, f, dict, arr);
}

template <typename F, typename Dictionary, typename T1, typename T2, typename T3>
	typename arg_stream::result_type<F>::type 
		call_with_dictionary(F f, std::nothrow_t, Dictionary const & dict,
			T1 const & key1, T2 const & key2, T3 const & key3)
{
	typedef typename dictionary_arg_stream_detail::key_type<Dictionary, T1>::type key_type;
	boost::array<key_type, 3> arr = { key_type(key1), key_type(key2), key_type(key3) };
	return call_with_dictionary(f, std::nothrow, dict, arr);
}

template <typename X, typename F, typename Dictionary, typename T1, typename T2, typename T3>
	typename arg_stream::result_type<F>::type 
		call_member_with_dictionary(X x, F f, std::nothrow_t, Dictionary const & dict,
			T1 const & key1, T2 const & key2, T3 const & key3)
{
	typedef typename dictionary_arg_stream_detail::key_type<Dictionary, T1>::type key_type;
	boost::array<key_type, 3> arr = { key_type(key1), key_type(key2), key_type(key3) };
	return call_member_with_dictionary(x, f, std::nothrow, dict, arr);
}

/////// 4 params
template <typename F, typename Dictionary, typename T1, typename T2, typename T3, typename T4>
	typename arg_stream::result_type<F>::type 
		call_with_dictionary(F f, Dictionary const & dict,
			T1 const & key1, T2 const & key2, T3 const & key3, T4 const & key4)
{
	typedef typename dictionary_arg_stream_detail::key_type<Dictionary, T1>::type key_type;
	boost::array<key_type, 4> arr = { key_type(key1), key_type(key2), key_type(key3), key_type(key4) };
	return call_with_dictionary(f, dict, arr);
}

template <typename X, typename F, typename Dictionary, typename T1, typename T2, typename T3, typename T4>
	typename arg_stream::result_type<F>::type 
		call_member_with_dictionary(X & x, F f, Dictionary const & dict,
			T1 const & key1, T2 const & key2, T3 const & key3, T4 const & key4)
{
	typedef typename dictionary_arg_stream_detail::key_type<Dictionary, T1>::type key_type;
	boost::array<key_type, 4> arr = { key_type(key1), key_type(key2), key_type(key3), key_type(key4) };
	return call_member_with_dictionary(x, f, dict, arr);
}

template <typename F, typename Dictionary, typename T1, typename T2, typename T3, typename T4>
	typename arg_stream::result_type<F>::type 
		call_with_dictionary(F f, std::nothrow_t, Dictionary const & dict,
			T1 const & key1, T2 const & key2, T3 const & key3, T4 const & key4)
{
	typedef typename dictionary_arg_stream_detail::key_type<Dictionary, T1>::type key_type;
	boost::array<key_type, 4> arr = { key_type(key1), key_type(key2), key_type(key3), key_type(key4) };
	return call_with_dictionary(f, std::nothrow, dict, arr);
}

template <typename X, typename F, typename Dictionary, typename T1, typename T2, typename T3, typename T4>
	typename arg_stream::result_type<F>::type 
		call_member_with_dictionary(X x, F f, std::nothrow_t, Dictionary const & dict,
			T1 const & key1, T2 const & key2, T3 const & key3, T4 const & key4)
{
	typedef typename dictionary_arg_stream_detail::key_type<Dictionary, T1>::type key_type;
	boost::array<key_type, 4> arr = { key_type(key1), key_type(key2), key_type(key3), key_type(key4) };
	return call_member_with_dictionary(x, f, std::nothrow, dict, arr);
}


} // namespace adobe

#endif // include guard

