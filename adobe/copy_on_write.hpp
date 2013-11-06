/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#ifndef ADOBE_COPY_ON_WRITE_HPP
#define ADOBE_COPY_ON_WRITE_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <mutex>

#include <boost/noncopyable.hpp>
#include <boost/operators.hpp>
#include <boost/static_assert.hpp>

#include <adobe/counter.hpp>
#include <adobe/memory.hpp>
#include <adobe/once.hpp>
#include <adobe/typeinfo.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

namespace version_1 {

/**************************************************************************************************/

/*!
\defgroup abi_misc ABI-Safe Utilities
\ingroup abi_safe
*/

/*!
\ingroup abi_misc util_misc
\brief A copy-on-write wrapper for any model of \ref concept_regular_type.

\model_of
    - \ref concept_movable

\requirements
    - \ref T models \ref concept_regular_type (supports T models \ref concept_movable).

Copy-on-write sematics allow for object to be lazily copied - only creating a
copy when the value is modified and there is more than one reference to the
value.

\note
copy_on_write is thread safe when complier with BOOST_HAS_THREADS defined.

\note
To get an instance with a particular allocator one would have to write:
<pre>
copy_on_write<T> x(custom_allocator);
x.write() = value;
</pre>
*/
template <typename T,                               // T models Regular
          typename A = adobe::capture_allocator<T>> // A models Allocator
class copy_on_write {
public:
    /// \brief The type of value stored.
    typedef T value_type;
    /// \brief The type of allocator.
    typedef A allocator_type;

#if !defined(ADOBE_NO_DOCUMENTATION)
private:
    struct implementation_t;
    typedef typename allocator_type::template rebind<implementation_t>::other
    implementation_allocator_type;

public:
#endif

    /*!
        The first call to the default constructor will construct a default
        instance of value_type which will be used for subsequent calls to the
        default constructor. The default instance will be released at exit.
    */
    copy_on_write() {
        std::call_once(flag_s, init_default);
        object_m = default_s;
        object_m->header_m.get().count_m.increment();
    }

    /*!
        Initializes the instance with a custom allocator
    */
    explicit copy_on_write(const allocator_type &a) : object_m(0) {
        implementation_allocator_type other_allocator(a);

        object_m = allocate(other_allocator);
    }

    /*!
    \brief Constructs a new copy_on_write object with a value \c x.

    \param x A default value to assign to this object
    */
    copy_on_write(T x) : object_m(allocate_move(0, std::move(x))) {}

    /*!
    Copy construction is a non-throwing operation and simply increments the
    reference count on the stored object.
    */
    copy_on_write(const copy_on_write &x) : object_m(x.object_m) {
        if (object_m)
            object_m->header_m.get().count_m.increment();
    }

    copy_on_write(copy_on_write &&x) : object_m(x.object_m) { x.object_m = 0; }

    ~copy_on_write() { release(object_m); }

    /*!
        As with copy construction, assignment is a non-throwing operation which
        releases the old value and increments the reference count of the item
        being assigned to.
    */
    copy_on_write &operator=(copy_on_write x) {
        swap(*this, x);
        return *this;
    }


    copy_on_write &operator=(T x) {
        if (!object_m)
            object_m = allocate_move(0, std::move(x));
        else if (object_m->header_m.get().count_m.is_one())
            object_m->value_m = std::move(x);
        else
            reset(allocate_move(object_m, std::move(x)));

        return *this;
    }

    /*!
    Obtain a reference to the value the object is referencing. This will copy
    the underlying value (if necessary) so changes to the value do not affect
    other copy_on_write objects.

    Note that write() does not have the same preconditions as operator=().
    write() returns a reference to the underlying object's value, thus requiring
    that an underlying object exist. operator=() on the other hand will perform
    an allocation if one is necessary.

    \return A reference to the underlying object
    */
    value_type &write() {
        assert(object_m && "FATAL (sparent) : using a moved copy_on_write object");

        if (!object_m->header_m.get().count_m.is_one())
            reset(allocate(object_m, object_m->value_m));

        return object_m->value_m;
    }

    /*!
    \brief Obtain a const reference to the underlying object.

    \return A const reference to the underlying object
    */
    const value_type &read() const {
        assert(object_m && "FATAL (sparent) : using a moved copy_on_write object");
        return object_m->value_m;
    }

    /*!
    \brief Obtain a const reference to the underlying object.

    \return A const reference to the underlying object
    */
    operator const value_type &() const { return read(); }

    /*!
    \brief Obtain a const reference to the underlying object.

    This is provided because you cannot override <code>operator.()</code>. It
    allows copy_on_write to be used with common transformation techniques, such
    as boost <code>indirect_iterator</code> and <code>transform_iterator</code>.
    It does not imply "pointer" semantics.

    \return A const reference to the underlying object
    */
    const value_type &operator*() const { return read(); }

    /*!
    \brief Obtain a const pointer to the underlying object.

    This is provided because you cannot override <code>operator.()</code>. It
    allows copy_on_write to be used with common transformation techniques, such
    as boost <code>indirect_iterator</code> and <code>transform_iterator</code>.
    It does not imply "pointer" semantics.

    \return A const pointer to the underlying object
    */
    const value_type *operator->() const { return &read(); }

