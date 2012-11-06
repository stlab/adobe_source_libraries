/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_MOVE_HPP
#define ADOBE_MOVE_HPP

#include <cassert>
#include <iterator>
#include <memory>
#include <vector>

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_class.hpp>
#include <boost/utility/enable_if.hpp>

/*!
\defgroup move_related Move Library
\ingroup utility
\brief
The move library is a collection of utilities for creating and using types that leverage
return value optimization (RVO) to avoid unnecessary copies.

\section move_tutorial Tutorial
User defined types often have remote parts either because they are implemented using a
pointer-to-implementation or are variable sized. Such objects can be expensive to copy
and are often copied unnecessarily when they are returned from functions or stored in other
objects or containers. The \ref move_related is a collection of utilities to implement types which
can be moved to elide copying in such situations as well as utilities to assist in moving value.

\par Implementing a Movable Type

A movable type models \ref concept_movable. There are three components of a movable type:
	- Satisfy the requirements of concept \ref concept_regular_type.
	- Implement a move-ctor using move_from<>.
	- Modify the assignment operator to take the operand by value and consume it.

A typical implementation of the move-ctor will simply extract the remote part, leaving the
source in a destructible state.

The assignment operator takes the operand parameter by value. Typically the simplest way
to destory the local remote part and consume the remote part of the operand is to swap
contents with the operand. This is similar to the copy-ctor and swap idiom for implementing
assignment.

Listing 1 shows an example movable class that implements a typical pointer-to-implementation
(PiPl) idiom and shows that it can be used as any regular type.

\code
#include <iostream>
#include <algorithm>

#include <boost/operators.hpp>

#include <adobe/move.hpp>

using std::swap;

struct implementation : boost::equality_comparable<implementation>
{
    explicit implementation(int x = 0) : member(x) { }

    implementation(const implementation& x) : member(x.member)
    { std::cout << "copy remote part: " << member << std::endl; }

    implementation& operator=(const implementation& x)
    {
        member = x.member;
        std::cout << "assign remote part: " << member << std::endl;
        return *this;
    }

    friend bool operator==(const implementation& x, const implementation& y)
    { return x.member == y.member; }

    int member;
};

class movable : public boost::equality_comparable<movable>
{
 public:
// model concept Regular

    explicit movable(int x = 0) : member(new implementation(x)) { }
    ~movable() { delete member; }
    movable(const movable& x) : member(new implementation(*x.member)) { }
    // operator=() implemented below

    friend bool operator==(const movable& x, const movable &y)
    { return *x.member == *y.member; }

    friend void swap(movable& x, movable& y)
    { swap(x.member, y.member); }

// model concept Movable

    // move-ctor assumes ownership of remote part
    movable(adobe::move_from<movable> x) : member(x.source.member)
    { x.source.member = 0; }

    // operator=() on a movable type takes parameter by value and consumes it
    movable& operator=(movable x)
    { swap(*this, x); return *this; }

 private:
    implementation* member;
};

int main()
{
    movable x(10);
    movable y = x;

    return 0;
}
\endcode
<center>Listing 1</center>

\verbatim
copy remote part: 10
\endverbatim
<center>Output of Listing 1</center>

\par Returning a Movable Type

We can return a movable type from a function by value and unnessary copies will be avoided as
Listing 2 illustrates:

\code
//...
movable f(int x, int y)
{ return movable(x * y); }

int main()
{
    movable x = f(10, 5);
    movable y;
    y = f(4, 3);

    return 0;
}
\endcode
<center>Listing 2</center>

\verbatim

\endverbatim
<center>Ouput of Listing 2</center>

In this example it is not necessary to make any copies. The result of f() is constructed directly
in place for x through a compiler optimization known as return value optimization or RVO. In the
case of assigning to y, the same optimization allows the compiler to construct the operand for
assignment as the result of f() which is them moved into y.

\par Implementing a Sink Function

A <em>sink</em> is any function that copies it's argument, usually for the purpose of storing it.
A sink is often a constructor or an insert function on a container. The \c operator=() on a movable
type is a form of a sink function. To implement a sink function pass the argument by value and then
use \c adobe::move() to move the argument into place. Note that this technique cannot be used to
implement \c operator=() on because it relies on assignment. Listing 3 implements an example sink
function.

\code
//...

struct sink
{
	explicit sink(movable x) : member(adobe::move(x)) { }

	movable member;
};

int main()
{
    movable x = f(10, 5);
    sink y(x);          // must copy.
    sink z(f(20, 2));   // no copy.

    return 0;
}
\endcode
<center>Listing 3</center>

\verbatim
copy remote part: 50
\endverbatim
<center>Output of Listing 3</center>

Here again unnessary copies are eliminated. Although adobe::move() can be used anytime to force the
move of an object, it should only be used as part of an explicit sink function otherwise it hinders
the understanding of code.

\par Utilities

There are many utilities as part of the move library which can be used to move elements instead of
copying them. These are useful when building containers or dealing with sink operations which must
manage a collection of movable objects. Generally these operations parallel the associated copying
algorithms from STL. Examples:

<table>
<tr><td><b>Move</b></td><td><b>Copy</b></td><td><b>Comment</b></td></tr>
<tr><td>adobe::move()</td><td>std::copy</td><td>Not to be confused with the single argument adobe::move()</td></tr>
<tr><td>adobe::move_backward()</td><td>std::copy_backward</td></tr>
<tr><td>adobe::back_move_iterator()</td><td>std::back_insert_iterator</td></tr>
<tr><td>adobe::back_mover()</td><td>std::back_inserter</td></tr>
<tr><td>adobe::move_construct()</td><td>std::construct</td></tr>
<tr><td>adobe::uninitialized_move()</td><td>std::uninitialized_copy</td></tr>
</table>

\par Theory of Operation

<em>to be written</em>

\par Acknowledgments:
The move library was inspired by the move library written by Dave Abrahams and the work on move
done by Dave Abrahams and Howard Hinnant.
*/

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

