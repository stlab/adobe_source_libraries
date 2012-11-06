/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_XSTRING_HPP
#define ADOBE_XSTRING_HPP

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/functional.hpp>
#include <adobe/implementation/string_pool.hpp>
#include <adobe/istream.hpp>
#include <adobe/name.hpp>
#include <adobe/string.hpp>
#include <adobe/unicode.hpp>
#include <adobe/xml_parser.hpp>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <sstream>
#include <vector>
#include <map>
#include <cassert>
#include <cctype>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

inline bool xstring_preorder_predicate(const token_range_t& range)
{
    // we want to check for both xstr and marker tags because both are
    // handled by the xstring system

    return  token_range_equal(range, static_token_range("xstr")) ||
            token_range_equal(range, static_token_range("marker"));
}

/****************************************************************************************************/

struct null_output_t
{
    typedef std::output_iterator_tag    iterator_category;
    typedef null_output_t               value_type;
    typedef std::ptrdiff_t              difference_type;
    typedef value_type*                 pointer;
    typedef value_type&                 reference;

    null_output_t&  operator ++ (int) { return *this; }
    null_output_t&  operator ++ () { return *this; }
    reference       operator * () { return *this; }

    template <typename T>
    null_output_t& operator = (const T&) { return *this; }
};

/****************************************************************************************************/

token_range_t xml_xstr_store(const token_range_t&     entire_element_range,
                                    const token_range_t&     name,
                                    const attribute_set_t&   attribute_set,
                                    const token_range_t&     value);

token_range_t xml_xstr_lookup(   const token_range_t&     entire_element_range,
                                        const token_range_t&     name,
                                        const attribute_set_t&   attribute_set,
                                        const token_range_t&     value);

token_range_t xml_element_finalize(  const token_range_t&     entire_element_range,
                                            const token_range_t&     name,
                                            const attribute_set_t&   attribute_set,
                                            const token_range_t&     value);

/*************************************************************************************************/

struct context_frame_t
{
    struct comp_t
    {
        bool operator () (const token_range_t& x, const token_range_t& y) const
        {
            return token_range_less(x, y);
        }
    };

    typedef std::pair<attribute_set_t, token_range_t> element_t;
    typedef std::multimap<token_range_t, element_t, comp_t>  store_t;
    typedef store_t::iterator                                       store_iterator;
    typedef store_t::value_type                                     store_value_type;
    typedef std::pair<store_iterator, store_iterator>               store_range_pair_t;

    typedef xml_parser_t<char*>::callback_proc_t                    callback_proc_t;
    typedef xml_parser_t<char*>::preorder_predicate_t               preorder_predicate_t;

    context_frame_t() :
        parse_info_m("xstring context_frame_t"),
        parsed_m(false)
    { }

    context_frame_t(const context_frame_t& rhs) :
        parse_info_m(rhs.parse_info_m),
        parsed_m(rhs.parsed_m),
        attribute_set_m(rhs.attribute_set_m),
        glossary_m(rhs.glossary_m),
        callback_m(rhs.callback_m),
        predicate_m(rhs.predicate_m)
        //slurp_m(rhs.slurp_m), // not to be transferred from context to context
        //pool_m(rhs.pool_m), // not to be transferred from context to context
    { }

    context_frame_t& operator = (const context_frame_t& rhs)
    {
        parse_info_m = rhs.parse_info_m;
        parsed_m = rhs.parsed_m;
        attribute_set_m = rhs.attribute_set_m;
        glossary_m = rhs.glossary_m;
        callback_m = rhs.callback_m;
        predicate_m = rhs.predicate_m;
        //slurp_m = rhs.slurp_m; // not to be transferred from context to context
        //pool_m = rhs.pool_m; // not to be transferred from context to context

        return *this;
    }

    ~context_frame_t()
        { if (slurp_m.first) delete [] slurp_m.first; }

    inline store_range_pair_t range_for_key(const store_t::key_type& key)
        { return glossary_m.equal_range(key); }

    std::pair<bool, store_iterator> exact_match_exists( const attribute_set_t&   attribute_set,
                                                        const token_range_t&     value);

    store_t::mapped_type*   store(  const store_t::key_type&        key,
                                    const attribute_set_t&   attribute_set,
                                    const token_range_t&     value,
                                    bool                            copy = false);

    store_iterator          closest_match(  store_range_pair_t              range,
                                            const attribute_set_t&   searching);

    token_range_t element_handler(   const token_range_t&     entire_element_range,
                                            const token_range_t&     name,
                                            const attribute_set_t&   attribute_set,
                                            const token_range_t&     value) const
    {
        if (xstring_preorder_predicate(name))
            // Note that this implicitly handles "marker" elements (by echoing them)
            return xml_xstr_lookup(entire_element_range, name, attribute_set, value);
        else if (predicate_m && predicate_m(name))
            return callback_m(entire_element_range, name, attribute_set, value);
        else
            return xml_element_strip(entire_element_range, name, attribute_set, value);
    }

    token_range_t   clone(const token_range_t& token);

    line_position_t      parse_info_m;
    bool                        parsed_m;
    attribute_set_t      attribute_set_m;
    store_t                     glossary_m;
    callback_proc_t             callback_m;
    preorder_predicate_t        predicate_m;
    token_range_t        slurp_m;
    unique_string_pool_t pool_m;
};

/*************************************************************************************************/

inline bool operator == (const context_frame_t::element_t& x, const context_frame_t::element_t& y)
    { return x.first == y.first && token_range_equal(x.second, y.second); }

/*************************************************************************************************/

implementation::context_frame_t& top_frame();

/*************************************************************************************************/

} // namespace implementation

/*************************************************************************************************/
#ifndef NDEBUG

