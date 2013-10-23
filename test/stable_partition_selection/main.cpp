/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>

#include <adobe/algorithm.hpp>
#include <adobe/selection.hpp>

#include <iostream>
#include <iterator>
#include <sstream>
#include <utility>
#include <vector>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

template <typename ForwardIterator>
std::pair<ForwardIterator, ForwardIterator> shift_range(ForwardIterator first,
                                                        ForwardIterator last,
                                                        ForwardIterator range_first,
                                                        ForwardIterator range_last,
                                                        ForwardIterator p)
{
    if (within_range(first, range_first, range_last, p))
        return std::pair<ForwardIterator, ForwardIterator>(range_first, range_last);
    else if (!within_range(first, range_last, last, p))
        return shift_range(first, last, p, range_first, range_last);

    return adobe::rotate(range_first, range_last, p);
}

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

namespace {

/****************************************************************************************************/

template <typename ForwardRange>  
void print_selection(const ForwardRange& range, const adobe::selection_t& selection)
{
    typedef typename ForwardRange::const_iterator       set_const_iterator;
    typedef typename adobe::selection_t::const_iterator selection_const_iterator;

    set_const_iterator       iter(boost::begin(range));
    set_const_iterator       last(boost::end(range));
    selection_const_iterator s_iter(boost::begin(selection));
    selection_const_iterator s_last(boost::end(selection));
    bool                     inside(selection.start_selected());
    std::stringstream        selection_output;

    while (iter != last)
    {
        if (s_iter != s_last && iter == boost::next(boost::begin(range), *s_iter))
        {
            inside = !inside;

            ++s_iter;
        }

        std::stringstream foo;

        foo << *iter << ' ';

        selection_output.fill(inside ? '*' : ' ');
        selection_output.width(static_cast<std::streamsize>(foo.str().size()));
        selection_output << (inside ? '*' : ' ');

        std::cout << foo.str().c_str();

        ++iter;
    }

    std::cout << std::endl << selection_output.str().c_str() << std::endl;

    std::cout << adobe::size(selection) << " boundaries, "
              << adobe::size(selection, range) << " items: [ ";

    selection_copy(selection, range,
                   std::ostream_iterator<typename ForwardRange::value_type>(std::cout, " "));

    std::cout << ']' << std::endl;
}

/****************************************************************************************************/

typedef std::vector<int>               set_type;
typedef set_type::iterator             iterator;
typedef adobe::selection_t             selection_type;
typedef selection_type::const_iterator selection_iterator;

/****************************************************************************************************/

template <typename IteratorCategory>
void do_split_selection(const adobe::selection_t& selection,
                        std::size_t               p,
                        IteratorCategory          iterator_category)
{
    std::pair<selection_iterator, adobe::selection_t::difference_type> result =
        adobe::selection_find_boundary(selection, p, iterator_category);

    std::cout << p << " selection boundary " << static_cast<int>(result.second) << "; ";

    if (result.first == boost::end(selection))
        std::cout << "returned selection split is (end)";
    else
        std::cout << "returned selection split is " << *result.first;

    std::cout << std::endl;
}

/****************************************************************************************************/

void test1()
{
    set_type set(20);

    adobe::iota(set.begin(), set.end(), 0);

    selection_type selection;

    selection.push_back(2);
    selection.push_back(5);
    selection.push_back(9);
    selection.push_back(12);
    selection.push_back(14);
    selection.push_back(17);

    print_selection(set, selection);
    std::cout << std::endl;

    std::cout << "forward case:" << std::endl;
    for (std::size_t p(0), last(set.size()); p != last; ++p)
        do_split_selection(selection, p, std::forward_iterator_tag());

    std::cout << "random_access case:" << std::endl;
    for (std::size_t p(0), last(set.size()); p != last; ++p)
        do_split_selection(selection, p, std::random_access_iterator_tag());
}

/****************************************************************************************************/

void test2()
{
    set_type set(20);

    adobe::iota(set.begin(), set.end(), 0);

    selection_type selection;

    selection.push_back(2);
    selection.push_back(5);
    selection.push_back(9);
    selection.push_back(12);
    selection.push_back(14);
    selection.push_back(17);

    print_selection(set, selection);
    std::cout << std::endl;

    set_type::iterator middle(selection_stable_partition(selection, set));

    selection_type selection2;

    selection2.push_back(static_cast<selection_type::size_type>(std::distance(set.begin(), middle)));

    print_selection(set, selection2);
    std::cout << std::endl;
}

/****************************************************************************************************/

void test3()
{
    for (int i(0); i != 21; ++i)
    {
        set_type set(20);

        adobe::iota(set.begin(), set.end(), 0);

        selection_type selection;

        selection.push_back(2);
        selection.push_back(5);
        selection.push_back(9);
        selection.push_back(12);
        selection.push_back(14);
        selection.push_back(17);

        std::pair<iterator, iterator> p_range =
            adobe::selection_stable_partition_about(selection, set, i);

        selection_type new_selection;

        new_selection.push_back(std::distance(set.begin(), p_range.first));
        new_selection.push_back(std::distance(set.begin(), p_range.second));

        print_selection(set, new_selection);
        std::cout << std::endl;
    }
}

/****************************************************************************************************/

void test4()
{
    set_type set(20);

    adobe::iota(set.begin(), set.end(), 0);

    selection_type selection;

    selection.push_back(2);
    selection.push_back(5);
    selection.push_back(9);
    selection.push_back(12);
    selection.push_back(14);
    selection.push_back(17);

    adobe::selection_partition_copy(selection, set,
                                    std::ostream_iterator<int>(std::cout, "b "),
                                    std::ostream_iterator<int>(std::cout, "g "));

    std::cout << std::endl;
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

int main()
{
    std::cout << "test1:" << std::endl;
    test1();

    std::cout << "test2:" << std::endl;
    test2();

    std::cout << "test3:" << std::endl;
    test3();

    std::cout << "test4:" << std::endl;
    test4();

    return 0;
}

/****************************************************************************************************/
