/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
/*************************************************************************************************/

#ifndef ADOBE_ALGORITHM_SELECTION_HPP
#define ADOBE_ALGORITHM_SELECTION_HPP

#include <adobe/config.hpp>

#include <algorithm>
#include <functional>
#include <vector>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/value_type.hpp>
#include <boost/range/size_type.hpp>
#include <boost/range/size.hpp>

#include <adobe/algorithm/other_of.hpp>
#include <adobe/algorithm/rotate.hpp>
#include <adobe/iterator/type_functions.hpp>

/*************************************************************************************************/

namespace adobe {

/*************************************************************************************************/
/*!
    \defgroup selection_algorithms selection
    \ingroup algorithm

    \par Terminology:
        - Selection: A collection of iterators in sorted order over a sequence. When considering a
          sequence the selection is initially "off". As the algorithm passes over a position within
          the list of selection points, the selection is toggled.

    This set of functions behave in similar manner to the suite of operations in the STL. What makes
    these algorithms different from their STL counterparts is that you are supplied the initial
    ranges of "good" and "bad" positions in the form of the selection; thus there is no need for a
    predicate. It is as if the "floor" of the algorithm's processing tree were removed, as the leaf
    nodes are merely for discerning the state of an element by means of the predicate.
*/
/*************************************************************************************************/
/*!
    \ingroup selection_algorithms

    When one selection is exhausted this algorithm will complete the selection operation for the
    remainder of the other selection.
*/
template <typename S, // S models ForwardIterator, value_type(S) == I
          typename O, // O models OutputIterator
          typename P> // P models BinaryPredicate
inline O selection_operation_remainder(S    first,
                                       S    last,
                                       O    output,
                                       bool this_inside,
                                       bool other_inside,
                                       P    pred)
{
    bool prev_op_result(pred(this_inside, other_inside));

    while (first != last)
    {
        this_inside = !this_inside;

        bool cur_op_result(pred(this_inside, other_inside));

        if (prev_op_result ^ cur_op_result)
            *output++ = *first;

        prev_op_result = cur_op_result;

        ++first;
    }

    return output;
}

/*************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief xor funtion object
*/
template <typename C1,      // C1 models ConvertibleToBool
          typename C2 = C1> // C2 models ConvertibleToBool
struct logical_xor : std::binary_function<C1, C2, bool>
{
    /// \return <code>x ^ y</code>
    bool operator()(const C1& x, const C2& y) const
    { return static_cast<bool>(x) ^ static_cast<bool>(y); }
};

/*************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief selection_operation implementation
*/
template <typename S1, // S1 models ForwardIterator, value_type(S1) == value_type(S2)
          typename S2, // S2 models ForwardIterator, value_type(S2) == value_type(S1)
          typename O,  // O models OutputIterator
          typename P,  // P models BinaryPredicate
          typename C>  // C models StrictWeakOrdering
O selection_operation(S1   s1_first,
                      S1   s1_last,
                      S2   s2_first,
                      S2   s2_last,
                      O    output,
                      bool s1_inside,
                      bool s2_inside,
                      P    pred,
                      C    comp)
{
    if (s1_first == s1_last)
        return selection_operation_remainder(s2_first, s2_last, output, s2_inside, s1_inside, pred);
    else if (s2_first == s2_last)
        return selection_operation_remainder(s1_first, s1_last, output, s1_inside, s2_inside, pred);

    bool prev_op_result(pred(s1_inside, s2_inside));

    while (true)
    {
        typename std::iterator_traits<S1>::value_type next(comp(*s1_first, *s2_first) ? *s1_first : *s2_first);

        if (*s1_first == next)
        {
            s1_inside = !s1_inside;

            ++s1_first;
        }

        if (*s2_first == next)
        {
            s2_inside = !s2_inside;

            ++s2_first;
        }

        bool cur_op_result(pred(s1_inside, s2_inside));

        if (prev_op_result ^ cur_op_result)
            *output++ = next;

        prev_op_result = cur_op_result;

        if (s1_first == s1_last)
            return selection_operation_remainder(s2_first, s2_last, output, s2_inside, s1_inside, pred);
        else if (s2_first == s2_last)
            return selection_operation_remainder(s1_first, s1_last, output, s1_inside, s2_inside, pred);
    }

    return output;
}

/*************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief selection_union implementation
*/
template <typename S1, // S1 models ForwardIterator, value_type(S1) == I
          typename S2, // S2 models ForwardIterator, value_type(S2) == I
          typename O,  // O models OutputIterator
          typename C>  // C models StrictWeakOrdering
inline O selection_union(S1   s1_first,
                         S1   s1_last,
                         S2   s2_first,
                         S2   s2_last,
                         O    output,
                         C    comp,
                         bool s1_inside = false,
                         bool s2_inside = false)
{
    return selection_operation(s1_first, s1_last,
                               s2_first, s2_last,
                               output,
                               s1_inside,
                               s2_inside,
                               std::logical_or<bool>(),
                               comp);
}

/*************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief selection_intersection implementation
*/
template <typename S1, // S1 models ForwardIterator, value_type(S1) == I
          typename S2, // S2 models ForwardIterator, value_type(S2) == I
          typename O,  // O models OutputIterator
          typename C>  // C models StrictWeakOrdering
inline O selection_intersection(S1   s1_first,
                                S1   s1_last,
                                S2   s2_first,
                                S2   s2_last,
                                O    output,
                                C    comp,
                                bool s1_inside = false,
                                bool s2_inside = false)
{
    return selection_operation(s1_first, s1_last,
                               s2_first, s2_last,
                               output,
                               s1_inside,
                               s2_inside,
                               std::logical_and<bool>(),
                               comp);
}

/*************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief selection_difference implementation
*/
template <typename S1, // S1 models ForwardIterator, value_type(S1) == I
          typename S2, // S2 models ForwardIterator, value_type(S2) == I
          typename O,  // O models OutputIterator
          typename C>  // C models StrictWeakOrdering
inline O selection_difference(S1   s1_first,
                              S1   s1_last,
                              S2   s2_first,
                              S2   s2_last,
                              O    output,
                              C    comp,
                              bool s1_inside = false,
                              bool s2_inside = false)
{
    return selection_intersection(s1_first, s1_last,
                                  s2_first, s2_last,
                                  output,
                                  comp,
                                  s1_inside,
                                  !s2_inside);
}

/*************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief selection_symmetric_difference implementation
*/
template <typename S1, // S1 models ForwardIterator, value_type(S1) == I
          typename S2, // S2 models ForwardIterator, value_type(S2) == I
          typename O,  // O models OutputIterator
          typename C>  // C models StrictWeakOrdering
inline O selection_symmetric_difference(S1   s1_first,
                                        S1   s1_last,
                                        S2   s2_first,
                                        S2   s2_last,
                                        O    output,
                                        C    comp,
                                        bool s1_inside = false,
                                        bool s2_inside = false)
{
    return selection_operation(s1_first, s1_last,
                               s2_first, s2_last,
                               output,
                               s1_inside,
                               s2_inside,
                               logical_xor<bool>(),
                               comp);
}

/*************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief selection_includes implementation
*/
template <typename S1, // S1 models InputIterator, value_type(S1) == I
          typename S2, // S2 models InputIterator, value_type(S2) == I
          typename C>  // C models StrictWeakOrdering
inline bool selection_includes(S1   s1_first,
                               S1   s1_last,
                               S2   s2_first,
                               S2   s2_last,
                               C    comp,
                               bool s1_inside = false,
                               bool s2_inside = false)
{
    if (s1_inside == s2_inside)
    {
        typedef typename std::iterator_traits<S1>::value_type value_type;

        std::vector<value_type> result;

        selection_intersection(s1_first, s1_last,
                               s2_first, s2_last,
                               std::back_inserter(result),
                               comp,
                               s1_inside, s2_inside);

        return std::equal(s2_first, s2_last, result.begin());
    }
    else if (s1_inside)
    {
        return selection_includes(boost::next(s1_first), s1_last,
                                  s2_first, s2_last,
                                  comp, !s1_inside, s2_inside);
    }

    // s2_inside == true
    return selection_includes(s1_first, s1_last,
                              boost::next(s2_first), s2_last,
                              comp, s1_inside, !s2_inside);
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief selection_intersection implementation
*/
template <typename Selection1, typename Selection2>
Selection1 selection_intersection(const Selection1& x, const Selection2& y)
{
    if (&x == &y)
        return x;

    Selection1 result;

    adobe::selection_intersection(x.begin(), x.end(),
                                  y.begin(), y.end(),
                                  std::back_inserter(result),
                                  std::less<typename boost::range_value<Selection1>::type>(),
                                  start_selected(x),
                                  start_selected(y));

    return result;
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief selection_union implementation
*/
template <typename Selection1, typename Selection2>
Selection1 selection_union(const Selection1& x, const Selection2& y)
{
    if (&x == &y)
        return x;

    Selection1 result;

    adobe::selection_union(x.begin(), x.end(),
                           y.begin(), y.end(),
                           std::back_inserter(result),
                           std::less<typename boost::range_value<Selection1>::type>(),
                           start_selected(x),
                           start_selected(y));

    return result;
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief selection_difference implementation
*/
template <typename Selection1, typename Selection2>
Selection1 selection_difference(const Selection1& x, const Selection2& y)
{
    if (&x == &y)
        return Selection1();

    Selection1 result;

    adobe::selection_difference(x.begin(), x.end(),
                                y.begin(), y.end(),
                                std::back_inserter(result),
                                std::less<typename boost::range_value<Selection1>::type>(),
                                start_selected(x),
                                start_selected(y));

    return result;
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief selection_symmetric_difference implementation
*/
template <typename Selection1, typename Selection2>
Selection1 selection_symmetric_difference(const Selection1& x, const Selection2& y)
{
    if (&x == &y)
        return Selection1();

    Selection1 result;

    adobe::selection_symmetric_difference(x.begin(), x.end(),
                                          y.begin(), y.end(),
                                          std::back_inserter(result),
                                          std::less<typename boost::range_value<Selection1>::type>(),
                                          start_selected(x),
                                          start_selected(y));

    return result;
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief selection_includes implementation
*/
template <typename Selection1, typename Selection2>
bool selection_includes(const Selection1& x, const Selection2& y)
{
    if (&x == &y)
        return true;

    return adobe::selection_includes(x.begin(), x.end(),
                                     y.begin(), y.end(),
                                     std::less<typename boost::range_value<Selection1>::type>(),
                                     start_selected(x),
                                     start_selected(y));
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief invert implementation
*/
template <typename Selection>
inline void invert(Selection& x)
{ x.invert(); }

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief start_selected implementation
*/
template <typename Selection>
inline bool start_selected(const Selection& x)
{ return x.start_selected(); }

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \return the number of selection boundaries present in \c x.
*/
template <typename Selection>
inline typename boost::range_size<Selection>::type size(const Selection& x)
{ return x.size(); }

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \return Given a container, returns the number of elements selected by the selection \c x.
*/
template <typename Selection, typename ForwardRange>
typename boost::range_size<Selection>::type size(const Selection& x, const ForwardRange& range)
{
    typedef typename boost::range_const_iterator<Selection>::type    selection_const_iterator;
    typedef typename boost::range_size<Selection>::type              selection_size_type;
    typedef typename boost::range_const_iterator<ForwardRange>::type range_const_iterator;

    if (x.empty())
        return 0;

    // this is the case when the selection has no elements, but it starts selected
    // (in other words, every item in the selection is toggled as selected)
    if (x.size() == 0)
        return boost::size(range);

    selection_const_iterator s_iter(boost::begin(x));
    selection_const_iterator s_last(boost::end(x));

    range_const_iterator prev(boost::begin(range));
    range_const_iterator iter(boost::next(prev, *s_iter));
    range_const_iterator last(boost::end(range));

    selection_size_type result(0);
    bool                inside(start_selected(x));

    while (true)
    {
        if (inside)
            result += static_cast<selection_size_type>(std::distance(prev, iter));

        if (iter == last)
            break;

        prev = iter;

        iter = ++s_iter == s_last ? last : boost::next(boost::begin(range), *s_iter);

        inside = !inside;
    }

    return result;
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \return Whether or not \c index is contained within Selection \c x.
*/
template <typename Selection>
bool is_selected(const Selection& x, typename Selection::value_type index)
{
    typename boost::range_const_iterator<Selection>::type found(std::upper_bound(boost::begin(x), boost::end(x), index));
    typename boost::range_size<Selection>::type           count(std::distance(boost::begin(x), found));

    return (count % 2 == 1) ^ start_selected(x);
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \todo (fbrereto) this looks eerily familiar to selection_foreach; they can probably collapse
    with an "assign and advance" iterator adaptor wrapped over the output iterator.
*/
template <typename Selection, typename ForwardRange, typename OutputIterator>
OutputIterator selection_copy(const Selection& x, const ForwardRange& range, OutputIterator output)
{
    typedef typename boost::range_const_iterator<Selection>::type    selection_const_iterator;
    typedef typename boost::range_const_iterator<ForwardRange>::type range_const_iterator;

    if (boost::size(range) == 0)
        return output;

    bool inside(start_selected(x));

    selection_const_iterator s_iter(boost::begin(x));
    selection_const_iterator s_last(boost::end(x));

    range_const_iterator iter(boost::begin(range));
    range_const_iterator last(boost::end(range));

    while (iter != last)
    {
        if (s_iter != s_last && iter == boost::next(boost::begin(range), *s_iter))
        {
            ++s_iter;

            inside = !inside;
        }

        if (inside)
            *output++ = *iter;

        ++iter;
    }

    return output;
}

/*************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief selection_partition_copy implementation
*/
template <typename Selection,
          typename ForwardRange,
          typename O1, // O1 models OutputIterator
          typename O2> // O2 models OutputIterator
std::pair<O1, O2> selection_partition_copy(const Selection& selection,
                                           ForwardRange&    range,
                                           O1               false_output,
                                           O2               true_output)
{
    typedef typename boost::range_const_iterator<Selection>::type selection_const_iterator;
    typedef typename boost::range_iterator<ForwardRange>::type    range_iterator;

    if (boost::size(range) == 0)
        return std::make_pair(false_output, true_output);

    selection_const_iterator selection_first(boost::begin(selection));
    selection_const_iterator selection_last(boost::end(selection));

    range_iterator first(boost::begin(range));
    range_iterator last(boost::end(range));

    bool inside(start_selected(selection));

    while (true)
    {
        range_iterator copy_last(selection_first == selection_last ? last : boost::next(boost::begin(range), *selection_first));

        // REVISIT (fbrereto) : It'd be nice to collapse the following into ? :
        //                      notation, but some compilers require that the
        //                      types returned by ? : be the same, which we cannot
        //                      guarantee here.
        if (inside)
            std::copy(first, copy_last, true_output);
        else
            std::copy(first, copy_last, false_output);

        if (copy_last == last)
            break;

        first = copy_last;
        ++selection_first;
        inside = !inside;
    }

    return std::make_pair(false_output, true_output);
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    \brief selection_foreach implementation
*/
template <typename Selection, typename ForwardRange, typename UnaryFunction>
UnaryFunction selection_foreach(const Selection& x, const ForwardRange& range, UnaryFunction proc)
{
    typedef typename boost::range_const_iterator<Selection>::type    selection_const_iterator;
    typedef typename boost::range_const_iterator<ForwardRange>::type range_const_iterator;

    bool inside(start_selected(x));

    selection_const_iterator s_iter(boost::begin(x));
    selection_const_iterator s_last(boost::end(x));

    range_const_iterator iter(boost::begin(range));
    range_const_iterator last(boost::end(range));

    while (iter != last)
    {
        if (s_iter != s_last && iter == boost::next(boost::begin(range), *s_iter))
        {
            ++s_iter;

            inside = !inside;
        }

        if (inside)
            proc(*iter);

        ++iter;
    }

    return proc;
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    This is a RandomAccessIterator implementation of selection_find_boundary
*/
template <typename Selection>
inline
std::pair<typename boost::range_const_iterator<Selection>::type,
          typename boost::range_size<Selection>::type>
selection_find_boundary(const Selection&              selection,
                        typename Selection::size_type p,
                        std::random_access_iterator_tag)
{
    typedef typename boost::range_const_iterator<Selection>::type const_iterator;
    typedef typename boost::range_size<Selection>::type           size_type;
    typedef std::pair<const_iterator, size_type>                  result_type;

    const_iterator bound(std::lower_bound(boost::begin(selection), boost::end(selection), p));

    return result_type(bound, static_cast<size_type>(std::distance(boost::begin(selection), bound)));
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    This is a ForwardIterator implementation of selection_find_boundary
*/
template <typename Selection>
std::pair<typename boost::range_const_iterator<Selection>::type,
          typename boost::range_size<Selection>::type>
selection_find_boundary(const Selection&              selection,
                        typename Selection::size_type p,
                        std::forward_iterator_tag)
{
    typedef typename boost::range_const_iterator<Selection>::type const_iterator;
    typedef typename boost::range_size<Selection>::type           size_type;
    typedef std::pair<const_iterator, size_type>                  result_type;

    const_iterator iter(boost::begin(selection));
    const_iterator last(boost::end(selection));
    size_type      boundary_count(0);

    while (iter != last && *iter < p)
    {
        ++boundary_count;
        ++iter;
    }

    return result_type(iter, boundary_count);
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    selection_find_boundary will take a selection and, given a position p,
    will divide the selection into two subsets: the subset of the selection
    before (or at) p and the subset of the selection selection after p.

    This will dispatch to the appropriate split_selection implementation
    based on the iterator_category of the iterators.

    \param selection is a container modeling the SequenceConcept
    \param p the point at which the selection is to be split
    
    \return
    A pair of values. The first is a SelectionIterator to an index
    divding the selection by p. The second value is a count of the number of
    selection boundaries iterated over to get to the SelectionIterator.
*/
template <typename Selection>
inline
std::pair<typename boost::range_const_iterator<Selection>::type,
          typename boost::range_size<Selection>::type>
selection_find_boundary(const Selection&              selection,
                        typename Selection::size_type p)
{
    typedef typename boost::range_const_iterator<Selection>::type const_iterator;
    typedef typename boost::range_size<Selection>::type           size_type;
    typedef std::pair<const_iterator, size_type>                  result_type;
    typedef typename iterator_category<const_iterator>::type iterator_category;

    if (boost::size(selection) == 0)
        return result_type(boost::end(selection), 0);

    return selection_find_boundary(selection, p, iterator_category());
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    stable_partition_selection takes a collection of elements defined by a selection and partitons
    them according to whether or not they are part of the selection. The algorithm is stable.
    The result is an iterator that is the boundary between the two partitions. For example:

    <pre>
    1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
           |---R1----|              |------R2------|              |------R3------|
    </pre>

    becomes:

    <pre>
    1 2 3 4 10 11 12 13 14 20 21 22 23 24 30 5 6 7 8 9 15 16 17 18 19 25 26 27 28 29
                                            |---------------------------------------|
                                            p
    </pre>

    \par Storage Requirements:

    The algorithm is in-situ.

    \par Time Complexity:

    <code>O(N log N)</code>, where \c N is the number of elements affected by the algorithm. This is
    a range from the first item affected (be that the first item in the selection, or p if p comes
    before it) to the last item affected (be that one before the last item in the selection, or p if
    p comes after it).
*/
template <typename SelectionIterator, typename RangeIterator>
RangeIterator selection_stable_partition(SelectionIterator selection_first,
                                         SelectionIterator selection_last,
                                         RangeIterator     first,
                                         RangeIterator     range_first,
                                         RangeIterator     range_last,
                                         std::size_t       boundary_count = 0)
{
    std::size_t n(static_cast<std::size_t>(std::distance(selection_first, selection_last)));

    if (n == 0)
        return boundary_count % 2 ? range_first : range_last;

    std::size_t       half(n / 2);
    SelectionIterator selection_middle(boost::next(selection_first, half));
    RangeIterator     range_middle(boost::next(first, *selection_middle));

    RangeIterator i(selection_stable_partition(selection_first, selection_middle,
                                               first, range_first, range_middle,
                                               boundary_count));

    RangeIterator j(selection_stable_partition(boost::next(selection_middle), selection_last,
                                               first, range_middle, range_last,
                                               boundary_count + half + 1));

    return other_of(adobe::rotate(i, range_middle, j), range_middle);
}

/*************************************************************************************************/
/*!
    \ingroup selection_algorithms

    A SelectionConcept-based version of \c selection_stable_partition.
*/
template <typename Selection,
          typename ForwardRange>
inline
typename boost::range_iterator<ForwardRange>::type
selection_stable_partition(const Selection& selection,
                           ForwardRange&    range)
{
    return selection_stable_partition(boost::begin(selection), boost::end(selection),
                                      boost::begin(range),
                                      boost::begin(range), boost::end(range),
                                      start_selected(selection));
}

/*************************************************************************************************/
/*!
    \ingroup selection_algorithms

    stable_partition_selection_about takes a collection of elements defined by a selection and moves
    them to a position (p) within the sequence. The algorithm is stable. The result is a pair of
    iterators [ p_first, p_last ) that contain the selection moved to p. For example:

    <pre>
    1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
           |---R1----|              |------R2------|        p     |------R3------|
    </pre>

    becomes:

    <pre>
    1 2 3 4 10 11 12 13 14 20 21 22 5 6 7 8 9 15 16 17 18 19 25 26 27 28 29 23 24 30
                                   |---------------------------------------|
                                   p_first                                 p_last
    </pre>

    The problem is broken down into two basic subproblems, namely, moving those ranges that are
    before p forward to p, and moving those ranges after p backward to p. These two problems are
    each an issue of modified stable partitioning.

    \par Storage Requirements:

    The algorithm is in-situ.

    \par Time Complexity:

    <code>O(N log N)</code>, where \c N is the number of elements affected by the algorithm. This is
    a range from the first item affected (be that the first item in the selection, or p if p comes
    before it) to the last item affected (be that one before the last item in the selection, or p if
    p comes after it).
*/
template <typename Selection,
          typename ForwardRange>
std::pair<typename boost::range_iterator<ForwardRange>::type,
          typename boost::range_iterator<ForwardRange>::type>
selection_stable_partition_about(const Selection&                            selection,
                                 ForwardRange&                               range,
                                 std::size_t                                 p,
                                 typename boost::range_size<Selection>::type prior_boundary_count = 0)
{
    typedef typename boost::range_size<Selection>::type           size_type;
    typedef typename boost::range_const_iterator<Selection>::type selection_const_iterator;
    typedef typename boost::range_iterator<ForwardRange>::type    range_iterator;

    std::pair<selection_const_iterator, size_type> selection_split =
        adobe::selection_find_boundary(selection, p);

    range_iterator first(boost::begin(range));
    range_iterator range_p(boost::next(first, p));
    range_iterator last(boost::end(range));

    range_iterator i(selection_stable_partition(boost::begin(selection), selection_split.first,
                                                first, first, range_p,
                                                prior_boundary_count));

    range_iterator j(selection_stable_partition(selection_split.first, boost::end(selection),
                                                first, range_p, last,
                                                selection_split.second + 1));

    return std::pair<range_iterator, range_iterator>(i, j);
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    Takes a set of indices and converts them to a boundary-based Selection
*/
template <typename Selection,
          typename ForwardRange>
Selection index_set_to_selection(const ForwardRange& index_set)
{
    Selection result;

    // REVISIT (fbrereto) : This would go much faster using divide-and-conquer
    //                      and eventually balanced reduction.

    typedef typename boost::range_const_iterator<ForwardRange>::type range_const_iterator;

    range_const_iterator iter(boost::begin(index_set));
    range_const_iterator last(boost::end(index_set));

    for (; iter != last; ++iter)
    {
        Selection tmp;

        tmp.push_back(*iter);
        tmp.push_back(*iter + 1);

        result = selection_union(result, tmp);
    }

    return result;
}

/****************************************************************************************************/
/*!
    \ingroup selection_algorithms

    Takes a set of indices and converts them to a boundary-based Selection
*/
template <typename Selection,
          typename OutputIterator>
OutputIterator selection_to_index_set(const Selection&                            selection,
                                      typename boost::range_size<Selection>::type max_index,
                                      OutputIterator                              output)
{
    typedef typename boost::range_size<Selection>::type           size_type;
    typedef typename boost::range_const_iterator<Selection>::type selection_const_iterator;

    bool                     selected(start_selected(selection));
    std::size_t              index(0);
    selection_const_iterator iter(boost::begin(selection));
    selection_const_iterator last(boost::end(selection));

    while (iter != last)
    {
        while (index != *iter && index != max_index)
        {
            if (selected)
                *output++ = index;

            ++index;
        }

        selected = !selected;
        ++iter;
    }

    return output;
}

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif

/*************************************************************************************************/
