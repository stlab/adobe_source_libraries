/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_XML_PARSER_HPP
#define ADOBE_XML_PARSER_HPP

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/any_regular.hpp>
#include <adobe/algorithm/set.hpp>
#include <adobe/istream.hpp>
#include <adobe/array.hpp>
#include <adobe/copy_on_write.hpp>
#include <adobe/name.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/string.hpp>
#include <adobe/implementation/xml_lex.hpp>
#include <adobe/implementation/xml_token.hpp>
#include <adobe/implementation/parser_shared.hpp>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/operators.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <utility>
#include <istream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <list>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/

// NOTE (fbrereto) : Class declaration for the documentation is in xml_parser.dox
struct attribute_set_t : public boost::equality_comparable<attribute_set_t>
{
    typedef token_range_t                key_type;
    typedef token_range_t                mapped_type;
    typedef std::pair<key_type, mapped_type>    value_type;
    typedef std::vector<value_type>             set_type;
    typedef set_type::size_type                 size_type;
    typedef set_type::const_iterator            const_iterator;
    typedef const_iterator                      iterator;

    /*!
    Support function object for the attribute_set_t class. Provides strict
    weak ordering functionality for the elements of a attribute_set_t,
    comparing both the key and the value of a single
    attribute_set_t::value_type.
    */
    struct less_t : std::binary_function<value_type, value_type, bool>
    {
        bool operator () (const value_type& x, const value_type& y) const
        {
            return  token_range_less(x.first, y.first) ||
                    (!token_range_less(y.first, x.first) &&
                    token_range_less(x.second, y.second));
        }
    };

    /*!
    Support function object for the attribute_set_t class. Provides strict
    weak ordering functionality for the elements of a attribute_set_t,
    comparing ONLY the key of a single attribute_set_t::value_type.
    */
    struct less_key_only_t : std::binary_function<value_type, value_type, bool>
    {
        bool operator () (const value_type& x, const value_type& y) const
        {
            return token_range_less(x.first, y.first);
        }
    };

    /*!
        Same logical result as std::lower_bound as it applies to attribute_set_t.
        Note that the value of the attribute is not used in the search; only the
        key is looked up.

        \param attribute the attribute whose key is to be searched for in the attribute_set_t
        \param result the resultant position to which the attribute key fits

        \return true if the attribute key exists within the attribute_set_t.
        false otherwise.
    */
    bool lower_bound(const value_type& attribute, set_type::iterator& result)
    {
        result = adobe::lower_bound(set_m.write(), attribute, less_key_only_t());

        return  result != set_m.write().end() &&
                token_range_equal(result->first, attribute.first);
    }

    /*!
        Same logical result as std::lower_bound as it applies to attribute_set_t.
        For this particular implementation only the attribute key is necessary as
        a parameter.

        \param key the key that is to be searched for in the attribute_set_t
        \param result the resultant position to which the key fits

        \return true if an attribute with this key exists within the attribute_set_t.
        false otherwise.
    */
    bool lower_bound(const key_type& key, set_type::iterator& result)
    { return lower_bound(value_type(key, mapped_type()), result); }

    /*!
        const-equivalent of lower_bound for attribute_set_t.
    */
    bool lower_bound(const value_type& attribute, set_type::const_iterator& result) const
    {
        result = adobe::lower_bound(*set_m, attribute, less_key_only_t());

        return  result != set_m->end() &&
                token_range_equal(result->first, attribute.first);
    }

    /*!
        const-equivalent of lower_bound for attribute_set_t.
    */
    bool lower_bound(const key_type& key, set_type::const_iterator& result) const
    { return lower_bound(value_type(key, mapped_type()), result); }

    /*!
        Fetches an unmodifiable value from an attribute_set_t stored
        under a given key.

        \param key the key under which the corresponding value should be found

        \return the value if one exists; otherwise the NULL-token (See
        token_range_t).
    */
    mapped_type operator [] (const key_type& key) const
    {
        set_type::const_iterator result;

        if (lower_bound(key, result))
            return result->second;

        return mapped_type();
    }

