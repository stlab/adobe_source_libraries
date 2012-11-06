/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/xstring.hpp>

#include <adobe/algorithm/find.hpp>
#include <adobe/algorithm/find_match.hpp>
#include <adobe/algorithm/minmax.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/implementation/string_pool.hpp>
#include <adobe/istream.hpp>
#include <adobe/name.hpp>
#include <adobe/once.hpp>

#include <boost/cstdint.hpp>

#include <boost/iterator/transform_iterator.hpp>

#if !defined(NDEBUG) && defined(ADOBE_SERIALIZATION)
    #define ADOBE_DOING_SERIALIZATION 1
#endif

#if ADOBE_DOING_SERIALIZATION
    #include <iostream>
#endif

/*************************************************************************************************/

ADOBE_ONCE_DECLARATION(xstr_once)
ADOBE_ONCE_STATIC_INSTANCE(xstr_once)
#if 0
{ } // REVISIT (fbrereto) : workaround for a bug in the BBEdit function menu parser
#endif

/*************************************************************************************************/

namespace {

/*************************************************************************************************/

adobe::token_range_t*   xstr_tag_g = 0;
adobe::token_range_t*   marker_tag_g = 0;
adobe::token_range_t*   attribute_id_g = 0;
adobe::token_range_t*   attribute_lang_g = 0;
adobe::token_range_t*   attribute_platform_g = 0;

/*************************************************************************************************/

ADOBE_THREAD_LOCAL_STORAGE(adobe::implementation::context_frame_t, thread_context)
#if 0
{ } // REVISIT (fbrereto) : workaround for a bug in the BBEdit function menu parser
#endif

/*************************************************************************************************/

#if 0
#pragma mark -
#endif

/*************************************************************************************************/

void init_xstr_once()
{
    // initialize the thread-specific context attribute set

    ADOBE_THREAD_LOCAL_STORAGE_INITIALIZE(thread_context);

    adobe::implementation::context_frame_t& context(adobe::implementation::top_frame());

    static adobe::token_range_t xstr_tag_s(adobe::static_token_range("xstr"));
    static adobe::token_range_t marker_tag_s(adobe::static_token_range("marker"));
    static adobe::token_range_t attribute_id_s(adobe::static_token_range("id"));
    static adobe::token_range_t attribute_lang_s(adobe::static_token_range("lang"));
    static adobe::token_range_t attribute_platform_s(adobe::static_token_range("platform"));

    xstr_tag_g = &xstr_tag_s;
    marker_tag_g = &marker_tag_s;
    attribute_id_g = &attribute_id_s;
    attribute_lang_g = &attribute_lang_s;
    attribute_platform_g = &attribute_platform_s;

    context.attribute_set_m.insert(std::make_pair(attribute_platform_s, adobe::static_token_range(
#if ADOBE_PLATFORM_LINUX
    "linux"
#elif ADOBE_PLATFORM_BSD
    "bsd"
#elif ADOBE_PLATFORM_SOLARIS
    "solaris"
#elif ADOBE_PLATFORM_IRIX
    "irix"
#elif ADOBE_PLATFORM_HPUX
    "hpux"
#elif ADOBE_PLATFORM_CYGWIN
    "cygwin"
#elif ADOBE_PLATFORM_WIN
    "windows"
#elif ADOBE_PLATFORM_BEOS
    "beos"
#elif ADOBE_PLATFORM_MAC
    "macintosh"
#elif ADOBE_PLATFORM_AIX
    "aix"
#elif ADOBE_PLATFORM_AMIGA
    "amiga"
#elif ADOBE_PLATFORM_UNIX
    "unix"
#else
    #error "Unknown platform - please configure and report the results to stlab.adobe.com"
#endif
)));

    // REVISIT (fbrereto) : This language code should be runtime obtainable, not static

    context.attribute_set_m.insert(std::make_pair(attribute_lang_s, adobe::static_token_range("en-us")));
}

/****************************************************************************************************/

} // namespace

/*************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

namespace implementation {

/*************************************************************************************************/

adobe::implementation::context_frame_t& top_frame()
{ return ADOBE_THREAD_LOCAL_STORAGE_ACCESS(thread_context); }

