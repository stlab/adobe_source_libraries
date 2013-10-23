/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef SUDOKU_UTILITIES_HPP
#define SUDOKU_UTILITIES_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>
#include <adobe/timer.hpp>
#include <adobe/dancing_links.hpp>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "sudoku.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>

/****************************************************************************************************/

#if defined(BOOST_MSVC)
namespace std {
    using ::isdigit;
}
#endif

/****************************************************************************************************/

namespace sudoku {

/****************************************************************************************************/

bool verbose_g(true);

/****************************************************************************************************/

enum
{
    dlx_rows_k = 729,
    dlx_cols_k = 324
};

/****************************************************************************************************/

void detailed_status(const sudoku_t& puzzle)
{
    if (!verbose_g) return;

    std::cout << "++=======+=======+=======++=======+=======+=======++=======+=======+=======++" << std::endl;

    for (int row(0); row < 9; ++row)
    {
        for (int mult(0); mult < 3; ++mult)
        {
            int offset(3 * mult);

            std::cout << "|| ";

            for (sudoku_t::const_row_iterator first(puzzle.row_begin(row)), last(puzzle.row_end(row));
                first != last; ++first)
            {
                if (first->value_m)
                {
                    if (mult == 1)
                        std::cout << " -" << first->value_m << "-  ";
                    else
                        std::cout << "      ";
                }
                else
                {
                    if (first->possibilities_m[1 + offset]) std::cout << (1 + offset);
                    else std::cout << ' ';
                    std::cout << ' ';
                    if (first->possibilities_m[2 + offset]) std::cout << (2 + offset);
                    else std::cout << ' ';
                    std::cout << ' ';
                    if (first->possibilities_m[3 + offset]) std::cout << (3 + offset);
                    else std::cout << ' ';
                    std::cout << ' ';
                }

                if (first.col() % 3 == 2)
                {
                    std::cout << "|| ";
                }
                else
                    std::cout << "| ";
                    
            }
            std::cout << std::endl;
        }

        if (row % 3 == 2)
        {
            std::cout << "++=======+=======+=======++=======+=======+=======++=======+=======+=======++" << std::endl;
        }
        else
            std::cout << "++-------+-------+-------++-------+-------+-------++-------+-------+-------++" << std::endl;
    }
}

/****************************************************************************************************/

template <typename Iterator>
std::string name(Iterator x)
{
    std::stringstream s;

    s << "R" << x.row()+1 << "C" << x.col()+1;

    return s.str();
}

/****************************************************************************************************/

template <typename Iterator>
std::string traversal_subject(Iterator x)
{
    std::stringstream s;

    s << x.traversal_subject_type() << " " << x.traversal_subject_identifier();

    return s.str();
}

/****************************************************************************************************/

template <typename Iterator>
std::string name_with_subject(Iterator x)
{
    std::stringstream s;

    s << name(x) << " in " << traversal_subject(x);

    return s.str();
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

inline bool is_solved(const square_t& x)
{ return x.value_m != 0; }

/****************************************************************************************************/

inline bool is_unsolved(const square_t& x)
{ return !is_solved(x); }

/****************************************************************************************************/

inline bool is_solved_for(const square_t& x, int n)
{ return x.value_m == n; }

/****************************************************************************************************/

inline bool is_unsolved_for(const square_t& x, int n)
{ return !is_solved_for(x, n); }

/****************************************************************************************************/

template <typename Iterator>
inline std::size_t count_unsolved_in_range(Iterator first, Iterator last)
{ return std::count_if(first, last, is_unsolved); }

/****************************************************************************************************/

template <typename Iterator>
inline std::size_t count_unsolved_for_range(Iterator first, Iterator last, int n)
{ return std::count_if(first, last, boost::bind(is_unsolved_for, _1, n)); }

/****************************************************************************************************/

template <typename Iterator>
inline bool solved_for_range(Iterator first, Iterator last, int n)
{ return std::find_if(first, last, boost::bind(is_solved_for, _1, n)) != last; }

/****************************************************************************************************/

template <typename Iterator>
inline bool unsolved_for_range(Iterator first, Iterator last, int n)
{ return !solved_for_range(first, last, n); }

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

template <typename Iterator>
bool verify_integrity_for_value_in_range(Iterator first, Iterator last, std::size_t value)
{
    Iterator result(std::find_if(first, last, boost::bind(is_solved_for, _1, value)));

    return  result == last ||
            std::find_if(++result, last, boost::bind(is_solved_for, _1, value)) == last;
}

/****************************************************************************************************/

template <typename Iterator>
bool verify_integrity_for_range(Iterator first, Iterator last)
{
    for (int n(1); n <= 9; ++n)
        if (!verify_integrity_for_value_in_range(first, last, n))
            return false;

    return true;
}

/****************************************************************************************************/

template <typename Iterator>
bool verify_integrity_for_square(const sudoku_t& puzzle, Iterator square)
{
    std::size_t value(square->value_m);

    if (value == 0) return true;

    int box(get_box(square));

    if (!verify_integrity_for_value_in_range(puzzle.row_begin(square.row()), puzzle.row_end(square.row()), value)) return false;
    if (!verify_integrity_for_value_in_range(puzzle.col_begin(square.col()), puzzle.col_end(square.col()), value)) return false;
    if (!verify_integrity_for_value_in_range(puzzle.box_begin(box), puzzle.box_end(box), value)) return false;

    return true;
}

/****************************************************************************************************/

bool verify_integrity(const sudoku_t& puzzle)
{
    for (int i(0); i < 9; ++i)
    {
        if (!verify_integrity_for_range(puzzle.row_begin(i), puzzle.row_end(i))) return false;
        if (!verify_integrity_for_range(puzzle.col_begin(i), puzzle.col_end(i))) return false;
        if (!verify_integrity_for_range(puzzle.box_begin(i), puzzle.box_end(i))) return false;
    }

    return true;
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

template <typename Iterator>
std::size_t pinned_reduction_for_square(const sudoku_t& puzzle, Iterator iterator, bool force = false)
{
    if (iterator->value_m) return 0;

    std::size_t usages(0);

    for (int n(1); n <= 9; ++n)
    {
        if (!iterator->possibilities_m[n] && !force) continue;

        int box(get_box(iterator));

        bool already_solved = 
            solved_for_range(puzzle.row_begin(iterator.row()), puzzle.row_end(iterator.row()), n) ||
            solved_for_range(puzzle.col_begin(iterator.col()), puzzle.col_end(iterator.col()), n) ||
            solved_for_range(puzzle.box_begin(box), puzzle.box_end(box), n);

        if (already_solved || force)
        {
            iterator->possibilities_m[n] = !already_solved;

            ++usages;
        }
    }

    return usages;
}

/****************************************************************************************************/

std::size_t pinned_reduction(sudoku_t& puzzle, bool force = false)
{
    std::size_t usages(0);

    for (sudoku_t::iterator first(puzzle.begin()), last(puzzle.end()); first != last; ++first)
        usages += pinned_reduction_for_square(puzzle, first, force);

    return usages;
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

std::size_t lone_candidate_reduction(sudoku_t& puzzle)
{
    /*
        Description of technique at
        
        http://www.simes.clara.co.uk/programs/sudokutechnique1.htm
    */

    std::size_t usages(0);

    for (   sudoku_t::iterator first(puzzle.begin()), last(puzzle.end());
            first != last; ++first)
    {
        if (first->value_m) continue;

        int new_value(0);

        for (int n(1); n <= 9; ++n)
        {
            if (first->possibilities_m[n])
            {
                if (new_value)
                {
                    new_value = 0;
                    break;
                }

                new_value = n;
            }
        }

        if (!new_value) continue;

        ++usages;

        first->value_m = new_value;

        if (verbose_g) std::cout << "Lone candidate selection of " << name(first) << " to " << new_value << std::endl;
    }

    return usages;
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

template <typename Iterator>
Iterator unique_possibility_for_range(Iterator first, Iterator last, int n)
{
    Iterator result(last);

    for (; first != last; ++first)
    {
        if (first->value_m == 0 && first->possibilities_m[n])
        {
            if (result != last)
                return last;

            result = first;
        }
    }

    return result;
}

/****************************************************************************************************/

template <typename Iterator>
std::size_t unique_candidate_reduction_for_range(Iterator first, Iterator last)
{
    std::size_t usages(0);

    for (int n(1); n <= 9; ++n)
    {
        if (solved_for_range(first, last, n)) continue;

        Iterator unique(unique_possibility_for_range(first, last, n));

        if (unique == last) continue;

        if (verbose_g) std::cout << "Unique candidate selection of " << name_with_subject(unique) << " to " << n << std::endl;

        unique->value_m = n;

        ++usages;
    }

    return usages;
}

/****************************************************************************************************/

std::size_t unique_candidate_reduction(sudoku_t& puzzle)
{
    /*
        Description of technique at

        http://www.simes.clara.co.uk/programs/sudokutechnique2.htm
    */

    std::size_t usages(0);

    for (int i(0); i < 9; ++i)
    {
        usages += unique_candidate_reduction_for_range(puzzle.row_begin(i), puzzle.row_end(i));
        usages += unique_candidate_reduction_for_range(puzzle.col_begin(i), puzzle.col_end(i));
        usages += unique_candidate_reduction_for_range(puzzle.box_begin(i), puzzle.box_end(i));
    }

    return usages;
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

template <typename Iterator, typename UnaryPredicate>
struct k_subset_iterator
{
    // For a container with n elements, iterates over the nCk k-subsets for the container

    typedef Iterator iterator;

    k_subset_iterator(iterator first, iterator last, UnaryPredicate pred, std::size_t k) :
        first_m(first), last_m(last), pred_m(pred), k_m(k)
    { }

    std::size_t chosen_size() const
        { return k_m; }

    iterator chosen(std::size_t i)
    {
        assert (i < k_m);

        return combination_m[i];
    }

    std::size_t unchosen_size() const
    {
        std::size_t count(0);

        for (iterator first(first_m), last(last_m); first != last; ++first)
            if (pred_m(*first) && !is_chosen(first)) ++count;

        return count;
    }

    iterator unchosen(std::size_t i)
    {
        assert (i < unchosen_size());

        for (iterator first(first_m), last(last_m); first != last; ++first)
            if (pred_m(*first) && !is_chosen(first) && i-- == 0)
                return first;

        return last_m; // should never be reached
    }

    std::size_t considered_count() const
        { return unchosen_size() + chosen_size(); }

    std::pair<iterator, bool> considered(std::size_t i)
    {
        assert (i < unchosen_size() + chosen_size());

        for (iterator first(first_m), last(last_m); first != last; ++first)
            if (pred_m(*first) && i-- == 0)
                return std::make_pair(first, is_chosen(first));

        return std::make_pair(last_m, false);
    }

    bool next_combination()
    {
        if (combination_m.empty())
        {
            iterator first(begin());

            for (std::size_t i(0); i < k_m; ++i)
            {
                combination_m.push_back(first);

                first = next(first);
            }

            return true;
        }

        std::size_t index(k_m - 1);
        iterator    next_i(next(combination_m[index]));

        if (next_i != last_m)
        {
            combination_m[index] = next_i;

            return true;
        }
        else
        {
            while (index != 0 && next(combination_m[index - 1]) == combination_m[index]) --index;

            if (index == 0) return false;

            --index;

            combination_m[index] = next(combination_m[index]);

            while (index < k_m - 1)
            {
                combination_m[index + 1] = next(combination_m[index]);

                ++index;
            }
        }

        return true;
    }

private:
    bool is_chosen(iterator i) const
        { return std::find(combination_m.begin(), combination_m.end(), i) != combination_m.end(); }

    iterator begin()
    {
        iterator first(first_m);
        while (first != last_m && !pred_m(*first)) ++first;
        return first;
    }

    iterator next(iterator prev)
    {
        if (prev == last_m) return prev;
        ++prev;
        while (prev != last_m && !pred_m(*prev)) ++prev;
        return prev;
    }

    iterator                first_m;
    iterator                last_m;
    UnaryPredicate          pred_m;
    std::size_t             k_m;
    std::vector<iterator>   combination_m;
};

/****************************************************************************************************/

template <typename Iterator>
std::size_t disjoint_subset_reduction_for_range(Iterator first, Iterator last, std::size_t set_size)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;
    typedef boost::function<bool (const value_type&)>           predicate_t;

    if (count_unsolved_in_range(first, last) <= set_size) return false;

    std::size_t usages(0);

    k_subset_iterator<Iterator, predicate_t> subset_iterator(first, last, is_unsolved, set_size);

    while (subset_iterator.next_combination())
    {
        possibility_set_t set(false);

        for (std::size_t i(0); i < set_size; ++i)
            set |= subset_iterator.chosen(i)->possibilities_m;

        if (set.count_possibilities() == set_size)
        {
            // found a disjoint subset, but only notify the user if we actually
            // alter a possibility set in a square not in the disjoint subset.

            std::stringstream notice;

            notice  << "Disjoint subset in "
                    << traversal_subject(subset_iterator.chosen(0))
                    << " with squares" ;

            for (std::size_t i(0); i < set_size; ++i)
            {
                if (i == set_size - 1)
                    notice << " and";
                else if (i > 0)
                    notice << ',';

                notice << " " << name(subset_iterator.chosen(i));
            }

            notice << ". Subset: " << set;

            bool notified(false);

            // now iterate the items in the range that are not equal to the items in the
            // subset and eliminate the set from the possibilities.

            std::size_t unchosen_size(subset_iterator.unchosen_size());

            for (std::size_t i(0); i < unchosen_size; ++i)
            {
                Iterator unchosen(subset_iterator.unchosen(i));

                possibility_set_t original(unchosen->possibilities_m);

                unchosen->possibilities_m -= set;

                if (original != unchosen->possibilities_m)
                {
                    if (!notified)
                    {
                        if (verbose_g) std::cout << notice.str() << std::endl;

                        notified = true;
                    }

                    if (verbose_g) std::cout    << "   ...updating possibility set in square "
                                                << name(unchosen) << " from " << original << " to "
                                                << unchosen->possibilities_m << std::endl;

                    ++usages;
                }
            }
        }
    }

    return usages;
}

/****************************************************************************************************/

std::size_t disjoint_subset_reduction(sudoku_t& puzzle)
{
    /*
        Description of technique at

        http://www.simes.clara.co.uk/programs/sudokutechnique5.htm
    */

    std::size_t usages(0);

    // the way to do this: pick n (from 2 - 5) squares, and OR their
    // possibility sets together. If the possibility set size is n, you have
    // a disjoint subset. We need not go higher than 5 because 9 - (any n > 5)
    // will be found already by a previous iteration of the loop

    for (std::size_t set_size(2); set_size < 5; ++set_size)
    {
        for (std::size_t i(0); i < 9; ++i)
        {
            usages += disjoint_subset_reduction_for_range(puzzle.row_begin(i), puzzle.row_end(i), set_size);
            usages += disjoint_subset_reduction_for_range(puzzle.col_begin(i), puzzle.col_end(i), set_size);
            usages += disjoint_subset_reduction_for_range(puzzle.box_begin(i), puzzle.box_end(i), set_size);
        }
    }

    return usages;
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

template <typename Iterator>
std::size_t unique_subset_reduction_for_range(Iterator first, Iterator last, std::size_t set_size)
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;
    typedef boost::function<bool (const value_type&)>           predicate_t;

    if (count_unsolved_in_range(first, last) <= set_size) return false;

    std::size_t usages(0);

    k_subset_iterator<Iterator, predicate_t> subset_iterator(first, last, is_unsolved, set_size);

    while (subset_iterator.next_combination())
    {
        possibility_set_t   unchosen_set(false);
        possibility_set_t   chosen_set(false);
        std::size_t         considered_count(subset_iterator.considered_count());

        for (std::size_t i(0); i < considered_count; ++i)
        {
            std::pair<Iterator, bool> next(subset_iterator.considered(i));

            if (next.second)
                chosen_set |= next.first->possibilities_m;
            else
                unchosen_set |= next.first->possibilities_m;
        }

        possibility_set_t hidden_set(chosen_set - unchosen_set);

        if (hidden_set.count_possibilities() == set_size)
        {
            // found a unique subset, but only notify the user if we actually
            // alter a possibility set in a square not in unique subset.

            std::stringstream notice;

            notice  << "Unique subset in "
                    << traversal_subject(subset_iterator.chosen(0))
                    << " with squares" ;

            for (std::size_t i(0); i < set_size; ++i)
            {
                if (i == set_size - 1)
                    notice << " and";
                else if (i > 0)
                    notice << ',';

                notice << " " << name(subset_iterator.chosen(i));
            }

            notice << ". Subset: " << hidden_set;

            bool notified(false);

            for (std::size_t i(0); i < set_size; ++i)
            {
                Iterator chosen(subset_iterator.chosen(i));

                possibility_set_t original(chosen->possibilities_m);

                chosen->possibilities_m &= hidden_set;

                if (original != chosen->possibilities_m)
                {
                    if (!notified)
                    {
                        if (verbose_g) std::cout << notice.str() << std::endl;

                        notified = true;
                    }

                    if (verbose_g) std::cout    << "   ...updating possibility set in square "
                                                << name(chosen) << " from " << original << " to "
                                                << chosen->possibilities_m << std::endl;

                    ++usages;
                }
            }
        }
    }

    return usages;
}

/****************************************************************************************************/

std::size_t unique_subset_reduction(sudoku_t& puzzle)
{
    /*
        Description of technique at

        http://www.simes.clara.co.uk/programs/sudokutechnique9.htm
    */

    std::size_t usages(0);

    // the way to do this: Use k-subset iteration to iterate over combinations
    // of the squares not yet solved in a range. Gather an OR'd possibility set
    // for both the candidate set and the others set. The hidden subset set is
    // any entries in (candidate - others). Iterate over the candidate set, &=ing
    // the unique_subset with the candidate set's possibility sets.

    for (std::size_t set_size(2); set_size < 5; ++set_size)
    {
        for (std::size_t i(0); i < 9; ++i)
        {
            usages += unique_subset_reduction_for_range(puzzle.row_begin(i), puzzle.row_end(i), set_size);
            usages += unique_subset_reduction_for_range(puzzle.col_begin(i), puzzle.col_end(i), set_size);
            usages += unique_subset_reduction_for_range(puzzle.box_begin(i), puzzle.box_end(i), set_size);
        }
    }

    return usages;
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

template <typename Iterator1, typename Iterator2>
bool is_in(Iterator1 first, Iterator1 last, Iterator2 square)
{
    while (first != last)
        if (&(*first) == &(*square))
            return true;
        else
            ++first;

    return false;
}

/****************************************************************************************************/

template <typename Iterator1, typename Iterator2>
std::size_t intersection_reduction_for_ranges(  Iterator1 first1, Iterator1 last1,
                                                Iterator2 first2, Iterator2 last2)
{
    std::size_t usages(0);

    possibility_set_t intersection_set(false);
    possibility_set_t non_intersection_set(false);

    for (Iterator1 first(first1), last(last1); first != last; ++first)
    {
        if (is_solved(*first)) continue;

        if (is_in(first2, last2, first))
            intersection_set |= first->possibilities_m;
        else
            non_intersection_set |= first->possibilities_m;
    }

    possibility_set_t difference_set(intersection_set - non_intersection_set);

    if (difference_set.count_possibilities() != 0)
    {
        bool notified(false);

        for (Iterator2 first(first2), last(last2); first != last; ++first)
        {
            if (is_solved(*first) || is_in(first1, last1, first)) continue;

            possibility_set_t original(first->possibilities_m);

            first->possibilities_m -= difference_set;

            if (original != first->possibilities_m)
            {
                if (!notified)
                {
                    if (verbose_g) std::cout    << "Intersection with " << traversal_subject(first1)
                                                << " and " << traversal_subject(first2) << " yields unique subset "
                                                << difference_set
                                                << " which can be removed from all non-intersecting squares in "
                                                << traversal_subject(first2) << std::endl;

                    notified = true;
                }

                if (verbose_g) std::cout    << "   ...updating possibility set in square "
                                            << name(first) << " from " << original << " to "
                                            << first->possibilities_m << std::endl;

                ++usages;
            }
        }
    }

    return usages;
}

/****************************************************************************************************/

template <typename Iterator>
std::size_t intersection_reduction_for_box_and_range(   sudoku_t::box_iterator  first1,
                                                        sudoku_t::box_iterator  last1,
                                                        Iterator                first2,
                                                        Iterator                last2)
{
    std::size_t usages(0);

    usages += intersection_reduction_for_ranges(first1, last1, first2, last2);
    usages += intersection_reduction_for_ranges(first2, last2, first1, last1);

    return usages;
}

/****************************************************************************************************/

std::size_t intersection_reduction(sudoku_t& puzzle)
{
    /*
        Description of technique at

        http://www.simes.clara.co.uk/programs/sudokutechnique3.htm

        Every box intersects with 3 rows and 3 columns. For each of these 3-square
        intersections, if there is a unique candidate in those 3 squares that does
        not exist anywhere else in the row or column, it can be eliminated from the
        non-intersecting squares in the box. Likewise, if there is a candidate in the
        intersection that does not exist in the rest of the box, it can be eliminated
        from the rest of the squares in the intersecting row or column.
    */

    std::size_t usages(0);

    // the way to do this: for every box step through the three row and column
    // intersections. Then get three posibility sets: one for the intersection
    // squares, one for the non-intersecting row/col squares, and one for the 
    // non-intersecting box squares. Subtract the non-intersection sets with
    // the intersection sets, and if a number remains in the resulting set it
    // can be eliminated as a possibility from the other non-intersecting squares.

    // Note that this handles box/box interactions as well, in that if two
    // different blocks share all their candidates for a number in the
    // same two rows or columns, then the third block that shares those same
    // rows or columns can have that number eliminated as a candidate where
    // it intersects with those rows or columns. Another way to think about
    // it is that a number must be a candidate for the third block in the
    // column or row for which the other two blocks do not have a viable
    // candidate, which is the same problem as the one described above.

    for (std::size_t i(0); i < 9; ++i)
    {
        sudoku_t::box_iterator  box_first(puzzle.box_begin(i));
        sudoku_t::box_iterator  box_last(puzzle.box_end(i));
        std::size_t             row_offset(box_first.row());
        std::size_t             col_offset(box_first.col());

        for (std::size_t j(0); j < 3; ++j)
        {
            usages += intersection_reduction_for_box_and_range(box_first, box_last,
                        puzzle.row_begin(j + row_offset), puzzle.row_end(j + row_offset));

            usages += intersection_reduction_for_box_and_range(box_first, box_last,
                        puzzle.col_begin(j + col_offset), puzzle.col_end(j + col_offset));
        }
    }

    return usages;
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

void setup_links(sudoku_t& puzzle, adobe::dancing_links_t& links)
{
    /*
        Big thanks to Daniel Seiler for the matrix encoding chart below, found at
            http://www.setbb.com/phpbb/viewtopic.php?t=119&highlight=dancing+links&mforum=sudoku

        The matrix has the following format:
                __________________________________________________________________________________________ 
                |  Cell#  |           1            |           2            |   |            9           | 
                |123...81 |r1...r9 c1...c9 b1...b9 |r1...r9 c1...c9 b1...b9 |...|r1...r9 c1...c9 b1...b9 | 
        ________|_________|________________________|________________________|___|________________________| 
        1 -> 1,1| 
        1 -> 1,2| 
          .     |
          .     |
          .     |
        9 -> 9,9|

        the matrix has the following dimensions 
        rows:       number of cells (9 * 9) multiplied by number of possible digits (9) 
        columns:    number of columns plus number of rows plus number of blocks (3 * 9) 
                    multiplied by number of different digits (9) plus number of cells (9 * 9) 

        column indices for number d, row r, column c, box b
        (assuming values are zero-based, including d)
            jump = 27; // 27 columns per number
            col_base = 9;
            box_base = 18;
            n_base = 81;
            row = n_base + d * jump + r
            col = n_base + d * jump + col_base + c;
            box = n_base + d * jump + box_base + b
            cell = row * 9 + col;
            
    */

    pinned_reduction(puzzle, true);
 
    enum
    {
        n = 9,
        n_base = 81,
        jump = 3 * n,
        col_base = n,
        box_base = 2 * n,
    };

    int row_index(0);

    // fill in possibilities here
    // iterate over all the possible digits d
    for (int d = 0; d < n; ++d)
    {
        const int d_jump(n_base + d * jump);
        const int d_jump_box(d_jump + box_base);
        const int d_jump_col(d_jump + col_base);
        const int dpp(d + 1);

        // iterate over all the possible rows r
        for (int r = 0; r < n; ++r)
        {
            const int row(d_jump + r);
            const int cell_base_row(r * 9);
            const int box_index_row((r / 3) * 3);

            // iterator over all the possible columns c
            for (int c = 0; c < n; ++c)
            {
                square_t&           sq(puzzle.square(r, c));
                const int           value(sq.value_m);
                const int           box_index(box_index_row + (c / 3));

                if (value == dpp || value == 0 && sq.possibilities_m[dpp])
                {
                    const int           col(d_jump_col + c);
                    const int           box(d_jump_box + box_index);
                    const int           cell(cell_base_row + c);
#if 0
                    std::cerr   << "possibility:"
                                << "number: " << (dpp)
                                << ", row_index: " << row_index
                                << ", row: " << (r+1) << "(" << row << ")"
                                << ", col: " << (c+1) << "(" << col << ")"
                                << ", box: " << (box_index+1) << "(" << box << ")"
                                << ", cell: " << (r * 9 + c) << "(" << cell << ")"
                                << std::endl;
#endif
                    assert (cell < row);
                    assert (row < col);
                    assert (col < box);

                    // fill in the 1's
                    links.set(row_index, cell);
                    links.set(row_index, row);
                    links.set(row_index, col);
                    links.set(row_index, box);
                }

                ++row_index;
            }
        }
    }
}

/****************************************************************************************************/

std::size_t brute_force_solve(sudoku_t& puzzle, std::size_t max_solutions)
{
    adobe::dancing_links_t links(dlx_rows_k, dlx_cols_k);

    setup_links(puzzle, links);

    return links.search(max_solutions);
}

/****************************************************************************************************/

template <typename ResultsCallback>
std::size_t brute_force_solve(  sudoku_t&       puzzle,
                                std::size_t     max_solutions,
                                ResultsCallback callback)
{
    adobe::dancing_links_t links(dlx_rows_k, dlx_cols_k);

    setup_links(puzzle, links);

    return links.search(max_solutions,
                        callback,
                        adobe::implementation::select_most_constrained_heuristic_t());
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

struct sodoku_filler_t
{
    sodoku_filler_t(sudoku_t& puzzle) :
        puzzle_m(puzzle), last_m(false)
    { }

    sodoku_filler_t(const sodoku_filler_t& rhs) :
        puzzle_m(rhs.puzzle_m), last_m(rhs.last_m)
    { }

    void operator () (std::size_t index, bool last)
    {
        if (last_m) return;

        std::size_t number(index / 81 + 1);
        std::size_t cell(index - (number - 1) * 81);
        std::size_t row(cell / 9);
        std::size_t col(cell % 9);

        puzzle_m.square(row, col).value_m = number;

        last_m = last;
    }

private:
    sudoku_t&   puzzle_m;
    bool        last_m;
};

/****************************************************************************************************/

void timed_solve(const sudoku_t& cpuzzle)
{
    sudoku_t puzzle(cpuzzle);

    adobe::timer_t timer;

    brute_force_solve(puzzle, 1, sodoku_filler_t(puzzle));

    double time(timer.split());

    std::cout << puzzle << std::endl;

    std::cout << "Finding the solution took " << time <<  " milliseconds (" << time / 1e3 << " sec)" << std::endl;
}

/****************************************************************************************************/

struct solution_output_t
{
private:
    typedef boost::array<int, 81>   results_t;

public:
    solution_output_t(std::ofstream* output) :
        output_m(output)
    { }

    void operator () (std::size_t index, bool last)
    {
        std::size_t number(index / 81 + 1);
        std::size_t cell(index - (number - 1) * 81);

        results_m[cell] = number;

        if (last)
        {
            for (   results_t::iterator first(results_m.begin()), last(results_m.end());
                    first != last; ++first)
            {
                (*output_m) << *first;

                *first = 0; // just to make sure the old solution isn't creeping
            }

            (*output_m) << std::endl;
        }
    }

private:
    results_t       results_m;
    std::ofstream*  output_m;
};

/****************************************************************************************************/

void count_solutions(const sudoku_t& cpuzzle, bool save_solutions = false)
{
    sudoku_t puzzle(cpuzzle);

    adobe::timer_t  timer;
    std::size_t     num_solutions(0);

    if (save_solutions)
    {
        std::ofstream output_m("count_solutions.txt");

        num_solutions = brute_force_solve(  puzzle,
                                            std::numeric_limits<std::size_t>::max(),
                                            solution_output_t(&output_m));
    }
    else
    {
        num_solutions = brute_force_solve(puzzle, std::numeric_limits<std::size_t>::max());
    }

    double time(timer.split());

    std::cout   << "Found " << num_solutions << " solution(s) that took " << time
                <<  " milliseconds (" << time / 1e3 << " sec) to find." << std::endl;
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

std::size_t add_potential_squares(sudoku_t& puzzle, std::size_t hint_count)
{
    // A puzzle is ready when removing two squares will leave an ambiguous
    // puzzle, and adding two squares leaves a puzzle with a unique solution

    while (true)
    {
        std::size_t index(std::rand() % 81);
        std::size_t rrow(index % 9);
        std::size_t rcol(index / 9);

        // REVISIT (fbrereto) : What to do if the center is found?
        if (rrow == rcol) continue;

        sudoku_t::iterator candidate1(puzzle.at(rrow, rcol));
        sudoku_t::iterator candidate2(puzzle.at(8 - rrow, 8 - rcol));

        if (candidate1->value_m != 0 || candidate2->value_m != 0) continue;

        // std::cerr << "chose " << name(candidate1) << " and " << name(candidate2) << std::endl;

        pinned_reduction_for_square(puzzle, candidate1, true);
        pinned_reduction_for_square(puzzle, candidate2, true);

        assert (candidate1->possibilities_m.count_possibilities() != 0 &&
                candidate2->possibilities_m.count_possibilities() != 0);

        while (true)
            if (candidate1->possibilities_m[candidate1->value_m = std::rand() % 9 + 1])
                break;

        while (true)
            if (candidate2->possibilities_m[candidate2->value_m = std::rand() % 9 + 1])
                break;

        if (verify_integrity(puzzle))
        {
            //std::cerr << "\tbrute force solve...";
            std::size_t num_results(brute_force_solve(puzzle, 2));
            //std::cerr << "found " << num_results << " solutions" << std::endl;

            switch (num_results)
            {
                case 0:
                {
                    //std::cerr << "broken puzzle -- trying again" << std::endl;
                }
                break;

                case 1:
                {
                    //std::cerr << "unique solution found" << std::endl;

                    return hint_count + 2;
                }
                break;

                default:
                {
                    //std::cerr << hint_count+2 << "..." << std::endl;

                    return add_potential_squares(puzzle, hint_count + 2);
                }
                break;
            }
        }

        candidate2->value_m = 0;
        candidate1->value_m = 0;
    }
}

/****************************************************************************************************/

sudoku_t generate_puzzle(bool noisy = true)
{
    sudoku_t result;

    result.name_m.assign("Generated Puzzle");

    std::size_t hint_count(add_potential_squares(result, 0));

    if (noisy) std::cout << "Generated a puzzle with " << hint_count << " hints" << std::endl;

    return result;
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

const char* score_to_string(std::size_t score)
{
    std::pair<std::size_t, const char*> titles[] =
    {
        std::make_pair<std::size_t, const char*>(25, "trivial"),
        std::make_pair<std::size_t, const char*>(50, "easier"),
        std::make_pair<std::size_t, const char*>(75, "easy"),
        std::make_pair<std::size_t, const char*>(100, "mostly easy"),
        std::make_pair<std::size_t, const char*>(125, "nearly moderate"),
        std::make_pair<std::size_t, const char*>(150, "moderate"),
        std::make_pair<std::size_t, const char*>(175, "beyond moderate"),
        std::make_pair<std::size_t, const char*>(200, "nearly tricky"),
        std::make_pair<std::size_t, const char*>(225, "tricky"),
        std::make_pair<std::size_t, const char*>(250, "beyond tricky"),
        std::make_pair<std::size_t, const char*>(275, "nearly difficult"),
        std::make_pair<std::size_t, const char*>(300, "difficult"),
        std::make_pair<std::size_t, const char*>(325, "beyond difficult"),
        std::make_pair<std::size_t, const char*>(350, "nearly hard"),
        std::make_pair<std::size_t, const char*>(375, "hard"),
        std::make_pair<std::size_t, const char*>(400, "beyond hard"),
        std::make_pair<std::size_t, const char*>(425, "nearly diabolical"),
        std::make_pair<std::size_t, const char*>(450, "diabolical"),
        std::make_pair<std::size_t, const char*>(475, "beyond diabolical"),
        std::make_pair<std::size_t, const char*>(500, "nearly wicked"),
        std::make_pair<std::size_t, const char*>(525, "wicked"),
        std::make_pair<std::size_t, const char*>(800, "beyond wicked")
    };
    std::size_t num_titles(sizeof(titles) / sizeof(titles[0]));

    for (std::size_t i(0); i < num_titles; ++i)
        if (score < titles[i].first)
            return titles[i].second;

    return "unbelievably wicked";
}

/****************************************************************************************************/

std::size_t rate_puzzle(const sudoku_t& cpuzzle, bool noisy = true)
{
    // Considering this solution engine doesn't solve for anything more
    // complicated than box/row/col intersection reductions the rating
    // isn't very accurate, but gives a rough sketch of a rating mechanism.

    typedef std::size_t (*solve_proc_t)(sudoku_t&);
    typedef std::pair<solve_proc_t, std::size_t> solve_rating_t;

    sudoku_t        puzzle(cpuzzle);
    solve_rating_t  rating_set[] =
    {
        std::make_pair<solve_proc_t, std::size_t>(lone_candidate_reduction, 1),
        std::make_pair<solve_proc_t, std::size_t>(unique_candidate_reduction,   2),
        std::make_pair<solve_proc_t, std::size_t>(disjoint_subset_reduction,    4),
        std::make_pair<solve_proc_t, std::size_t>(unique_subset_reduction,  8),
        std::make_pair<solve_proc_t, std::size_t>(intersection_reduction,   16)
    };
    std::size_t rating_set_size(sizeof(rating_set) / sizeof(rating_set[0]));

    std::size_t score(0);
    std::size_t subscore(0);

    verbose_g = false; // don't give out solutions

    while (true)
    {
        if (count_unsolved_in_range(puzzle.begin(), puzzle.end()) == 0) break;

        subscore = 0;

        for (std::size_t i(0); i < rating_set_size; ++i)
        {
            std::size_t subsubscore(0);

            do
            {
                pinned_reduction(puzzle);

                subsubscore = (*rating_set[i].first)(puzzle);

                subscore += subsubscore * rating_set[i].second;
            } while (subsubscore != 0);
        }

        if (subscore == 0) break;   // puzzle could not be solved
        else score += subscore;     // made progress in puzzle
    }

    verbose_g = true; // reset

    if (subscore == 0)
    {
        if (noisy) std::cout << "I couldn't finish the puzzle without guessing..." << std::endl;
        score = 1000 - score;   // set the score to something high but still reflective
                                // of how much progress you can make in the puzzle.
    }

    if (noisy) std::cout    << "I would give this puzzle a rating of " << score_to_string(score)
                            << " (" << score << ")" << std::endl;

    return score;
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

void mass_puzzle_generation()
{
    std::size_t num_puzzles(0);
    std::size_t min_score(0);
    std::string filename;

    std::cout << "Number of puzzles: ";
    std::cin >> num_puzzles;

    std::cout << "Minimum score to save puzzle: ";
    std::cin >> min_score;

    std::cout << "Filename for output (no spaces): ";
    std::cin >> filename;

    std::cout << "Generating, Rating, and saving the good puzzles to " << filename << "..." << std::endl;

    std::ofstream   output(filename.c_str());
    std::size_t     num_saved(0);
    adobe::timer_t  timer;
    std::size_t     dot_marker(num_puzzles / 100);

    for (std::size_t i(0); i < num_puzzles; ++i)
    {
        timer.reset();

        sudoku_t    puzzle(generate_puzzle(false));
        std::size_t score(rate_puzzle(puzzle, false));

        timer.accrue();

        if (score >= min_score)
        {
            for (sudoku_t::iterator first(puzzle.begin()), last(puzzle.end()); first != last; ++first)
                if (is_solved(*first))
                    output << first->value_m;
                else
                    output << '.';
    
            output << '\t';
    
            output << score_to_string(score) << " puzzle (" << score << ")" << std::endl;

            ++num_saved;
        }

        std::size_t i_t(i + 1);

        if (i_t % dot_marker == 0)
            if ((i_t / dot_marker) % 10 == 0)
                std::cerr << (i_t / dot_marker) << '%';
            else
                std::cerr << '.';
    }

    double avg(timer.accrued_average());

    std::cout   << std::endl << "Generated " << num_puzzles << " puzzles, "
                << num_saved << " of which were scored at least " << min_score << "."
                << " Average puzzle generation/rating time: " << avg
                << " miliseconds, or " << (1000.0 / avg) << " per second." << std::endl;
}

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

void performance_benchmark()
{
    std::string filename;

    std::cout << "Name of file with sudoku puzzles in it: ";
    std::cin >> filename;

    std::ifstream input(filename.c_str());

    if (input.fail()) throw std::runtime_error("Can't open file for performance testing");

    adobe::timer_t  timer;
    char            input_buffer[256];

    while (!input.eof())
    {
        input.getline(&input_buffer[0], 256);

        if (input.gcount() < 81) continue;

        sudoku_t puzzle;

        for (int r(0); r < 9; ++r)
        {
            for (int c(0); c < 9; ++c)
            {
                char str_value(input_buffer[r * 9 + c]);

                puzzle.square(r, c).value_m = std::isdigit(str_value) ? str_value - '0' : 0;
            }
        }

        std::size_t num_solutions(0);

        timer.reset();
#if 0
        num_solutions = brute_force_solve(puzzle, std::numeric_limits<std::size_t>::max());
#else
        num_solutions = brute_force_solve(puzzle, 1);
#endif
        timer.accrue();

#if 1
        double last_split(timer.split());

        std::cout   << "Solved a puzzle with " << num_solutions << " solution(s) in "
                    << last_split << " miliseconds (" << (last_split / 1e3) << " seconds)"
                    << std::endl;
#endif
    }

    double total_time(timer.accrued_total());
    double avg_time(timer.accrued_average());

    std::cout   << "Solved all puzzles in " << total_time << " miliseconds ("
                << (total_time / 1e3) << " seconds)" << std::endl
                << "Average puzzle solve time " << avg_time << " miliseconds ("
                << (avg_time / 1e3) << " seconds)" << std::endl;
}

/****************************************************************************************************/

} // namespace sudoku

/****************************************************************************************************/

#endif

/****************************************************************************************************/