    /*!
        This merges two attribute sets together and places the union into
        dst. In the case where both attribute sets have an element with the
        same name, the value in the union will be from *this. The values are
        not compared when performing the union. This means the "override"
        attribute set should be the first source when you call this
        function.
    
        \param other_set the second attribute_set_t with which this one will be merged.

        \return an attribute_set_t that is the result of merging this
        attribute_set_t with the one passed in. This attribute_set_t is
        left unchanged.
    */
    attribute_set_t merge(const attribute_set_t& other_set) const
    {

        attribute_set_t merged;

        adobe::set_union(*set_m, *other_set.set_m, std::back_inserter(merged.set_m.write()), less_key_only_t());

        return merged;
    }

    /*!
        Inserts a key-value pair into the current attribute set. If there
        are more than one references to the internal instance of this
        attribute set, then a copy will be spun off before the insertion
        is made.

        In the case when a value is already stored under the key specified
        in the attribute to be inserted, that value is replaced.

        \param attribute the key-value pair to be inserted.
    */
    void insert(const value_type& attribute)
    {
        set_type::iterator result;

        if (lower_bound(attribute, result))
            result->second = attribute.second;
        else
            set_m.write().insert(result, attribute);
    }

    /*!
        Inserts a range of key-value pairs into the current attribute set.
        Equivalent to invoking insert() over each element in the range.

        \pre
            \c last must be reachable from \c first.

        \param first A pointer to the first key-value pair in the range.
        \param last A pointer to one-past the last key-value pair in the range.
    */
    template <typename I> // I models InputIterator
    inline void insert(I first, I last)
    { for (; first != last; ++first) insert(*first); }

    /*!
        Inserts a key-value pair into the current attribute set. This is
        a helper implementation, allowing the client to specify the key
        and the value to be inserted as separate parameters.

        \param key the key under which the value is to be inserted
        \param value the value to be inserted into the attribute set
    */
    inline void insert(const key_type& key, const mapped_type& value)
    { insert(value_type(key, value)); }

    /*!
        Counts the number of attributes two attribute sets hold in
        similarity. When making the comparison the value of the attributes
        can be optionally omitted.

        \param other_set the other attribute set used for comparison
        \param mapped_matters toggles whether or not to consider the values
        found under the respective keys. If false, only the existence of the
        keys are required for the two sets to be considered 'sharing' that key.
    */
    std::size_t count_same(const attribute_set_t& other_set, bool mapped_matters = true) const;

    /*!
        A collision is defined as a key existing in both attribute sets but
        having different values. This routine searches two attribute sets
        looking to see if any collisions exist between them.

        This routine is no slower (and is often faster than) the expression
        <code>count_collisions() != 0</code>.

        \param other_set the other attribute set used for comparison

        \return Whether or not there is at least one collision between the
        two attribute sets.
    */
    bool        has_collisions(const attribute_set_t& other_set) const;

    /*!
        A collision is defined as a key existing in both attribute sets but
        having different values. This routine searches two attribute sets
        and counts the number of collisions that exist between them.

        \param other_set the other attribute set used for comparison

        \return The number of collisions that exist between two sets.
    */
    std::size_t count_collisions(const attribute_set_t& other_set) const;

    /*!
        \return Specifies whether or not this attribute set is empty
    */
    inline bool empty() const
        { return set_m->empty(); }

    /*!
        \return Specifies the number of key-value pairs stored in this
        attribute set
    */
    inline size_type size() const
        { return set_m->size(); }

    /*!
        \return An unmodifiable (but dereferenceable) iterator to the
        first key-value pair in the attribute set
    */
    const_iterator begin() const { return set_m->begin(); }

    /*!
        \return An iterator to one-past-the last key-value pair in
        the attribute set
    */
    const_iterator end() const { return set_m->end(); }

    /*!
        Erases the entire contents of the attribute set.

        \post
            <code>empty() == true</code>
    */
    void clear() { set_m.write().clear(); }

private:
    friend bool operator == (const attribute_set_t& x, const attribute_set_t& y);
    friend std::ostream& operator << (std::ostream& s, const attribute_set_t& attribute_set);

    copy_on_write<set_type> set_m;
};

/*************************************************************************************************/