/*************************************************************************************************/

#if 0
#pragma mark -
#endif

/*************************************************************************************************/
#if ADOBE_DOING_SERIALIZATION
std::ostream& operator << (std::ostream& s, const context_frame_t::element_t& element)
{
    s << "<xstr " << element.first << ">" << std::string(element.second.first, element.second.second) << "</xstr>";

    return s;
}
#endif
/*************************************************************************************************/

#if 0
#pragma mark -
#endif

/*************************************************************************************************/

inline std::pair<bool, context_frame_t::store_iterator> context_frame_t::exact_match_exists(    const adobe::attribute_set_t&   attribute_set,
                                                                                                const adobe::token_range_t&     value)
{
    store_range_pair_t              the_range(range_for_key(attribute_set[*attribute_id_g]));
    std::pair<bool, store_iterator> result;

    result.second = adobe::find_match(the_range, element_t(attribute_set, value),
                        adobe::compare_members(&store_value_type::second,
                        std::equal_to<element_t>()));

    result.first = result.second != the_range.second;

    return result;
}

/*************************************************************************************************/

token_range_t context_frame_t::clone(const token_range_t& range)
{
    //std::size_t len(boost::size(range));
    std::string str(boost::begin(range), boost::end(range));
    const char* added(pool_m.add(str.c_str()));

    return token_range_t(reinterpret_cast<uchar_ptr_t>(added), reinterpret_cast<uchar_ptr_t>(added) + str.length());
}

/*************************************************************************************************/

context_frame_t::store_t::mapped_type* context_frame_t::store(  const store_t::key_type&        key,
                                                                const adobe::attribute_set_t&   attribute_set,
                                                                const adobe::token_range_t&     value,
                                                                bool                            copy)
{
    assert (std::distance(boost::begin(key), boost::end(key)));

    store_t::key_type       to_key(key);
    adobe::attribute_set_t  to_mapped_attribute_set(attribute_set);
    adobe::token_range_t    to_mapped_value(value);

    std::pair<bool, store_iterator> found_exact(exact_match_exists(to_mapped_attribute_set, to_mapped_value));

    if (found_exact.first)
        return &(found_exact.second->second);

    if (copy)
    {
        // the reason we need to copy is if the lookup failed and we want to add
        // the xstring and its default value into the dictionary. In that case
        // all the iterators and pointers in the token_range_t instances are all
        // pointing to temporary data. In here we need to clone everything --
        // key, attribute_set, and value, because they all point to parts of the
        // temporary string we were using for lookup.

        to_mapped_attribute_set.clear();

        to_key          = clone(key);
        to_mapped_value = clone(value);

        for (   adobe::attribute_set_t::const_iterator first(attribute_set.begin()),
                last(attribute_set.end()); first != last; ++first)
            to_mapped_attribute_set.insert(clone(first->first), clone(first->second));
    }

    store_iterator result(glossary_m.insert(store_value_type(   to_key,
                                                                element_t(  to_mapped_attribute_set,
                                                                            to_mapped_value))));

    assert (result != glossary_m.end());

#if 0 && ADOBE_DOING_SERIALIZATION
    std::cerr   << "Added value " << glossary_m.size() << " \""
                << result->second << "\" to glossary "
                << static_cast<void*>(&glossary_m);

    if (copy)
        std::cerr << " (copied)";

    std::cerr << std::endl;

#if 0
    for (   store_t::iterator first(glossary_m.begin()), last(glossary_m.end());
            first != last; ++first)
        std::cerr   << "key: \"" << first->first << "\"; value: \""
                    << first->second << "\"" << std::endl;
#endif
#endif

    return &result->second;
}

/*************************************************************************************************/

struct store_count_same_t : std::binary_function<   const context_frame_t::store_value_type,
                                                    const adobe::attribute_set_t,
                                                    std::size_t>
{
    typedef std::size_t result_type;

    result_type operator() (const context_frame_t::store_value_type& x, const adobe::attribute_set_t& y) const
    {
        const adobe::attribute_set_t& lhs(x.second.first);

        return lhs.has_collisions(y) ? 0 : lhs.count_same(y);
    }
};

/*************************************************************************************************/

