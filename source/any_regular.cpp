/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#if defined(ADOBE_STD_SERIALIZATION)

#include <adobe/any_regular.hpp>

#include <cassert>
#include <string>

#ifdef ADOBE_BUILT_WITH_CMAKE
#include <double-conversion/double-conversion.h>
#else
#include <double-conversion/src/double-conversion.h>
#endif

#include <adobe/algorithm/sorted.hpp>
#include <adobe/algorithm/lower_bound.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/iomanip.hpp>
#include <adobe/name.hpp>
#include <adobe/string.hpp>
#include <adobe/table_index.hpp>

#include <ostream>

using namespace std;

/**************************************************************************************************/

namespace {

using namespace adobe;

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

namespace implementation {

/**************************************************************************************************/

struct serializable_t {
    virtual ~serializable_t() {};
    virtual void operator()(std::ostream& out, const any_regular_t& x) const = 0;
};

/**************************************************************************************************/

template <typename T>
struct serializable : serializable_t {
    serializable() {}
    void operator()(std::ostream& out, const any_regular_t& x) const { out << format(x.cast<T>()); }
};

/**************************************************************************************************/

template <>
struct serializable<double> : serializable_t {
    serializable() {}
    void operator()(std::ostream& out, const any_regular_t& x) const {
        using namespace double_conversion;

        const DoubleToStringConverter& c(DoubleToStringConverter::EcmaScriptConverter());
        char                           buf[32] = { 0 };
        StringBuilder                  builder(buf, sizeof(buf));
        c.ToShortest(x.cast<double>(), &builder);
        out << builder.Finalize();
    }
};

/**************************************************************************************************/

template <typename T, typename Any = void>
struct make_serializable {
    static const serializable<T> value;
};

template <typename T, typename Any>
const serializable<T> make_serializable<T, Any>::value;

/**************************************************************************************************/

typedef aggregate_pair<const std::type_info*, const serializable_t*> serializable_lookup_t;

serializable_lookup_t serializable_table[] = {
    {&typeid(bool), &make_serializable<bool>::value},
    {&typeid(dictionary_t), &make_serializable<dictionary_t>::value},
    {&typeid(double), &make_serializable<double>::value},
    {&typeid(empty_t), &make_serializable<empty_t>::value},
    {&typeid(name_t), &make_serializable<name_t>::value},
    {&typeid(string), &make_serializable<string>::value},
    {&typeid(array_t), &make_serializable<array_t>::value}};

/**************************************************************************************************/

} // namespace implementation

/**************************************************************************************************/

namespace version_1 {

std::ostream& operator<<(std::ostream& out, const any_regular_t& x) {
    using namespace implementation;

    static bool inited = false;

    if (!inited) {
        inited = true;
        sort(serializable_table, &std::type_info::before,
             [](const serializable_lookup_t & x)->const std::type_info & { return *x.first; });
    }

    serializable_lookup_t* i = binary_search(
        serializable_table, x.type_info(), &std::type_info::before,
        [](const serializable_lookup_t & x)->const std::type_info & { return *x.first; });

    if (i == boost::end(serializable_table))
        throw std::logic_error("Type not serializable.");

    (*i->second)(out, x);

    return out;
}

std::ostream& operator<<(std::ostream& out, const dictionary_t& x) {
    typedef table_index<const name_t, const dictionary_t::value_type> index_type;

    index_type index(x.begin(), x.end(), &dictionary_t::value_type::first);
    index.sort();

    out << begin_bag("[0]");

    for (index_type::const_iterator first(index.begin()), last(index.end()); first != last;
         ++first) {
        out << begin_sequence;
        out << format(first->first);
        out << format(first->second);
        out << end_sequence;
    }

    out << end_bag;

    return out;
}

} // namespace version_1

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

#endif