/*!
    \relates attribute_set_t

    Defines equality comparisons between two attribute sets. Equivalent to calling count_same over
    the attribute sets and arriving at the size of both sets (which, by implication, must be the
    same.)

    \param x the first attribute set
    \param y the second attribute set

    \return \c true if the two attribute sets are equal. \c false otherwise.
*/
inline bool operator == (const attribute_set_t& x, const attribute_set_t& y)
{
    return x.set_m->size() == y.set_m->size() && x.count_same(y) == x.set_m->size();
}

/*************************************************************************************************/

/*!
    \relates attribute_set_t

    Defines serialization functionality for an attribute set. Output format is similar to the
    format required for attribute sets in xml documents.

    \param s the stream to receive the output
    \param attribute_set the attribute set to be serialized to the stream

    \return the output stream
*/
inline std::ostream& operator << (std::ostream& s, const attribute_set_t& attribute_set)
{
    attribute_set_t::set_type::const_iterator    first(attribute_set.set_m->begin());
    attribute_set_t::set_type::const_iterator    last(attribute_set.set_m->end());
    bool                                                not_first(false);

    for (; first != last; ++first)
    {
        if (not_first)
            s << " ";
        else
            not_first = true;

        adobe::copy(first->first, std::ostream_iterator<char>(s));

        s << "='";

        adobe::copy(first->second, std::ostream_iterator<char>(s));

        s << "'";
    }

    return s;
}

/*************************************************************************************************/

inline std::size_t attribute_set_t::count_same(const attribute_set_t& other_set, bool mapped_matters) const
{
    std::size_t result(0);

    if (mapped_matters)
        result = adobe::set_intersection(   *set_m, *other_set.set_m,
                                            counting_output_iterator(),
                                            less_t())
                                    .count();
    else
        result = adobe::set_intersection(   *set_m, *other_set.set_m,
                                            counting_output_iterator(),
                                            less_key_only_t())
                                    .count();

    #if 0
        std::cerr   << "    count_same:\n"
                    << "          orig: " << *this << "\n"
                    << "          test: " << other_set << "\n"
                    << "        result: " << result << std::endl;
    #endif

    return result;
}

/*************************************************************************************************/

inline bool attribute_set_t::has_collisions(const attribute_set_t& other_set) const
{
    attribute_set_t::set_type::const_iterator    first(set_m->begin());
    attribute_set_t::set_type::const_iterator    last(set_m->end());

    for (; first != last; ++first)
    {
        set_type::const_iterator result;

        if (other_set.lower_bound(*first, result) && !token_range_equal(result->second, first->second))
            return true;
    }

    return false;
}

/*************************************************************************************************/

inline std::size_t attribute_set_t::count_collisions(const attribute_set_t& other_set) const
{
    attribute_set_t::set_type::const_iterator    first(set_m->begin());
    attribute_set_t::set_type::const_iterator    last(set_m->end());
    std::size_t                                         collision_count(0);

    for (; first != last; ++first)
    {
        set_type::const_iterator result;

        if (other_set.lower_bound(*first, result) && result->second != first->second)
            ++collision_count;
    }

    return collision_count;
}

/*************************************************************************************************/

// REVISIT (sparent) : Extra typedef just for the doxygen tool.

typedef token_range_t (implementation_xml_element_proc_t)(
                        const token_range_t&     entire_element_range,
                        const token_range_t&     name,
                        const attribute_set_t&   attribute_set,
                        const token_range_t&     value);

typedef boost::function<implementation_xml_element_proc_t> xml_element_proc_t;

/*************************************************************************************************/

// NOTE (fbrereto) : Class declaration for the documentation is in xml_parser.dox
template <typename O> // O models OutputIterator
class xml_parser_t : public boost::noncopyable
{
public:
    typedef xml_element_proc_t                              callback_proc_t;
    typedef boost::function<bool (const token_range_t&)>    preorder_predicate_t;
    typedef xml_lex_t::token_type                           token_type;

    xml_parser_t(   uchar_ptr_t             first,
					uchar_ptr_t             last,
					const line_position_t&  position,
					preorder_predicate_t    predicate,
					callback_proc_t         callback,
					O                       output) :
        pred_m(predicate),
        callback_m(callback),
        output_m(output),
        token_stream_m(first, last, position),
        preorder_mode_m(false)
    { }

