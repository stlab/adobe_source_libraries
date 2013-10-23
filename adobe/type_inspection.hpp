/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/


#ifndef ADOBE_TYPE_INSPECTION_HPP
#define ADOBE_TYPE_INSPECTION_HPP



namespace adobe
{


namespace detail
{
	struct yes_struct {char a;};
	struct no_struct  { yes_struct a[2];};
}

/*!
\ingroup inspection

\brief Implementation part of \c ADOBE_HAS_TYPE macro.   Required before using ADOBE_HAS_TYPE.

\par
Unfortunately C++ doesn't have full inspection (yet) so we need to use 2 macros to implement ADOBE_HAS_TYPE.
This macro sets up the implementation for the other.
You need to use this macro for each type/typedef that you want to inspect for.
Internally it defines a template class, thus it must be used only at global, namespace, or class scope.
See ADOBE_HAS_TYPE.
*/
#define ADOBE_HAS_TYPE_IMPL(TypeInQuestion) \
template< \
	typename C##TypeInQuestion> \
struct has_type##TypeInQuestion \
{ \
	template <typename T##TypeInQuestion> \
	static adobe::detail::yes_struct SFINAE(typename T##TypeInQuestion::TypeInQuestion*); \
	template <typename> \
	static adobe::detail::no_struct SFINAE(...); \
	static const bool value = sizeof(SFINAE<C##TypeInQuestion>(0)) == sizeof(adobe::detail::yes_struct); \
}

/*!
\ingroup inspection

\brief returns true iff \c C has an internal type named 'TypeInQuestion'.  ie returns true iff C::TypeInQuestion is a type.

\par
Unfortunately C++ doesn't have full inspection (yet) so we need to use macros to implement ADOBE_HAS_TYPE.
In order to use this macro, you also need to declare its implementation (for each TypeInQuestion) using ADOBE_HAS_TYPE_IMPL.
eg: \code
	ADOBE_HAS_TYPE_IMPL(value_type);

	assert(ADOBE_HAS_TYPE(std::string<char>, value_type));
	assert(!ADOBE_HAS_TYPE(int, value_type));
\endcode
Also, make sure the 2 macros are in the same namespace!
*/
#define ADOBE_HAS_TYPE(C, TypeInQuestion) \
has_type##TypeInQuestion<C>::value


/*!
\ingroup inspection

\brief is T::type a valid type (or a compile error?)
*/
// one of the most important types to look for is 'type', so let's just implement that one here
template <typename T>
struct has_type_type
{
	ADOBE_HAS_TYPE_IMPL(type);

	static const bool value = ADOBE_HAS_TYPE(T, type);
};

template <typename T, typename Default>
struct type_or_default
{
	template <bool condition, typename IFtype, typename ELSEtype>
	struct if_has_type
	{
		typedef typename IFtype::type type;
	};
	template <typename IFtype, typename ELSEtype>
	struct if_has_type<false, IFtype, ELSEtype>
	{
		typedef ELSEtype type;
	};
	typedef typename if_has_type<has_type_type<T>::value, T, Default>::type type;
};


/*!
\ingroup inspection

\brief Implementation part of \c ADOBE_HAS_MEMBER macro.  Required before using ADOBE_HAS_MEMBER.

\par
Unfortunately C++ doesn't have full inspection (yet) so we need to use 2 macros to implement ADOBE_HAS_MEMBER.
This macro sets up the implementation for the other.
You need to use this macro for each member that you want to inspect for.
Internally it defines a template class, thus it must be used only at global, namespace, or class scope.
See ADOBE_HAS_MEMBER
*/
/**
**	Visual Studio 8.0 and lower can't handle the template version
**  not sure about 9.0 (yet)
*/
#if _MSC_VER <= 1400
#define ADOBE_HAS_MEMBER_IMPL(Member) \
template <class Class> \
struct has_member##Member \
{ \
	__if_exists(Class::Member) \
	{ \
		static const bool value = true; \
	} \
	__if_not_exists(Class::Member) \
	{ \
		static const bool value = false; \
	} \
}
#else

namespace detail
{
	template<size_t>
	struct member_test_helper
	{
	};
}

#define ADOBE_HAS_MEMBER_IMPL(MemberInQuestion) \
template <class Class> \
struct has_member##MemberInQuestion \
{ \
	template <class T##MemberInQuestion> \
	static adobe::detail::yes_struct SFINAE( adobe::detail::member_test_helper<sizeof(&T##MemberInQuestion::MemberInQuestion)> * ); \
	template<class> \
	static adobe::detail::no_struct SFINAE(...); \
	static const bool value = sizeof(SFINAE<Class>(0)) == sizeof(adobe::detail::yes_struct); \
}
#endif

/*!
\ingroup inspection

\brief returns true iff \c C has an internal member named 'MemberInQuestion'.

\par
Unfortunately C++ doesn't have full inspection (yet) so we need to use macros to implement ADOBE_HAS_MEMBER.
In order to use this macro, you also need to declare its implementation (for each MemberInQuestion) using ADOBE_HAS_MEMBER_IMPL.
eg: \code
	ADOBE_HAS_MEMBER_IMPL(first);  // defines a template, thus must be outside of any function, may be in a class

	assert(ADOBE_HAS_MEMBER(std::pair<char, int>, first));
	assert(!ADOBE_HAS_MEMBER(int, first));
\endcode
Also, make sure the 2 macros are in the same namespace!
*/
#define ADOBE_HAS_MEMBER(C, MemberInQuestion) \
has_member##MemberInQuestion<C>::value


/*!
\ingroup inspection

\brief Implementation part of \c ADOBE_HAS_TEMPLATE1 macro.   Required before using ADOBE_HAS_TEMPLATE1.

\par
Unfortunately C++ doesn't have full inspection (yet) so we need to use 2 macros to implement ADOBE_HAS_TEMPLATE1.
This macro sets up the implementation for the other.
You need to use this macro for each internal template that you want to inspect for.
Internally it defines a template class, thus it must be used only at global, namespace, or class scope.
See ADOBE_HAS_TEMPLATE1.
*/
#define ADOBE_HAS_TEMPLATE1_IMPL(TemplateInQuestion) \
template< \
	typename C##TemplateInQuestion> \
struct has_template1##TemplateInQuestion \
{ \
	template <typename T##TemplateInQuestion> \
	static adobe::detail::yes_struct SFINAE(typename T##TemplateInQuestion::template TemplateInQuestion<int>*); \
	template <typename> \
	static adobe::detail::no_struct SFINAE(...); \
	static const bool value = sizeof(SFINAE<C##TemplateInQuestion>(0)) == sizeof(adobe::detail::yes_struct); \
}

/*!
\ingroup inspection

\brief returns true iff \c C has an internal template named 'TemplateInQuestion' with 1 (nondefault) template param.  ie returns true iff "C::template TemplateInQuestion<someType>" is a syntactically correct.

\par
Unfortunately C++ doesn't have full inspection (yet) so we need to use macros to implement ADOBE_HAS_TEMPLATE1.
In order to use this macro, you also need to declare its implementation (for each TemplateInQuestion) using ADOBE_HAS_TEMPLATE1_IMPL.
eg: \code
	ADOBE_HAS_TEMPLATE_IMPL(value_type);

	assert(ADOBE_HAS_TEMPLATE1(std::string<char>, value_type));
	assert(!ADOBE_HAS_TEMPLATE1(int, value_type));
\endcode
Note, this only works for templates over types, not templates over constant values.  ie
	template mytemplate<typename T> { ... }
not
	template mytemplate<bool test> { ... }
we could make even more macros for the constant value case(s), and all the permutations... if need be
Also, make sure the 2 macros are in the same namespace!
*/
#define ADOBE_HAS_TEMPLATE1(C, TemplateInQuestion) \
has_template1##TemplateInQuestion<C>::value


////////////////////////
// 2 Arg case
//
#define ADOBE_HAS_TEMPLATE2_IMPL(TemplateInQuestion) \
template< \
	typename C##TemplateInQuestion> \
struct has_template2##TemplateInQuestion \
{ \
	template <typename T##TemplateInQuestion> \
	static adobe::detail::yes_struct SFINAE(typename T##TemplateInQuestion::template TemplateInQuestion<int, int>*); \
	template <typename> \
	static adobe::detail::no_struct SFINAE(...); \
	static const bool value = sizeof(SFINAE<C##TemplateInQuestion>(0)) == sizeof(adobe::detail::yes_struct); \
}

#define ADOBE_HAS_TEMPLATE2(C, TemplateInQuestion) \
has_template2##TemplateInQuestion<C>::value

#define ADOBE_HAS_TEMPLATE3_IMPL(TemplateInQuestion) \
template< \
	typename C##TemplateInQuestion> \
struct has_template3##TemplateInQuestion \
{ \
	template <typename T##TemplateInQuestion> \
	static adobe::detail::yes_struct SFINAE(typename T##TemplateInQuestion::template TemplateInQuestion<int, int, int>*); \
	template <typename> \
	static adobe::detail::no_struct SFINAE(...); \
	static const bool value = sizeof(SFINAE<C##TemplateInQuestion>(0)) == sizeof(adobe::detail::yes_struct); \
}

#define ADOBE_HAS_TEMPLATE3(C, TemplateInQuestion) \
has_template3##TemplateInQuestion<C>::value


} // namespace

#endif // include guard

