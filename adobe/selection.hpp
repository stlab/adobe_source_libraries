/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_SELECTION_HPP
#define ADOBE_SELECTION_HPP

#include <adobe/config.hpp>

#include <vector>
#include <cassert>

#include <boost/operators.hpp>

/*************************************************************************************************/

namespace adobe {

/****************************************************************************************************/
/*!
    \defgroup selection Selection
    \ingroup asl_libraries
*/
/****************************************************************************************************/
/*!
    \ingroup selection

    \brief A container used to represent a linear boolean selection.

    A linear boolean selection is a set of context-free indices, each of which toggles the selection
    "on" or "off". From an implementation standpoint, each selection index must be greater than the
    one that precedes it (strictly monotonically increasing). There is also a boolean value
    associated with the selection, and that is to represent whether or not the selection is to start
    out "on" or "off" -- thus the inversion of a selection has the same selection boundary set but a
    different boolean value.

    Selections are context-free, meaning that a selection can be spoken about without making
    reference to a container. This allows for selections to be manipulated without applying them to
    a series of elements (e.g., union, size, etc.). Selections can then be applied to a container to
    get context-relevant information (e.g., the number of elements contained within a selection).
    Selections can also be used in place of a predicate for some algorithms. There are a series of
    algorithms associated with the selection container; Please see \ref selection_algorithms.
*/
class selection_t : boost::equality_comparable<selection_t>
{
public:
    /// value_type for selection_t
    typedef std::size_t                   value_type;
    /// store_type for selection_t
    typedef std::vector<value_type>       store_type;
    /// iterator for selection_t
    typedef store_type::iterator          iterator;
    /// const_iterator for selection_t
    typedef store_type::const_iterator    const_iterator;
    /// size_type for selection_t
    typedef store_type::size_type         size_type;
    /// difference_type for selection_t
    typedef store_type::difference_type   difference_type;
    /// reference for selection_t
    typedef value_type&                   reference;
    /// const_reference for selection_t
    typedef const value_type&             const_reference;
    /// pointer for selection_t
    typedef value_type*                   pointer;
    /// const_pointer for selection_t
    typedef const value_type*             const_pointer;

    /*!
        Default constructor.

        \param start_selected Specifies whether or not this selection begins "on" or not. If so,
                              a selection has everything selected when it is first constructed
                              (as it has no other toggle points).
    */
    explicit selection_t(bool start_selected = false) :
        start_selected_m(start_selected)
    { }

    /*!
        Constructor with the ability to initialize the selection with toggle points from the get-go.

        \pre
            <code>[ first, last )</code> is strictly monotonically increasing

        \param first an iterator to the first toggle point to include
        \param last an iterator to one past the last toggle point to include
        \param start_selected Specifies whether or not this selection begins "on" or not.
    */
    template <typename I> // value_type(I) == value_type
    selection_t(I first, I last, bool start_selected = false) :
        start_selected_m(start_selected),
        store_m(first, last)
    { }

    /*!
        \return the number of boundaries in the selection; to get the number of elements contained
                within a selection, a container must also be supplied, as such:
                <code>selection_t::size_type selected_element_size(adobe::size(selection, container))</code>
    */
    size_type size() const
    { return store_m.size(); }

    /// \return \c true if there is at least one toggle point, or the selection starts selected.
    bool empty() const
    { return store_m.empty() && start_selected_m == false; }

    /// pushes a toggle point onto the back of the selection.
    /// The index must be greater than the previous toggle point.
    void push_back(const value_type& x)
    {
        assert(empty() || store_m.back() < x);

        store_m.push_back(x);
    }

    /// gets an iterator to the beginning of the toggle point sequence
    const_iterator begin() const { return store_m.begin(); }

    /// gets an iterator to the end of the toggle point sequence
    const_iterator end() const { return store_m.end(); }

    /// fetches a toggle point at index i of the toggle point sequence
    const value_type& operator[](const size_type& i) const
    { return store_m[i]; }

    /*!
        inverts the selection.
    */
    void invert()
    { start_selected_m = !start_selected_m; }

    /*!
        \return Whether or not the selection begins toggled "on"
    */
    bool start_selected() const
    { return start_selected_m; }

    friend inline bool operator==(const selection_t& x, const selection_t& y)
    {
        return x.size() == y.size() &&
               std::equal(x.begin(), x.end(), y.begin());
    }

private:
#ifndef ADOBE_NO_DOCUMENTATION
    // This *can* be public because modifying it will not
    // violate any invariants.
    bool start_selected_m;

    // The requirement on the index store is that it be sorted.
    // Thus, we need to provide mechanisms where it stays so.
    store_type store_m;
#endif
};

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