    xml_parser_t(const xml_parser_t& rhs) :
        pred_m(rhs.pred_m),
        callback_m(rhs.callback_m),
        output_m(rhs.output_m),
        token_stream_m(rhs.token_stream_m),
        preorder_mode_m(rhs.preorder_mode_m)
    { }

    xml_parser_t& operator = (const xml_parser_t& rhs)
    {
        pred_m = rhs.pred_m;
        callback_m = rhs.callback_m;
        output_m = rhs.output_m;
        token_stream_m = rhs.token_stream_m;
        preorder_mode_m = rhs.preorder_mode_m;

        return *this;
    }

    virtual ~xml_parser_t()
    { }

    const line_position_t& next_position()
        { return token_stream_m.next_position(); }

	/*!
        Allows the client to specify a different preorder predicate after object instantiation

        \param[in] pred predicate that indicates whether the client wants a given element to be parsed pre-order or in-order
	*/
    void set_preorder_predicate(preorder_predicate_t pred)
    { pred_m = pred; }

	/*!
    Parses the content range as a sequence of xml elements. Each element encountered in the
    content range is processed by the application. Character data between top-level elements
    in the content range is ignored by the parser and is not processed.

    \example
    \par
        Consider this content range:
	\verbatim
<top-level type="simple">element 1</top-level>

these characters are ignored

<top-level type="complex">element 2<embedded/></top-level>\endverbatim
	\par
		Parsing this content range as an element sequence yields two top-level elements, one of which
		contains embedded elements. Each top-level element is processed by the application according
		to the application's preorder predicate and content callback functions. The content between
		the two top-level elements is ignored.
	\par
		Element sequences can be useful for preference sets or other simple data that do not need
		significant structure (e.g. as a full document would contain).
	*/
    void parse_element_sequence();

	/*!
	Parses the content range as the content of an xml element.
	\par
	This function is most useful when invoking a sub-parser within an application's content
	callback function.
	
	\example
	\par
		Consider this document:
	\verbatim
<?xml encoding="UTF-8" version="1.0" ?>
<root>
    <content>sample document content</content>
</root>\endverbatim
	\par
		An application that wished to enforce the string structure of the document could
		use a content callback like the following to ensure that the root element of the
		document is "root", while also processing the content of the root element.
	\code
token_range_t top_level_callback(
                                  const token_range_t&     entire_element_range,
                                  const token_range_t&     name,
                                  const attribute_set_t&   attribute_set,
                                  const token_range_t&     value)
{
    assert(token_range_equal(name, static_token_range("root")));
    
    make_xml_parser(value.first, value.second,
                           line_position_t("top_level_callback"),
                           always_true<token_range_t>(),
                           root_callback,
                           null_output_t())
        .parse_content();
    
    return token_range_t();
}
	\endcode
	\par
	In this example, the top level callback performs a sanity check that the element
	it encounters is the tag "root". It then creates a new parser to process the content
	of the root element.
	\par
	A less strict application could choose to use a single callback function for all elements
	or annotate a more complex data structure as the document is processed.
	*/
	void parse_content();
	
	/*!
	Parses the content range as a well-formed xml document.
	*/
	void parse_document();

/*
    REVISIT (sparent) : We should provide a protected call to get the token stream and allow
    subclasses to access it directly - but for now we'll stick with the law of Demiter.
*/

protected:
    const token_type& get_token()
        { return token_stream_m.get(); }
    void putback()
        { token_stream_m.putback(); }

    bool is_token(xml_lex_token_set_t name, token_range_t& value);
    bool is_token(xml_lex_token_set_t name);
    void require_token(xml_lex_token_set_t name, token_range_t& value);
    void require_token(xml_lex_token_set_t name);

    /* REVISIT (sparent) : Should these be const? And is there a way to specify the class to throw? */

    void throw_exception(const char* error_string)
        { throw_parser_exception(error_string, next_position()); }
    void throw_exception(xml_lex_token_set_t found, xml_lex_token_set_t expected)
        { throw_parser_exception(token_to_string(found), token_to_string(expected), next_position()); }

    bool is_element(token_range_t& element);
    bool is_content(token_range_t& element);
    bool is_e_tag(token_range_t& name, token_range_t& close_tag);
    bool is_attribute_set(attribute_set_t& attribute_set);
    bool is_attribute(token_range_t& name, token_range_t& value);
	bool is_prolog();
	bool is_bom(token_range_t& bom);
	bool is_xml_decl(token_range_t& xml_decl);