template <typename UnaryFunction, typename Range>
struct transform_range
{
    typedef typename boost::range_iterator<Range>::type                             base_iterator;
    typedef typename boost::range_const_iterator<Range>::type                       base_const_iterator;
    typedef typename boost::transform_iterator<UnaryFunction, base_iterator>        iterator;
    typedef typename boost::transform_iterator<UnaryFunction, base_const_iterator>  const_iterator;
    typedef typename std::iterator_traits<base_iterator>::value_type                value_type;
    typedef typename std::iterator_traits<base_iterator>::reference                 reference;
    typedef typename std::iterator_traits<base_iterator>::pointer                   pointer;
    typedef typename std::iterator_traits<base_iterator>::difference_type           difference_type;

    transform_range(Range& range, UnaryFunction f) :
        range_m(range), f_m(f)
    { }

    iterator begin()
        { return iterator(boost::begin(range_m), f_m); }

    iterator end()
        { return iterator(boost::end(range_m), f_m); }
#if 0
    const_iterator begin() const
        { return const_iterator(boost::begin(range_m), f_m); }

    const_iterator end() const
        { return const_iterator(boost::end(range_m), f_m); }
#endif
private:
    Range&          range_m;
    UnaryFunction   f_m;
};

/*************************************************************************************************/

template <typename Range, typename UnaryFunction>
boost::tuple<std::size_t, std::size_t, typename boost::range_iterator<Range>::type>
    count_max_element_tuple(Range& x, UnaryFunction f)
{
    typedef transform_range<UnaryFunction, Range> transform_range_t;

    transform_range_t container(x, f);
    
    typename transform_range_t::iterator max_item = max_element(container);
    if (max_item == container.end()) return boost::make_tuple(1, 0, max_item.base());
    
    return boost::make_tuple(std::count(max_item, container.end(), *max_item),
    	*max_item, max_item.base());
}

/*************************************************************************************************/

context_frame_t::store_iterator context_frame_t::closest_match( store_range_pair_t              range,
                                                                const adobe::attribute_set_t&   searching)
{
    typedef std::iterator_traits<store_iterator>::difference_type   difference_type;

    difference_type range_size(std::distance(boost::begin(range), boost::end(range)));

    if (!range_size) return glossary_m.end();

    boost::tuple<std::size_t, std::size_t, store_iterator> result_tuple =
        count_max_element_tuple(range, boost::bind(store_count_same_t(), _1, boost::cref(searching)));

    if (boost::get<1>(result_tuple) == 0)
        return glossary_m.end();
    else if (boost::get<0>(result_tuple) > 1)
    {
        std::stringstream errstr;

        errstr  << "xstr: ambiguous closest match; found " << static_cast<unsigned int>(boost::get<0>(result_tuple))
                << " glossary entries that matched " << static_cast<unsigned int>(boost::get<1>(result_tuple))
                << " attribute(s)";

#if ADOBE_SERIALIZATION
        errstr  << " while looking for a match to { " << searching << " }";
#endif

        throw std::runtime_error(errstr.str());
    }

    return boost::get<2>(result_tuple);
}

/*************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

adobe::token_range_t xml_xstr_store(const adobe::token_range_t&     entire_element_range,
                                    const adobe::token_range_t&     name,
                                    const adobe::attribute_set_t&   attribute_set,
                                    const adobe::token_range_t&     value)
{
    if (token_range_equal(name, *xstr_tag_g))
    {
#if 0 && ADOBE_DOING_SERIALIZATION
        std::cerr << "xml_xstr_store \"" << entire_element_range << "\"" << std::endl;
#endif

        adobe::token_range_t id(attribute_set[*attribute_id_g]);

        return top_frame().store(id, attribute_set, value)->second;
    }

    return xml_element_echo(entire_element_range, name, attribute_set, value);
}

/****************************************************************************************************/