    /*!
    \brief unique_instance returns whether or not the reference count to the
    object instance is one. This is useful to determine if writing will cause a
    copy.

    \return <code>true</code> if the ref count for the instance is one.
    */
    bool unique_instance() const { return !object_m || object_m->header_m.get().count_m.is_one(); }

    /*!
    \brief identity is used to see if two copy_on_write items refer to the same
    instance.

    \return Boolean; <code>true</code> if the underlying object instance is
    shared by both objects.
    */
    bool identity(const copy_on_write &x) const { return object_m == x.object_m; }

    friend inline void swap(copy_on_write &x, copy_on_write &y) {
        std::swap(x.object_m, y.object_m);
    }

    friend inline bool operator<(const copy_on_write &x, const copy_on_write &y) {
        return y.object_m && (!x.object_m || (!x.identity(y) && *x < *y));
    }

    friend inline bool operator>(const copy_on_write &x, const copy_on_write &y) { return y < x; }

    friend inline bool operator<=(const copy_on_write &x, const copy_on_write &y) {
        return !(y < x);
    }

    friend inline bool operator>=(const copy_on_write &x, const copy_on_write &y) {
        return !(x < y);
    }

    friend inline bool operator==(const copy_on_write &x, const copy_on_write &y) {
        return x.identity(y) || (x.object_m && y.object_m && *x == *y);
    }

    friend inline bool operator!=(const copy_on_write &x, const copy_on_write &y) {
        return !(x == y);
    }

    allocator_type get_allocator() const {
        return object_m ? allocator_type(object_m->get_allocator()) : allocator_type();
    }

private:
#if !defined(ADOBE_NO_DOCUMENTATION)
    static implementation_t *allocate(const implementation_t *alloc_src, const T &x = T()) {
        implementation_allocator_type allocator(alloc_src ? alloc_src->get_allocator()
                                                          : implementation_allocator_type());

        return allocate(allocator, x);
    }

    static implementation_t *allocate(implementation_allocator_type &allocator, const T &x = T()) {
        implementation_t *tmp(allocator.allocate(1));

        try {
            ::new (static_cast<void *>(tmp)) implementation_t(x);
        }
        catch (...) {
            tmp->get_allocator().deallocate(tmp, 1);
            throw;
        }

        return tmp;
    }

    static implementation_t *allocate_move(const implementation_t *alloc_src, T x) {
        implementation_allocator_type allocator(alloc_src ? alloc_src->get_allocator()
                                                          : implementation_allocator_type());
        implementation_t *tmp(allocator.allocate(1));

        try {
            ::new (static_cast<void *>(tmp)) implementation_t(std::move(x));
        }
        catch (...) {
            tmp->get_allocator().deallocate(tmp, 1);
            throw;
        }

        return tmp;
    }

    static void release(implementation_t *x) {
        /*
            I thought about returning a bool from this routine (denoting whether
            or not a deallocation took place) but decided not to in the end.
            Release's semantics are that of giving up ownership of the
            implementation instance, so you are not allowed to subsequently
            interact with the implementation instance you've released. Thus,
            notifying the caller of a deallocation is a moot point.
        */

        if (x == 0 || x->header_m.get().count_m.decrement() == false)
            return;

        implementation_allocator_type allocator(x->get_allocator());

        destroy(x);

        allocator.deallocate(x, 1);
    }

    void reset(implementation_t *to) {
        release(object_m);
        object_m = to;
    }

    implementation_t *object_m;

    static std::once_flag flag_s;
    static implementation_t *default_s;

    static void release_default();
    static void init_default();
#endif
};

/**************************************************************************************************/

#if !defined(ADOBE_NO_DOCUMENTATION)

/**************************************************************************************************/

template <typename T, typename A>
std::once_flag copy_on_write<T, A>::flag_s;

template <typename T, typename A>
typename copy_on_write<T, A>::implementation_t *copy_on_write<T, A>::default_s;

template <typename T, typename A>
struct copy_on_write<T, A>::implementation_t : private boost::noncopyable {
    // Assert proper size for counter_t
    BOOST_STATIC_ASSERT((sizeof(counter_t) == sizeof(std::size_t)));
    // Assert proper alignment for counter_t
    BOOST_STATIC_ASSERT((sizeof(counter_t) == sizeof(void *)));

    struct header_t {
        counter_t count_m;
        allocator_type allocator_m;
    };

    explicit implementation_t(T x) : value_m(std::move(x)) {}

    implementation_allocator_type get_allocator() const {
        return implementation_allocator_type(header_m.get().allocator_m);
    }

    aligned_storage<header_t> header_m;
    value_type value_m;
};

template <typename T, typename A>
void copy_on_write<T, A>::init_default() {
    implementation_allocator_type allocator;

    default_s = allocate(allocator);

    std::atexit(release_default); // ignore failure
}

template <typename T, typename A>
void copy_on_write<T, A>::release_default() {
    release(default_s);
}

#endif

/**************************************************************************************************/

} // namespace version_1

/**************************************************************************************************/

using version_1::copy_on_write;

/**************************************************************************************************/

#if 0
template <typename T, typename A> struct is_movable<copy_on_write<T, A> > : boost::mpl::true_ { };
#endif

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
