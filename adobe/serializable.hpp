/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/*************************************************************************************************/

#ifndef ADOBE_SERIALIZABLE_HPP
#define ADOBE_SERIALIZABLE_HPP

#include <adobe/config.hpp>

#include <double-conversion/src/double-conversion.h>

#include <adobe/typeinfo.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

template <typename T>
struct serialize {
    std::ostream& operator()(std::ostream& s, const T& x) const {
        return s << x;
    }
};

/*************************************************************************************************/

template <typename T>
struct serialize<std::reference_wrapper<T>> {
    std::ostream& operator()(std::ostream& s, const std::reference_wrapper<T>& x) const {
        return serialize<T>()(s, x.get());
    }
};
/*************************************************************************************************/

template <>
struct serialize<bool> {
    std::ostream& operator()(std::ostream& s, const bool& x) const {
        return s << std::boolalpha << x << std::noboolalpha;
    }
};


/*************************************************************************************************/

template <>
struct serialize<double> {
    std::ostream& operator()(std::ostream& s, const double& x) const {
        using namespace double_conversion;

        const DoubleToStringConverter& c(DoubleToStringConverter::EcmaScriptConverter());
        char buf[32] = { 0 };
        StringBuilder builder(buf, sizeof(buf));
        c.ToShortest(x, &builder);

        return s << builder.Finalize();
    }
};

/*************************************************************************************************/

class serializable_t {
public:    
    template <typename T>
    explicit serializable_t(T x) :
        instance_m(new instance<T>(x)) {}

    serializable_t(const serializable_t& x) :
        instance_m(x.object()._copy()) {}

    serializable_t(serializable_t&& x) :
        instance_m(x.instance_m.release()) {}

    std::ostream& operator()(std::ostream& s) const {
        return object()(s);
    }

    const std::type_info& type_info() const {
        return object().type_info();
    }

    template <typename T>
    const T& cast() const {
        const std::type_info& type(type_info());

        if (type != typeid(T))
            throw bad_cast(type, typeid(T));

        return static_cast<const T&>(reinterpret_cast<const instance<T>&>(object()).object_m);
    }

private:
    struct instance_t {
        virtual ~instance_t() {}

        virtual instance_t* _copy() const = 0;

        virtual std::ostream& operator()(std::ostream& s) const = 0;
        virtual const std::type_info& type_info() const = 0;
    };

    template <typename T>
    struct instance : instance_t {
        explicit instance(T x) :
            object_m(std::move(x)) {}

        instance_t* _copy() const {
            return new instance(object_m);
        }

        std::ostream& operator()(std::ostream& s) const {
            return serialize<T>()(s, object_m);
        }

        const std::type_info& type_info() const {
            return typeid (T);
        }

        T object_m;
    };

    inline instance_t& object() { return *instance_m; }
    inline const instance_t& object() const { return *instance_m; }

    std::unique_ptr<instance_t> instance_m;
};

/*************************************************************************************************/

inline std::ostream& operator<<(std::ostream& s, const serializable_t& x) {
    return x(s);
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