    void    content_callback(   token_range_t&           result_element,
                                const token_range_t&     old_element,
                                const token_range_t&     start_tag,
                                const attribute_set_t    attribute_set,
                                const token_range_t&     content,
                                bool                            preorder_parent);

    preorder_predicate_t    pred_m;
    callback_proc_t         callback_m;
    O                       output_m;

private:
    xml_lex_t               token_stream_m;
    bool                    preorder_mode_m;
};

/*************************************************************************************************/

inline token_range_t xml_element_echo(   const token_range_t&     entire_element_range,
                                                const token_range_t&     /*name*/,
                                                const attribute_set_t&   /*attribute_set*/,
                                                const token_range_t&     /*value*/)
    { return entire_element_range; }

/*************************************************************************************************/

inline token_range_t xml_element_strip(  const token_range_t&     /*entire_element_range*/,
                                                const token_range_t&     /*name*/,
                                                const attribute_set_t&   /*attribute_set*/,
                                                const token_range_t&     value)
    { return value; }

/*************************************************************************************************/

inline token_range_t xml_element_linefeed(   const token_range_t&     /*entire_element_range*/,
                                                    const token_range_t&     name,
                                                    const attribute_set_t&   attribute_set,
                                                    const token_range_t&     value)
{
    if (token_range_equal(name, static_token_range("br")) &&
        attribute_set.empty() &&
        boost::size(value) == 0)
    {
#if ADOBE_PLATFORM_WIN
        return static_token_range("&cr;&lf;");
#elif ADOBE_PLATFORM_MAC
        return static_token_range("&cr;");
#elif   ADOBE_PLATFORM_UNIX || ADOBE_PLATFORM_LINUX || ADOBE_PLATFORM_BSD || ADOBE_PLATFORM_SOLARIS ||\
        ADOBE_PLATFORM_IRIX || ADOBE_PLATFORM_HPUX || ADOBE_PLATFORM_CYGWIN || ADOBE_PLATFORM_AIX
        return static_token_range("&lf;");
#else
    #error "Line ending for platform unknown - please configure and report the results to stlab.adobe.com"
#endif
    }

    return value;
}

/*************************************************************************************************/

namespace implementation {

/*************************************************************************************************/

token_range_t transform_reference(const token_range_t& reference);

/*************************************************************************************************/

} // namespace implementation

/*************************************************************************************************/