void xstring_clear_glossary();

#endif
/*************************************************************************************************/

// XML fragment parsing

template <typename O> // O models OutputIterator
inline void parse_xml_fragment(uchar_ptr_t fragment, std::size_t n, O output)
{
    const implementation::context_frame_t& context(implementation::top_frame());

    make_xml_parser( fragment,
							fragment + n,
							line_position_t("parse_xml_fragment"),
							always_true<token_range_t>(),
							boost::bind(&implementation::context_frame_t::element_handler, boost::cref(context), _1, _2, _3, _4),
							output)

    .parse_content(); // REVISIT (fbrereto) : More or less legible than having it after the above declaration?
}

template <typename O> // O models OutputIterator
inline void parse_xml_fragment(const std::string& fragment, O output)
    { return parse_xml_fragment(reinterpret_cast<uchar_ptr_t>(fragment.c_str()), fragment.size(), output); }

template <typename O> // O models OutputIterator
inline void parse_xml_fragment(const char* fragment, O output)
    { return parse_xml_fragment(reinterpret_cast<uchar_ptr_t>(fragment), std::strlen(fragment), output); }

/*************************************************************************************************/

// xstring lookup with OutputIterator; all of these functions return a valid XML fragment

template <typename O> // O models OutputIterator; required: sizeof(value_type(O)) >= 21 bits
inline void xstring(const char* xstr, std::size_t n, O output)
    { parse_xml_fragment(reinterpret_cast<uchar_ptr_t>(xstr), n, output); }

template <typename O> // O models OutputIterator; required: sizeof(value_type(O)) >= 21 bits
inline void xstring(const char* xstr, O output)
    { xstring(xstr, std::strlen(xstr), output); }

/*************************************************************************************************/

// xstring lookup; all of these functions return a valid XML fragment

inline std::string xstring(const char* xstr, std::size_t n)
{
    std::string result;

    xstring(xstr, n, std::back_inserter(result));

    return result;
}

inline std::string xstring(const std::string& xstr)
    { return xstring(xstr.c_str(), xstr.size()); }

/*************************************************************************************************/

// Context-sensitive marker replacement

std::string xstring_replace(    const std::string& xstr,
                                const std::string& marker);

std::string xstring_replace(    const std::string& xstr,
                                const std::string* first,
                                const std::string* last);

std::string xstring_replace(    const name_t& xstr_id,
                                const std::string& marker);

std::string xstring_replace(    const name_t& xstr_id,
                                const std::string* first,
                                const std::string* last);

/*************************************************************************************************/

struct xstring_context_t : boost::noncopyable
{
    typedef implementation::context_frame_t::callback_proc_t         callback_proc_t;
    typedef implementation::context_frame_t::preorder_predicate_t    preorder_predicate_t;

    xstring_context_t(  const char*                     parse_first,
                        const char*                     parse_last,
                        const line_position_t&   parse_info =
                            line_position_t("xstring_context_t")) :
        back_frame_m(implementation::top_frame()) // save snapshot of stack
    {
        implementation::context_frame_t& context(implementation::top_frame());

        context.slurp_m.first = reinterpret_cast<uchar_ptr_t>(parse_first);
        context.slurp_m.second = reinterpret_cast<uchar_ptr_t>(parse_last);
        context.parse_info_m = parse_info;
        context.parsed_m = false;

        glossary_parse();
    }

    template <typename I> // I models InputIterator
    xstring_context_t(  I   first_attribute,
                        I   last_attribute) :
        back_frame_m(implementation::top_frame()) // save snapshot of stack
    {
        implementation::top_frame().attribute_set_m.insert(first_attribute, last_attribute);
    }

    template <typename I> // I models InputIterator
    xstring_context_t(  I                               first_attribute,
                        I                               last_attribute,
                        const unsigned char*            parse_first,
                        const unsigned char*            parse_last,
                        const line_position_t&   parse_info =
                            line_position_t("xstring_context_t")) :
        back_frame_m(implementation::top_frame()) // save snapshot of stack
    {
        implementation::context_frame_t& context(implementation::top_frame());

        context.attribute_set_m.insert(first_attribute, last_attribute);
        context.slurp_m.first = parse_first;
        context.slurp_m.second = parse_last;
        context.parse_info_m = parse_info;
        context.parsed_m = false;

        glossary_parse();
    }

    void set_preorder_predicate(preorder_predicate_t proc)
    { implementation::top_frame().predicate_m = proc; }

    void set_element_handler(callback_proc_t proc)
    { implementation::top_frame().callback_m = proc; }

    ~xstring_context_t()
    { implementation::top_frame() = back_frame_m; } // restore stack as it was

private:
    void glossary_parse()
    {
        implementation::context_frame_t& context(implementation::top_frame());

        if (context.parsed_m || !boost::size(context.slurp_m))
            return;

        make_xml_parser(
            context.slurp_m.first,
            context.slurp_m.second,
            context.parse_info_m,
            implementation::xstring_preorder_predicate,
            &implementation::xml_xstr_store,
            implementation::null_output_t())

        .parse_element_sequence(); // REVISIT (fbrereto) : More or less legible than having it after the above declaration?

        context.parsed_m = true;
    }

    implementation::context_frame_t back_frame_m;
};

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/
#ifdef __ADOBE_COMPILER_CONCEPTS__
namespace std {
    // It would be nice to be able to instantiate this for all T. Not sure why it doesn't work.
    concept_map OutputIterator<adobe::implementation::null_output_t, char> {};
    concept_map OutputIterator<adobe::implementation::null_output_t, unsigned char> {};
}
#endif

/*************************************************************************************************/

#endif

/*************************************************************************************************/