namespace implementation {

/*************************************************************************************************/

class test_can_convert_anything { };

template <typename T>
struct has_default_constructor : boost::mpl::false_ { };

template <typename T, typename A>
struct has_default_constructor<std::vector<T, A> > : boost::mpl::true_ { };

/*************************************************************************************************/

} //namespace implementation


/*************************************************************************************************/

/*!
\ingroup move_related
\brief move_from is used for move_ctors.
*/

template <typename T>
struct move_from
{
    explicit move_from(T& x) : source(x) { }
    T& source;
};

/*!
\ingroup move_related
\brief The is_movable trait can be used to identify movable types.
*/
template <typename T>
struct is_movable : boost::mpl::and_<
                        boost::is_convertible<move_from<T>, T>,
                        boost::mpl::not_<boost::is_convertible<implementation::test_can_convert_anything, T> >
                    > { };

/*************************************************************************************************/

/*!
\ingroup move_related
\brief This version of move is selected when T is_movable . It in turn calls the move
constructor. This call, with the help of the return value optimization, will cause x to be moved
instead of copied to its destination. See adobe/test/move/main.cpp for examples.

*/
template <typename T>
T move(T& x, typename boost::enable_if<is_movable<T>, void*>::type = 0)
{ return T(move_from<T>(x)); }

/*************************************************************************************************/

/*!
\ingroup move_related
\brief This version of move is selected when T is not movable . The net result will be that
x gets copied.
*/

template <typename T>
T& move(T& x, typename boost::disable_if<boost::mpl::or_<is_movable<T>,
        implementation::has_default_constructor<T> >, void*>::type = 0)
{ return x; }

template <typename T>
T move(T& x, typename boost::enable_if<boost::mpl::and_<boost::mpl::not_<is_movable<T> >,
       implementation::has_default_constructor<T> >, void*>::type = 0)
{ using std::swap; T result; swap(x, result); return result; }


/*************************************************************************************************/

/*!
\ingroup move_related
\brief Iterator pair version of move. Similar to std::copy but with move semantics,
for movable types, otherwise with copy semantics.
*/
template <typename I, // I models InputIterator
          typename O> // O models OutputIterator
O move(I f, I l, O result)
{
    while (f != l) {
        *result = adobe::move(*f);
        ++f; ++result;
    }
    return result;
}

/*************************************************************************************************/

/*!
\ingroup move_related
\brief \ref concept_convertible_to_range version of move. Similar to copy but with move semantics,
for movable types, otherwise with copy semantics.
*/
template <typename I, // I models InputRange
          typename O> // O models OutputIterator
inline O move(I& in, O out) { return adobe::move(boost::begin(in), boost::end(in), out); }

/*************************************************************************************************/

/*!
\ingroup move_related
\brief Iterator pair version of move_backwards. Similar to std::copy_backwards but with move semantics,
for movable types, otherwise with copy semantics.
*/
template <typename I, // I models BidirectionalIterator
          typename O> // O models BidirectionalIterator
O move_backward(I f, I l, O result)
{
    while (f != l) {
        --l; --result;
        *result = adobe::move(*l);
    }
    return result;
}

/*************************************************************************************************/

/*!
\ingroup move_related
\brief \ref concept_convertible_to_range version of move_backwards. Similar to std::copy_backwards but
with move semantics, for movable types, otherwise with copy semantics.
*/
template <typename I, // I models BidirectionalRange
          typename O> // O models BidirectionalIterator
inline O move_backward(I& in, O out)
{ return move_backward(boost::begin(in), boost::end(in), out); }

/*************************************************************************************************/

/*!
\ingroup move_related
\brief Similar to std::back_insert_iterator but
with move semantics, for movable types, otherwise with copy semantics.
*/

template <typename C> // C models Container
class back_move_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
    C* container_m;

 public:
    typedef C container_type;

    explicit back_move_iterator(C& x) : container_m(&x) { }

    back_move_iterator& operator=(typename C::value_type x)
    { container_m->push_back(adobe::move(x)); return *this; }

    back_move_iterator& operator*() { return *this; }
    back_move_iterator& operator++() { return *this; }
    back_move_iterator& operator++(int) { return *this; }
};

/*************************************************************************************************/

/*!
\ingroup move_related
\brief Similar to std::back_inserter but
with move semantics, for movable types, otherwise with copy semantics.
*/

template <typename C> // C models Container
inline back_move_iterator<C> back_mover(C& x) { return back_move_iterator<C>(x); }

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