adobe::token_range_t xml_xstr_lookup(   const adobe::token_range_t&     entire_element_range,
                                        const adobe::token_range_t&     name,
                                        const adobe::attribute_set_t&   attribute_set,
                                        const adobe::token_range_t&     value)
{
    if (token_range_equal(name, *xstr_tag_g))
    {
#if 0 && ADOBE_DOING_SERIALIZATION
        std::cerr << "xml_xstr_lookup \"" << entire_element_range << "\"..." << std::endl;
#endif

        context_frame_t&                context(top_frame());
        adobe::attribute_set_t          merged(attribute_set.merge(context.attribute_set_m));
        adobe::token_range_t            id(merged[*attribute_id_g]);
        context_frame_t::store_iterator closest(context.closest_match(context.range_for_key(id), merged));
        context_frame_t::store_iterator last(context.glossary_m.end());

        if (closest == last)
        {
#if 0 && ADOBE_DOING_SERIALIZATION
            std::cerr << "lookup failed" << std::endl;
#endif

#ifndef NDEBUG
            return top_frame().store(id, attribute_set, value, true)->second;
#else
            return value;
#endif
        }

#if 0 && ADOBE_DOING_SERIALIZATION
            std::cerr << "lookup found { " << closest->second.first << " }" << std::endl;
#endif

        return closest->second.second;
    }

    return xml_element_echo(entire_element_range, name, attribute_set, value);
}

/*************************************************************************************************/

} // namespace implementation

/*************************************************************************************************/

#if 0
#pragma mark -
#endif

/*************************************************************************************************/
#ifndef NDEBUG

void xstring_clear_glossary()
{
    adobe::implementation::context_frame_t::store_t& glossary
        (adobe::implementation::top_frame().glossary_m);

    glossary.erase(glossary.begin(), glossary.end());
}

#endif
/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

namespace anonymous_xstring_cpp { // can't instantiate templates on types from real anonymous

/****************************************************************************************************/

struct marker_t
{
    marker_t(   const adobe::attribute_set_t&   attribute_set,
                const adobe::token_range_t&     value) :
    attribute_set_m(attribute_set), value_m(value)
    { }

    adobe::attribute_set_t  attribute_set_m;
    adobe::token_range_t    value_m;
};

/****************************************************************************************************/

bool operator == (const marker_t& x, const marker_t& y)
{
    // value is noncritical data

    return x.attribute_set_m == y.attribute_set_m;
}

/****************************************************************************************************/

} // namespace anonymous_xstring_cpp

/****************************************************************************************************/

