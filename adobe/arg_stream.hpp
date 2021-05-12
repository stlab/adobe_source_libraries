/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef ADOBE_ARG_STREAM_H
#define ADOBE_ARG_STREAM_H

#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/result_type.hpp>

#include <boost/mpl/begin.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/end.hpp>
#include <boost/mpl/next.hpp>

// Apple sucks
#ifdef nil
#undef nil
#endif

#include <boost/fusion/include/cons.hpp>
#include <boost/fusion/include/invoke.hpp>
#include <boost/fusion/include/push_back.hpp>

#include <boost/type_traits/add_pointer.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <boost/utility/enable_if.hpp>

#include <adobe/type_inspection.hpp> // ADOBE_HAS_TYPE/ADOBE_HAS_MEMBER


// forward declare boost::function so we can specialize against it
namespace boost {
template <typename F>
class function;
}


namespace adobe {

/*!
\defgroup arg_stream arg_stream

\c arg_stream is a namespace wrapping together the concept of a stream from which you can retrieve
items (ie function arguments) of various types.

An arg_stream is any object which implements a templatized member function
\code
    template <typename R> R get_next_arg();
\endcode

and (optionally) implements
\code
    bool eof();
\endcode

arg_stream implementors can communicate that they have an eof function by specializing \c
arg_stream::traits<T>::has_eof_memberfunction.
\code
namespace adobe { namespace arg_stream {
    template <typename>
    struct traits<my_arg_stream_goes_here>
    {
        static const bool has_eof_memberfunction = true;
    };
} }
\endcode

Alternatively, you may rely on the default implementation of traits<T>, which uses compile time type
inspection to determine if T has an eof function.

The point of arg_stream is the \c arg_stream::call() function:
\code
    template<typename F, typename ArgStream>
    result_type<F>::type arg_stream::call(F f, ArgStream argstream);
\endcode

*/

namespace arg_stream {


/*!
\ingroup arg_stream
    arg_stream::no_more_args or a derivative thereof should be thrown by an arg_stream if
get_next_arg<>() is called when there are no more args available.
*/
struct no_more_args : std::exception {};


namespace detail {
ADOBE_HAS_TYPE_IMPL(eof);

template <typename T>
struct has_eof_member {
    static const bool value = ADOBE_HAS_TYPE(T, eof);
};
} // namespace detail

/*!
\ingroup arg_stream

\brief defines any traits that help with the implementation of arg_stream::call() and/or helper
objects like arg_stream::chain.

\par Currently only communicates if the type T has a member function eof() (which returns
whether
there are any arguements left in the stream).
This may be specialized by various arg_streams.
*/
template <typename T>
struct traits {
    static const bool has_eof_member = detail::has_eof_member<T>::value;
};


namespace detail {
template <class ArgStream>
static bool
eof_check(ArgStream& as,
          typename boost::enable_if_c<traits<ArgStream>::has_eof_memberfunction>::type* dummy = 0) {
    return as.eof();
}

template <class ArgStream>
static bool eof_check(
    ArgStream& as,
    typename boost::disable_if_c<traits<ArgStream>::has_eof_memberfunction>::type* dummy = 0) {
    return false;
}

template <class ArgStream>
static bool eof_check(ArgStream* as) {
    return as ? eof_check(*as) : true;
}
} // namespace detail

/*!
\ingroup arg_stream

\brief \c arg_stream::eof(argstream) returns true if there are no more args available.

\par
    This function is available for specialization by arg_streams.
    If not specialized, the default implementation attempts to call the arg_stream's member
function
eof() if one is available.
    If there is no member function, it returns false - ie if it is not known, then it is assumed
that more args exist, and
    thus we must still always consider that the arg_stream::no_more_args exception may be
thrown.
*/
template <typename ArgStream>
bool eof(ArgStream const& as) {
    return detail::eof_check(as);
}

/*!
\ingroup arg_stream

\brief \c arg_stream::get_next_arg<T>(argstream) returns the next arg as a T

\par
    This function is what defines something as an arg_stream.  An arg_stream must either:
    1. implement a templatized member function get_next_arg<T>() or
    2. specialize arg_stream::get_next_arg<T>(YourArgStream & argstream)
    ie If not specialized, this default implementation attempts to call the arg_stream's member
function get_next_arg<T>().
    If there is no member function, and the global function is not specialized, it will not
compile.
*/
template <typename R, typename ArgStream>
R get_next_arg(ArgStream const& as) {
    return as.get_next_arg<R>();
}
// specialize these or let them fallback to the above specialization
template <typename R, typename ArgStream>
R get_next_arg(ArgStream& as) {
    return as.get_next_arg<R>();
}
template <typename R, typename ArgStream>
R get_next_arg(ArgStream* as) {
    return get_next_arg<R>(*as);
}
template <typename R, typename ArgStream>
R get_next_arg(ArgStream const* as) {
    return get_next_arg<R>(*as);
}


/*!
\ingroup arg_stream

\brief returns the function signature of the callable object type F

\template_parameters F must model callable_object(?)
*/

// for some reason boost::function_types does not handle boost::functions,
// nor does boost::function have a function::signature typedef,
// so in order to support boost, we use this signature<F>::type mechanism:
template <typename F>
struct signature {
    typedef F type;
};
// specialize for boost::function
template <typename F>
struct signature<boost::function<F>> {
    typedef F type;
};

/*!
\ingroup arg_stream

\brief result_type<F>::type is the return type of the function f.

\template_parameters
    - \c F models \ref callable_object
*/
template <typename F>
struct result_type {
    typedef typename boost::function_types::result_type<typename signature<F>::type>::type type;
};

namespace detail {
// how it all works...


template <typename T>
struct remove_cv_ref : boost::remove_cv<typename boost::remove_reference<T>::type> {};


// see also boost::function_types 'interpreter' example
// we convert the function signature into a mpl sequence (it *is* an mpl sequence, since it
// implements mpl::begin/end/etc)
// we recursively push the args onto a fusion sequence, calling get_next_arg as we go.
// the recursion ends when we get to the end of the mpl-sequence-function-sigature
// (see the specialized case where From == To)
// and then luckily fusion has a magic invoke function we can use
template <typename F,
          class From = typename boost::mpl::begin<
              boost::function_types::parameter_types<typename signature<F>::type>>::type,
          class To = typename boost::mpl::end<
              boost::function_types::parameter_types<typename signature<F>::type>>::type>
struct invoker {
    // add an argument to a Fusion cons-list for each parameter type
    template <typename Args, typename ArgStream>
    static inline typename result_type<F>::type apply(F func, ArgStream& astream,
                                                      Args const& args) {
        typedef typename remove_cv_ref<typename boost::mpl::deref<From>::type>::type arg_type;
        typedef typename boost::mpl::next<From>::type next_iter_type;

        return invoker<F, next_iter_type, To>::apply(
            func, astream, boost::fusion::push_back(args, get_next_arg<arg_type>(astream)));
    }
};

// specialize final case
template <typename F, class To>
struct invoker<F, To, To> {
    template <typename Args, typename ArgStream>
    static inline typename result_type<F>::type apply(F func, ArgStream&, Args const& args) {
        return boost::fusion::invoke(func, args);
    }
};

} // namespace detail


/*!
\ingroup arg_stream

\brief Calls function/callable-object f with function arguments supplied by the arg_stream.

\par This is the point of arg_stream.  To call a function with typed-args pulled out of some
abstracted object.
*/
template <typename F, typename ArgStream>
typename result_type<F>::type call(F f, ArgStream& astream) {
    return detail::invoker<F>::template apply(f, astream, boost::fusion::nil());
}

/*!
\ingroup arg_stream

\brief specialization of arg_stream::call for handling member function calls.
*/
template <class T, typename F, typename ArgStream>
typename result_type<F>::type call(T* that, F f, ArgStream& astream) {
    // object gets pushed on as first arg of fusion list,
    // and remove first arg from signature (the object that the member function belongs to)
    // using
    // mpl::next
    boost::fusion::nil args;
    return detail::invoker<
        F,
        typename boost::mpl::next<typename boost::mpl::begin<boost::function_types::parameter_types<
            typename signature<F>::type, boost::add_pointer<boost::mpl::placeholders::_>>>::type>::
            type,
        typename boost::mpl::end<boost::function_types::parameter_types<
            typename signature<F>::type, boost::add_pointer<boost::mpl::placeholders::_>>>::type>::
        template apply(f, astream, boost::fusion::push_back(args, that));
}


/*!
\ingroup arg_stream

\brief chain 2 arg_streams together by calling the first stream until depleted, then calling the
second.

\par note that with repeated application you can chain any nunber of arg_streams together.
(chain<AS1, chain<AS2, chain<AS3, AS4> > >, etc)
*/
template <typename ArgStreamFirst, typename ArgStreamSecond>
struct chain {
    template <class ArgStream>
    bool eof(ArgStream* as) {
        return detail::eof_check(as);
    }

