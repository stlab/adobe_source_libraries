/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_SERIALIZABLE_HPP
#define ADOBE_SERIALIZABLE_HPP

#include <adobe/config.hpp>

#include <memory>
#include <ostream>
#include <type_traits>

#include <adobe/string/to_string.hpp>
#include <adobe/typeinfo.hpp>


/**************************************************************************************************/

namespace adobe {

#if ASL_CPP_VERSION_LESS_THAN(20)

/**************************************************************************************************/

namespace implementation {

/**************************************************************************************************/

template <class>
struct sfinae_true : std::true_type {};

// test_stream_insertion is a modification of http://stackoverflow.com/a/9154394

template <class T, class Stream>
static auto test_stream_insertion(int)
    -> sfinae_true<decltype(std::declval<Stream>() << std::declval<T>())>;

template <class, class>
static auto test_stream_insertion(long) -> std::false_type;

/**************************************************************************************************/

} // namespace implementation

/**************************************************************************************************/

template <class T, class Stream>
struct has_stream_insertion : decltype(implementation::test_stream_insertion<T, Stream>(0)) {};

template <class T>
using has_ostream_insertion = has_stream_insertion<T, std::ostream&>;

/**************************************************************************************************/

template <class T>
inline typename std::enable_if<has_ostream_insertion<T>::value>::type
ostream_insertion(std::ostream& s, const T& x) {
    s << x;
}

template <class T>
inline typename std::enable_if<!has_ostream_insertion<T>::value>::type
ostream_insertion(std::ostream&, const T&) {}

#else  // ASL_CPP_VERSION_LESS_THAN(20)

template <class T>
inline void ostream_insertion(std::ostream& s, const T& x) {
    if constexpr (requires { s << x; }) {
        s << x;
    }
}
#endif // ASL_CPP_VERSION_LESS_THAN(20)

template <>
inline void ostream_insertion<bool>(std::ostream& s, const bool& x) {
    s << std::boolalpha << x << std::noboolalpha;
}

template <>
inline void ostream_insertion<double>(std::ostream& s, const double& x) {
    s << adobe::to_string(x);
}

/**************************************************************************************************/
// serialize<T> is an opportunity to hook user-defined wrapper types (e.g.,
// std::reference_wrapper)

template <typename T>
struct serialize {
    void operator()(std::ostream& s, const T& x) const { ostream_insertion(s, x); }
};

template <typename T>
struct serialize<std::reference_wrapper<T>> {
    void operator()(std::ostream& s, const std::reference_wrapper<T>& x) const {
        ostream_insertion(s, x.get());
    }
};

/**************************************************************************************************/

class serializable_t {
public:
    template <typename T>
    explicit serializable_t(T x) : instance_m(std::make_unique<instance<T>>(x)) {}

    serializable_t(const serializable_t& x) : instance_m(x.object()._copy()) {}

    serializable_t(serializable_t&& x) : instance_m(x.instance_m.release()) {}

    void operator()(std::ostream& s) const { object()._out(s); }

    const std::type_info& type_info() const { return object().type_info(); }

    template <typename T>
    const T& cast() const {
        const std::type_info& type(type_info());

        if (type != typeid(T))
            throw bad_cast(type, typeid(T));

        return static_cast<const instance<T>&>(object()).object_m;
    }

    template <typename T>
    T& cast() {
        return const_cast<T&>(const_cast<const serializable_t&>(*this).cast<T>());
    }

private:
    struct instance_t {
        virtual ~instance_t() {}

        virtual std::unique_ptr<instance_t> _copy() const = 0;

        virtual void _out(std::ostream& s) const = 0;
        virtual const std::type_info& type_info() const = 0;
    };

    template <typename T>
    struct instance final : instance_t {
        explicit instance(T x) : object_m(std::move(x)) {}

        std::unique_ptr<instance_t> _copy() const override {
            return std::make_unique<instance>(object_m);
        }

        void _out(std::ostream& s) const override { ostream_insertion(s, object_m); }

        const std::type_info& type_info() const override { return typeid(T); }

        T object_m;
    };

    instance_t& object() { return *instance_m; }
    const instance_t& object() const { return *instance_m; }

    std::unique_ptr<instance_t> instance_m;
};

/**************************************************************************************************/

inline std::ostream& operator<<(std::ostream& s, const serializable_t& x) {
    x(s);

    return s;
}

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif

/**************************************************************************************************/