using namespace anonymous_xstring_cpp;

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct replacement_engine_t
{
    typedef std::vector<marker_t> marker_set_t;

    replacement_engine_t(const adobe::name_t& id) :
        score_m(-1), xstring_id_m(adobe::static_token_range(id.c_str()))
    { }

    replacement_engine_t(const std::string& xstr) :
        score_m(-1)
    {
        adobe::make_xml_parser( reinterpret_cast<uchar_ptr_t>(&xstr[0]),
                                reinterpret_cast<uchar_ptr_t>(&xstr[0]) + xstr.size(),
								adobe::line_position_t("replacement_engine_t"),
								adobe::implementation::xstring_preorder_predicate,
								boost::bind(&replacement_engine_t::xstr_id_harvest,
                                            boost::ref(*this), _1, _2, _3, _4),
								adobe::implementation::null_output_t())

        .parse_content();
    }

    void add_marker(const std::string& marker)
    {
        adobe::make_xml_parser( reinterpret_cast<uchar_ptr_t>(&marker[0]),
                                reinterpret_cast<uchar_ptr_t>(&marker[0]) + marker.size(),
								adobe::line_position_t("add_marker"),
								adobe::implementation::xstring_preorder_predicate,
								boost::bind(&replacement_engine_t::marker_parse,
                                            boost::ref(*this), _1, _2, _3, _4),
								adobe::implementation::null_output_t())

        .parse_content();
    }

    std::string run()
    {
        implementation::context_frame_t::store_range_pair_t range(implementation::top_frame().range_for_key(xstring_id_m));
#ifndef NDEBUG
        std::iterator_traits<implementation::context_frame_t::store_iterator>::difference_type range_size(std::distance(range.first, range.second));
#endif

        for (; range.first != range.second; ++range.first)
        {
            int                     score(0);
            const token_range_t&    cur_token_range(range.first->second.second);
            uchar_ptr_t             first(cur_token_range.first);
            uchar_ptr_t             last(cur_token_range.second);
            std::string             temp_result;

            adobe::make_xml_parser( first, last,
									adobe::line_position_t("replacement_engine_t::run"),
									adobe::implementation::xstring_preorder_predicate,
									boost::bind(&replacement_engine_t::candidate_parse,
                                                boost::ref(*this), _1, _2, _3, _4, boost::ref(score)),
									std::back_inserter(temp_result))

            .parse_content();

            if (score > score_m)
            {
                result_m = temp_result;

                score_m = score;
            }
        }

        if (score_m < 0)
        {
#ifndef NDEBUG
            std::stringstream err;

            err << "xstring: string replacement: no viable candidates of " << static_cast<unsigned int>(range_size) << " possibilities" << std::endl;

            throw std::runtime_error(err.str());
#else
            throw std::runtime_error("xstring: string replacement: no viable candidates");
#endif
        }
        return result_m;
    }

private:
    adobe::token_range_t xstr_id_harvest(   const adobe::token_range_t&     /*entire_element_range*/,
                                            const adobe::token_range_t&     name,
                                            const adobe::attribute_set_t&   attribute_set,
                                            const adobe::token_range_t&     /*value*/)
    {
        if (token_range_equal(name, *xstr_tag_g) && !std::distance(boost::begin(xstring_id_m),boost::begin(xstring_id_m)))
            xstring_id_m = attribute_set[*attribute_id_g];

        return adobe::token_range_t();
    }

    adobe::token_range_t marker_parse(  const adobe::token_range_t&     entire_element_range,
                                        const adobe::token_range_t&     name,
                                        const adobe::attribute_set_t&   attribute_set,
                                        const adobe::token_range_t&     value)
    {
        if (token_range_equal(name, *marker_tag_g))
        {
            marker_set_m.push_back(marker_t(attribute_set, value));

            return adobe::token_range_t();
        }
        else
        {
#if 0
            std::cerr   << " Echoing ``" << entire_element_range
                        << "``, value: ``" << value << "``" << std::endl;
#endif

            return xml_element_echo(entire_element_range, name, attribute_set, value);
        }
    }

    adobe::token_range_t candidate_parse(   const adobe::token_range_t&     /*entire_element_range*/,
                                            const adobe::token_range_t&     name,
                                            const adobe::attribute_set_t&   attribute_set,
                                            const adobe::token_range_t&     /*value*/,
                                            int&                            score)
    {
        if (token_range_equal(name, *marker_tag_g))
        {
            if (score == -1) return adobe::token_range_t(); // short-circuit on error

            marker_set_t::iterator result(adobe::find(marker_set_m, marker_t(attribute_set, adobe::token_range_t())));

            // marker must have a replacement

            if (result != marker_set_m.end())
            {
                ++score;

                return result->value_m;
            }
            else
                score = -1; // error state
        }
#if 0
        else
        {
            return std::distance(boost::begin(name), boost::end(name)) ? xml_element_echo(entire_element_range, name, attribute_set, value) : value;
        }
#endif

        return adobe::token_range_t();
    }

    marker_set_t            marker_set_m;
    int                     score_m;
    std::string             result_m;
    adobe::token_range_t    xstring_id_m;
};

/****************************************************************************************************/

std::string xstring_replace(const std::string& xstr, const std::string& marker)
{
    replacement_engine_t engine(xstr);

    engine.add_marker(marker);

    return engine.run();
}

/****************************************************************************************************/

std::string xstring_replace(const std::string& xstr, const std::string* first, const std::string* last)
{
    replacement_engine_t engine(xstr);

    for (; first != last; ++first)
        engine.add_marker(*first);

    return engine.run();
}

/****************************************************************************************************/

std::string xstring_replace(const adobe::name_t& xstr_id, const std::string& marker)
{
    replacement_engine_t engine(xstr_id);

    engine.add_marker(marker);

    return engine.run();
}

/****************************************************************************************************/

std::string xstring_replace(const adobe::name_t& xstr_id, const std::string* first, const std::string* last)
{
    replacement_engine_t engine(xstr_id);

    for (; first != last; ++first)
        engine.add_marker(*first);

    return engine.run();
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

ADOBE_ONCE_DEFINITION(xstr_once, init_xstr_once)

/*************************************************************************************************/