    typedef ArgStreamFirst first_type;
    typedef ArgStreamSecond second_type;
    ArgStreamFirst* first;
    ArgStreamSecond* second;

    chain(ArgStreamFirst& first_stream, ArgStreamSecond& second_stream)
        : first(&first_stream), second(&second_stream) {}

    template <typename T>
    T get_next_arg() {
        if (!eof(first)) {
            try {
                return first->get_next_arg<T>();
            } catch (arg_stream::no_more_args&) {
                first = 0;
            }
        }

        return second->get_next_arg<T>();
    }

    bool eof() const { return eof(first) && eof(second); }
};

template <typename S1, typename S2>
struct traits<chain<S1, S2>> {
    static const bool has_eof_memberfunction = true;
};

/*!
\brief given 2 arg_streams, returns an arg_stream of the 2 streams chained together
*/
template <typename ArgStreamFirst, typename ArgStreamSecond>
chain<ArgStreamFirst, ArgStreamSecond> make_chain(ArgStreamFirst& first_stream,
                                                  ArgStreamSecond& second_stream) {
    return chain<ArgStreamFirst, ArgStreamSecond>(first_stream, second_stream);
}

/*!
\brief the empty-set arg stream has no arguments.  Not sure what this might be useful for.
*/
struct nonarg {
    bool eof() { return true; }

