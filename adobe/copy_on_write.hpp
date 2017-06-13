/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_COPY_ON_WRITE_HPP
#define ADOBE_COPY_ON_WRITE_HPP

/**************************************************************************************************/

#include <atomic>
#include <cassert>
#include <cstddef>
#include <utility>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

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
copy_on_write is thread safe.

*/
template <typename T> // T models Regular
class copy_on_write {
#if !defined(ADOBE_NO_DOCUMENTATION)

    struct model {
        std::atomic<std::size_t> _count{1};

        model() = default;

        template <class V>
        explicit model(V&& x) : _value(std::forward<V>(x)) {}

        T _value;
    };

    void destroy() {
        if (_self && (--_self->_count == 0)) delete _self;
    }

    model* _self;

    template <class U>
    using disable_copy = std::enable_if_t<!std::is_same<std::decay_t<U>, copy_on_write>::value>*;

    template <typename U>
    using disable_copy_assign =
        std::enable_if_t<!std::is_same<std::decay_t<U>, copy_on_write>::value, copy_on_write&>;
#endif

public:
    /// \brief The type of value stored.
    /* [[deprecated]] */ using value_type = T;

    /// \brief The type of value stored.
    using element_type = T;

    /*!
        The first call to the default constructor will construct a default
        instance of element_type which will be used for subsequent calls to the
        default constructor. The default instance will be released at exit.
    */
    copy_on_write() {
        static model default_s;
        _self = &default_s;
        ++_self->_count;
    }

    /*!
    \brief Constructs a new copy_on_write object with a value \c x.

    \param x A default value to assign to this object
    */
    template <class U>
    copy_on_write(U&& x, disable_copy<U> = nullptr) : _self(new model(std::forward<U>(x))) {}

    /*!
    Copy construction is a non-throwing operation and simply increments the
    reference count on the stored object.
    */
    copy_on_write(const copy_on_write& x) noexcept : _self(x._self) { ++_self->_count; }
    copy_on_write(copy_on_write&& x) noexcept : _self(x._self) { x._self = nullptr; }

    ~copy_on_write() { destroy(); }

    /*!
        As with copy construction, assignment is a non-throwing operation which
        releases the old value and increments the reference count of the item
        being assigned to.
    */
    auto operator=(const copy_on_write& x) noexcept -> copy_on_write& {
        return *this = copy_on_write(x);
    }
    auto operator=(copy_on_write&& x) noexcept -> copy_on_write& {
        assert(!identity(x) && "FATAL (sparent) : invalid self-move.");
        
        destroy();
        _self = x._self;
        x._self = nullptr;
        return *this;
    }

    template <class U>
    auto operator=(U&& x) -> disable_copy_assign<U> {
        if (_self && unique()) {
            _self->_value = std::forward<U>(x);
            return *this;
        }

        return *this = copy_on_write(std::forward<U>(x));
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
    auto write() -> element_type& {
        assert(_self && "FATAL (sparent) : using a moved copy_on_write object");

        if (!unique()) {
            auto tmp = new model(_self->_value);
            destroy();
            _self = tmp;
        }

        return _self->_value;
    }

    /*!
    \brief Obtain a const reference to the underlying object.

    \return A const reference to the underlying object
    */
    auto read() const noexcept -> const element_type& {
        assert(_self && "FATAL (sparent) : using a moved copy_on_write object");

        return _self->_value;
    }

    /*!
    \brief Obtain a const reference to the underlying object.

    \return A const reference to the underlying object
    */
    operator const element_type&() const noexcept { return read(); }

    /*!
    \brief Obtain a const reference to the underlying object.

    This is provided because you cannot override <code>operator.()</code>. It
    allows copy_on_write to be used with common transformation techniques, such
    as boost <code>indirect_iterator</code> and <code>transform_iterator</code>.
    It does not imply "pointer" semantics.

    \return A const reference to the underlying object
    */
    auto operator*() const noexcept -> const element_type& { return read(); }

    /*!
    \brief Obtain a const pointer to the underlying object.

    This is provided because you cannot override <code>operator.()</code>. It
    allows copy_on_write to be used with common transformation techniques, such
    as boost <code>indirect_iterator</code> and <code>transform_iterator</code>.
    It does not imply "pointer" semantics.

    \return A const pointer to the underlying object
    */
    auto operator-> () const noexcept -> const element_type* { return &read(); }

    /*!
    \brief unique returns whether or not the reference count to the
    object instance is one. This is useful to determine if writing will cause a
    copy.

    \return <code>true</code> if the ref count for the instance is one.
    */
    bool unique() const noexcept {
        assert(_self && "FATAL (sparent) : using a moved copy_on_write object");

        return _self->_count == 1;
    }
    [[deprecated]] bool unique_instance() const noexcept { return unique(); }

    /*!
    \brief identity is used to see if two copy_on_write items refer to the same
    instance.

    \return Boolean; <code>true</code> if the underlying object instance is
    shared by both objects.
    */
    bool identity(const copy_on_write& x) const noexcept { return _self == x._self; }

    friend inline void swap(copy_on_write& x, copy_on_write& y) noexcept {
        std::swap(x._self, y._self);
    }

    friend inline bool operator<(const copy_on_write& x, const copy_on_write& y) noexcept {
        return !x.identity(y) && (*x < *y);
    }

    friend inline bool operator>(const copy_on_write& x, const copy_on_write& y) noexcept {
        return y < x;
    }

    friend inline bool operator<=(const copy_on_write& x, const copy_on_write& y) noexcept {
        return !(y < x);
    }

    friend inline bool operator>=(const copy_on_write& x, const copy_on_write& y) noexcept {
        return !(x < y);
    }

    friend inline bool operator==(const copy_on_write& x, const copy_on_write& y) noexcept {
        return x.identity(y) || (*x == *y);
    }

    friend inline bool operator!=(const copy_on_write& x, const copy_on_write& y) noexcept {
        return !(x == y);
    }
};
/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/