template <typename O> // O models OutputIterator
bool xml_parser_t<O>::is_token(xml_lex_token_set_t token_name, token_range_t& token_range)
{
    const token_type& result(get_token());

    if (result.enum_m == token_name)
    {
        token_range = result.range_m;

        return true;
    }

    putback();

    return false;
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
bool xml_parser_t<O>::is_token(xml_lex_token_set_t token_name)
{
    const token_type& result(get_token());

    if (result.enum_m == token_name)
        return true;

    putback();

    return false;
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
void xml_parser_t<O>::require_token(xml_lex_token_set_t token_name, token_range_t& token_range)
{
    const token_type& result(get_token());

    if (result.enum_m != token_name)
        throw_exception(result.enum_m, token_name);

    token_range = result.range_m;
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
void xml_parser_t<O>::require_token(xml_lex_token_set_t token_name)
{
    const token_type& result(get_token());

    if (result.enum_m != token_name)
        throw_exception(result.enum_m, token_name);
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
void xml_parser_t<O>::content_callback( token_range_t&           result_element,
										const token_range_t&     old_element,
										const token_range_t&     start_tag,
										const attribute_set_t    attribute_set,
										const token_range_t&     content,
										bool                            preorder_parent)
{
    if (preorder_parent)
    {
        // if we are in preorder mode and we are the preorder_parent,
        // we send the content to the client callback function.
        // We get back a single token_range, which we then parse all
        // over again in a content parser all its own.

        token_range_t new_content(callback_m(old_element, start_tag, attribute_set, content));

        if (old_element == new_content)
        {
            // In the case when the new content is the same as the old element,
            // the user has opted to echo the element to the output unchanged.

            adobe::copy(old_element, output_m);
        }
        else
        {
            // otherwise we need to parse the new content before we can move on to
            // the rest of the parse. The new parser has the same predicate and
            // output iterator as this one

            xml_parser_t<O>( new_content.first, new_content.second,
									next_position(), pred_m, callback_m, output_m).parse_content();
        }

        // once the token_range from the client has been parsed, we can turn off
        // preorder mode and resume parsing the original token stream from where we
        // left off.

        preorder_mode_m = false; // only the preorder_parent can turn off preorder mode
    }
    else
    {
        // in the case we are in preorder mode but we are not the initiator of
        // the mode, we are within the context of another preorder parse. In
        // this case we use the entire contents of the element as the token range
        // and hand it back as the return value of this function.

        result_element = old_element;
    }
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
bool xml_parser_t<O>::is_element(token_range_t& element)
{
    element = token_range_t();

    attribute_set_t attribute_set;

    token_range_t   open_tag;
    token_range_t   close_tag;

    if (!is_token(xml_token_open_tag_k, open_tag)) return false;

    token_range_t   start_tag;
    token_range_t   end_tag;

    require_token(xml_token_name_k, start_tag);

    bool preorder_parent(false); // explained below

    // Preorder mode is a state for the entire parser. In this state the
    // client processing callback is never called until the end of the
    // current element is found. This precludes the processing of elements
    // and other entities nested within this element from being handled until
    // this containing element is processed. This is useful in the case when 
    // the content of the element could potentially be replaced, in which
    // case processing the nested elements first would be a moot point.

    if (!preorder_mode_m && pred_m)
    {
        // preorder mode is only set when the predicate is defined and
        // returns true for the start_tag of this element.

        preorder_mode_m = pred_m(start_tag);


        // preorder_parent is used to denote which frame in the stack began
        // the preorder traversal, as it is this frame alone that can turn
        // it back off again.

        preorder_parent = preorder_mode_m;
    }

    is_attribute_set(attribute_set);

    if (is_token(xml_token_slash_close_tag_k, close_tag))
    {
        if (preorder_mode_m)
        {
            content_callback(   element,
                                token_range_t(open_tag.first, close_tag.second),
                                start_tag,
                                attribute_set,
                                token_range_t(),
                                preorder_parent);
        }
        else
        {
            // in the case when we are not in preorder mode at all, we pass the element
            // to the client callback and output the token_range we receive back.

            token_range_t result(callback_m( token_range_t(open_tag.first, close_tag.second),
                                                    start_tag,
                                                    attribute_set,
                                                    token_range_t()));

            adobe::copy(result, output_m);
        }

        return true;
    }

    token_range_t close_of_open_tag;

    require_token(xml_token_close_tag_k, close_of_open_tag);

    token_range_t content;

    // In the case of inorder parsing we want to output the tags
    // as we see them; in this case we need to output the opening
    // tag before we can go on to the content parsing.

    if (!preorder_mode_m)
        std::copy(open_tag.first, close_of_open_tag.second, output_m);

    if (!is_content(content))
        throw std::runtime_error("Content expected but not found.");
    
    if (!is_e_tag(end_tag, close_tag))
        throw std::runtime_error("End tag expected but not found.");

    if (!token_range_equal(start_tag, end_tag))
        throw std::runtime_error("Start tag and end tag do not have the same name.");

    if (!preorder_mode_m)
    {
        // in the case when we are not in preorder mode
        // we output the content we have immediately,
        // then we need to output the closing tag before
        // we can go on to the rest of the parse.

        adobe::copy(content, output_m);
        adobe::copy(token_range_t(end_tag.first - 2, end_tag.second + 1), output_m);
    }
    else
    {
        // In this instance we are continuing a preorder parse...

        content_callback(   element,
                            token_range_t(open_tag.first, close_tag.second),
                            start_tag,
                            attribute_set,
                            content,
                            preorder_parent);
    }

    return true;
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
bool xml_parser_t<O>::is_content(token_range_t& content)
{
    content = token_range_t();

    token_range_t char_data;

    // NOTE (fbrereto) :    The content parser can never initiate a preorder mode.
    //                      It can only be initiated by the parsing of a preorder
    //                      element, which isn't handled here. So for the content
    //                      parse we are either in preorder mode or not; we need
    //                      not worry about managing it.

    if (is_token(xml_token_char_data_k, char_data))
    {
        // in the case when we are in preorder mode, we are part of a nested
        // content, and we want to use this beginning char_data token as the
        // start of the overall content token_range.

        if (preorder_mode_m)
            { content = char_data; }

        // in the case when we are not in preorder mode this range of char_data
        // needs to be sent directly to the output.

        else
            { adobe::copy(char_data, output_m); }
    }

    while (true)
    {
        token_range_t result;

        if (is_token(xml_token_reference_k, result))
        {
            if (boost::size(result))
            {
                if (preorder_mode_m)
                {
                    // Again, if we're in preorder mode we're not outputting
                    // but extending (possibly even starting, too) the token_range
                    // for the preorder element.

                    if (!content.first) content.first = result.first;

                    content.second = result.second;
                }
                else
                {
                    // if we're not in preorder mode, we pass the element's
                    // reference-transformed token_range result directly to
                    // the output.

                    adobe::copy(implementation::transform_reference(result), output_m);
                }
            }
        }
        else if (is_element(result))
        {
            if (boost::size(result))
            {
                if (preorder_mode_m)
                {
                    // Again, if we're in preorder mode we're not outputting
                    // but extending (possibly even starting, too) the token_range
                    // for the preorder element.

                    if (!content.first) content.first = result.first;

                    content.second = result.second;
                }
                else
                {
                    // if we're not in preorder mode, we pass the element's
                    // token_range result directly to the output.

                    adobe::copy(result, output_m);
                }
            }
        }
		else if (is_token(xml_token_comment_k, result))
		{
			// Comments are not parsed by any client functions.
			// They are merely ignored by the parser.

			// REVISIT eberdahl - Because some clients may want to
			// handle comments, we may want to extend the client
			// callback system to permit a comment callback.
		}
        else
            { break; }

        if (is_token(xml_token_char_data_k, char_data))
        {
            // if we find more char_data at the end of the content, we
            // either extent the preorder content data or we output
            // the contents of the char_data directly to the output (in
            // fullorder mode).

            if (preorder_mode_m)
                { content.second = char_data.second; }
            else
                { adobe::copy(char_data, output_m); }
        }
    }

    return true;
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
bool xml_parser_t<O>::is_e_tag(token_range_t& name, token_range_t& close_tag)
{
    if (!is_token(xml_token_open_slash_tag_k)) return false;

    require_token(xml_token_name_k, name);

    require_token(xml_token_close_tag_k, close_tag);

    return true;
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
bool xml_parser_t<O>::is_attribute_set(attribute_set_t& attribute_set)
{
    token_range_t att_name;
    token_range_t att_value;

    while (is_attribute(att_name, att_value))
        attribute_set.insert(att_name, att_value);

    return true;
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
bool xml_parser_t<O>::is_prolog()
{
	token_range_t bom;
	token_range_t xml_decl;

	if (is_bom(bom))
	{
		// REVISIT eberdahl 2006 Jun 18 - sanity check the bom
	}
	
	if (is_xml_decl(xml_decl))
	{
		// REVISIT eberdahl 2006 Jun 18 - sanity check the encoding
		// of the XMLDecl
		
		return true;
	}
	
	return false;
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
bool xml_parser_t<O>::is_bom(token_range_t& bom)
{
	const token_range_t utf8_bom = static_token_range("\xEF\xBB\xBF");
	const token_range_t utf16_be_bom = static_token_range("\xFE\xFF");
	const token_range_t utf16_le_bom = static_token_range("\xFF\xFE");

	bool result = false;
	
	// whitespace skipping should be off when sniffing for a bom
	token_stream_m.set_skip_white_space(false);

	if (is_token(xml_token_char_data_k, bom))
	{
		if (boost::size(utf8_bom) <= boost::size(bom) &&
			adobe::equal(utf8_bom, bom.first))
		{
			bom.second = bom.first;
			std::advance(bom.second, boost::size(utf8_bom));

			result = true;
		}
		else if (boost::size(utf16_be_bom) <= boost::size(bom) &&
				 adobe::equal(utf16_be_bom, bom.first))
		{
			// it's a bom, but it's not a format the parser supports
			throw_exception("utf16be bom encountered; xml_parser_t only supports utf8 encoding");			
		}
		else if (boost::size(utf16_le_bom) <= boost::size(bom) &&
				 adobe::equal(utf16_le_bom, bom.first))
		{
			// it's a bom, but it's not a format the parser supports
			throw_exception("utf16le bom encountered; xml_parser_t only supports utf8 encoding");			
		}
	}

	token_stream_m.set_skip_white_space(true);

	return result;
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
bool xml_parser_t<O>::is_xml_decl(token_range_t& xml_decl)
{
	if (is_token(xml_token_processing_instruction_k, xml_decl))
	{
		// REVISIT eberdahl 2006 Jun 18 - sanity check that the PI
		// encountered is, in fact, targeted at the xml application
		
		return true;
	}
	
	return false;
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
bool xml_parser_t<O>::is_attribute(token_range_t& name, token_range_t& value)
{
    if (is_token(xml_token_name_k, name))
    {
        require_token(xml_token_equals_k);

        require_token(xml_token_att_value_k, value);

        return true;
    }

    return false;
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
void xml_parser_t<O>::parse_element_sequence()
{
    assert(callback_m);
	
    token_range_t dummy;

	token_stream_m.set_skip_white_space(false);

    while (is_element(dummy))
        is_token(xml_token_char_data_k);
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
void xml_parser_t<O>::parse_content()
{
    token_range_t content;

	token_stream_m.set_skip_white_space(false);

    while (true)
    {
        // always returns true; have to test results
        is_content(content);

        if (boost::size(content))
        {
            token_range_t result(this->callback_m(   content,
                                                            token_range_t(),
                                                            attribute_set_t(),
                                                            content));

            adobe::copy(result, this->output_m);
        }
        else
            break;
    }
}

/*************************************************************************************************/

template <typename O> // O models OutputIterator
void xml_parser_t<O>::parse_document()
{
	token_range_t dummy;
	
	token_stream_m.set_skip_white_space(true);
	
	is_prolog();
	is_element(dummy);
}

/*************************************************************************************************/

/*!
\relates xml_parser_t

\brief Create an object that will parse the indicated content range using the preorder and content
functions indicated.

\param[in] first		the start of the content range (analagous to a begin iterator)
\param[in] last			the end of the content range (analagous to an end iterator)
\param[in] position		an annotation of the line number at which then content range begins. Used
						when errors are encountered while parsing the content range.
\param[in] predicate	a predicate that indicates whether the application's content callback will
						be called pre-order or in-order for a given element
\param[in] callback		the applications content callback function
\param[in] output		an object that models OutputIterator to which the parser will insert
						the result of processing the content range

\return an xml parser object that will process the indicated content range using the indicated
		application callbacks
*/
template <typename O> // O models OutputIterator
inline xml_parser_t<O> make_xml_parser( uchar_ptr_t                                     first,
										uchar_ptr_t                                     last,
										const line_position_t&                          position,
										typename xml_parser_t<O>::preorder_predicate_t  predicate,
										typename xml_parser_t<O>::callback_proc_t       callback,
										O                                               output)
{ return xml_parser_t<O>(first, last, position, predicate, callback, output); }

/*************************************************************************************************/
/*!
    hexadecimal atoi

    Iterator-based atoi, but for hexadecimal number strings.
*/
template <typename Result, typename InputIterator>
InputIterator xatoi(InputIterator first, InputIterator last, Result& result)
{
    result = 0;

    while (first != last && std::isxdigit(*first))
    {
        typename std::iterator_traits<InputIterator>::value_type c(*first);

        result <<= 4;

        if (std::isdigit(c))
        {
            result += c - '0';
        }
        else
        {
            c = std::use_facet<std::ctype<char> >(std::locale()).tolower(c);

            result += c - 'a' + 10;
        }

        ++first;
    }

    return first;
}

/*************************************************************************************************/
/*!
    decimal atoi

    Iterator-based atoi
*/
template <typename Result, typename InputIterator>
InputIterator datoi(InputIterator first, InputIterator last, Result& result)
{
    result = 0;

    while (first != last && std::isdigit(*first))
    {
        result *= 10;

        result += *first - '0';

        ++first;
    }

    return first;
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