    template <typename R>
    R get_next_arg() {
        throw arg_stream::no_more_args();

        return *(R*)32; // some compilers need a return; here's a bad one (but that doesn't require
                        // default construction)
    }
};

template <>
struct traits<nonarg> {
    static const bool has_eof_memberfunction = true;
};

/*!
\brief holds a single value, and returns it as an arg n (default 1) times
\par this is useful (in combination with arg_stream::chain) when you need to slip a single value
at
the front/back of an arg_stream.
*/
template <typename T>
struct single {
    typedef T value_type;

    T value;
    unsigned int repeat; // yep, unsigned.

    single(typename boost::add_reference<T const>::type t, unsigned int count = 1)
        : value(t), repeat(count) {}

    bool eof() { return repeat == 0; }

    template <typename R>
    R convert_or_throw(
        value_type& value,
        typename boost::enable_if<boost::is_convertible<value_type, R>>::type* dummy = 0) {
        return R(value);
    }
    template <typename R>
    R convert_or_throw(
        value_type& value,
        typename boost::disable_if<boost::is_convertible<value_type, R>>::type* dummy = 0) {
        throw adobe::bad_cast();
        return *(R*)value;
    }

    template <typename R>
    R get_next_arg() {
        if (repeat) {
            repeat--;
        } else {
            throw arg_stream::no_more_args();
        }

        return convert_or_throw<R>(value);
    }
};

template <typename T>
struct traits<single<T>> {
    static const bool has_eof_memberfunction = true;
};

template <typename ArgStream, typename Transformer>
struct with_transform {
    ADOBE_HAS_TEMPLATE1_IMPL(arg_stream_inverse_lookup);

    template <typename Class>
    struct has_inverse_lookup {
        static const bool value = ADOBE_HAS_TEMPLATE1(Class, arg_stream_inverse_lookup);
    };

    template <typename Class, typename R, bool>
    struct has_entry_if_has_inverse_lookup {
        static const bool value = false;
    };
    template <typename Class, typename R>
    struct has_entry_if_has_inverse_lookup<Class, R, true> {
        static const bool value =
            has_type_type<typename Class::template arg_stream_inverse_lookup<R>>::value;
    };

    template <typename Class, typename R>
    struct has_transform {
        static const bool value =
            has_entry_if_has_inverse_lookup<Class, R, has_inverse_lookup<Class>::value>::value;
    };

    typedef ArgStream value_type;
    typedef ArgStream arg_stream_type;
    typedef Transformer transformer_type;

    ArgStream& argstream;
    Transformer& transformer;

    with_transform(ArgStream& as, Transformer& trans) : argstream(as), transformer(trans) {}

    bool eof() { return detail::eof_check(argstream); }

    template <typename R>
    R transforming_get(typename boost::enable_if<has_transform<Transformer, R>>::type* dummy = 0) {
        typedef typename Transformer::template arg_stream_inverse_lookup<R>::type Rfrom;
        return transformer.template arg_stream_transform<R>(
            arg_stream::get_next_arg<Rfrom>(argstream));
    }
    template <typename R>
    R transforming_get(typename boost::disable_if<has_transform<Transformer, R>>::type* dummy = 0) {
        return arg_stream::get_next_arg<R>(argstream);
    }

    template <typename R>
    R get_next_arg() {
        return transforming_get<R>();
    }
};

template <typename ArgStream, typename Transformer>
with_transform<ArgStream, Transformer> make_transforming(ArgStream& as, Transformer& transformer) {
    return with_transform<ArgStream, Transformer>(as, transformer);
}

} // namespace arg_stream
} // namespace adobe


#endif // include_guard
