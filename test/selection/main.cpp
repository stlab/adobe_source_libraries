/*
    Copyright 2016 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
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

namespace {

/****************************************************************************************************/

template <typename ForwardRange>
void print_selection(const ForwardRange& range, const adobe::selection_t& selection) {
    typedef typename ForwardRange::const_iterator set_const_iterator;
    typedef typename adobe::selection_t::const_iterator selection_const_iterator;

    set_const_iterator iter(boost::begin(range));
    set_const_iterator last(boost::end(range));
    selection_const_iterator s_iter(boost::begin(selection));
    selection_const_iterator s_last(boost::end(selection));
    bool inside(selection.start_selected());
    std::stringstream selection_output;

    while (iter != last) {
        if (s_iter != s_last && iter == boost::next(boost::begin(range), *s_iter)) {
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

    std::cout << adobe::size(selection) << " boundaries, " << adobe::size(selection, range)
              << " items: [ ";

    selection_copy(selection, range,
                   std::ostream_iterator<typename ForwardRange::value_type>(std::cout, " "));

    std::cout << ']' << std::endl;
}

/****************************************************************************************************/

typedef std::vector<int> set_type;
typedef set_type::iterator iterator;
typedef adobe::selection_t selection_type;
typedef selection_type::const_iterator selection_iterator;

/****************************************************************************************************/
#if 0
template <typename IteratorCategory>
void do_split_selection(const adobe::selection_t& selection, std::size_t p,
                        IteratorCategory iterator_category) {
    std::pair<selection_iterator, adobe::selection_t::difference_type> result =
        adobe::selection_find_boundary(selection, p, iterator_category);

    std::cout << p << " selection boundary " << static_cast<int>(result.second) << "; ";

    if (result.first == boost::end(selection))
        std::cout << "returned selection split is (end)";
    else
        std::cout << "returned selection split is " << *result.first;

    std::cout << std::endl;
}
#endif
/****************************************************************************************************/

void test1() {
    selection_type selection(true);
    set_type       set;

    selection_to_index_set(selection, 10, std::back_inserter<set_type>(set));

    print_selection(set, selection);    
}

/****************************************************************************************************/

void test2() {
    set_type       set{ 1, 2, 3, 4, 5 };
    selection_type all_set_1(true);
    selection_type all_set_2;
    selection_type one_set;

    all_set_2.push_back(0);

    one_set.push_back(1);
    one_set.push_back(2);

    print_selection(set, adobe::selection_difference(all_set_1, one_set));
    print_selection(set, adobe::selection_difference(all_set_2, one_set));

    all_set_1.invert();
    all_set_2.invert();
    one_set.invert();

    print_selection(set, adobe::selection_difference(one_set, all_set_1));
    print_selection(set, adobe::selection_difference(one_set, all_set_2));
}

/****************************************************************************************************/

void test3() {
    set_type       set{ 1, 2, 3, 4, 5 };
    selection_type all_set_1(true);
    selection_type all_set_2;
    selection_type one_set;

    all_set_2.push_back(0);

    one_set.push_back(1);
    one_set.push_back(2);

    print_selection(set, adobe::selection_intersection(all_set_1, one_set));
    print_selection(set, adobe::selection_intersection(all_set_2, one_set));

    one_set.invert();

    print_selection(set, adobe::selection_intersection(all_set_1, one_set));
    print_selection(set, adobe::selection_intersection(all_set_2, one_set));
}

/****************************************************************************************************/

void test4() {
    set_type       set{ 1, 2, 3, 4, 5 };
    selection_type all_set_1(true);
    selection_type all_set_2;
    selection_type one_set;

    all_set_2.push_back(0);

    one_set.push_back(1);
    one_set.push_back(2);

    print_selection(set, adobe::selection_union(all_set_1, one_set));
    print_selection(set, adobe::selection_union(all_set_2, one_set));

    all_set_1.invert();
    all_set_2.invert();
    one_set.invert();

    print_selection(set, adobe::selection_union(all_set_1, one_set));
    print_selection(set, adobe::selection_union(all_set_2, one_set));
}

/****************************************************************************************************/

void test5() {
    set_type       set{ 1, 2, 3, 4, 5 };
    selection_type all_set_1(true);
    selection_type all_set_2;
    selection_type one_set;

    all_set_2.push_back(0);

    one_set.push_back(1);
    one_set.push_back(2);

    print_selection(set, adobe::selection_symmetric_difference(all_set_1, one_set));
    print_selection(set, adobe::selection_symmetric_difference(all_set_2, one_set));

    all_set_1.invert();
    all_set_2.invert();
    one_set.invert();

    print_selection(set, adobe::selection_symmetric_difference(all_set_1, one_set));
    print_selection(set, adobe::selection_symmetric_difference(all_set_2, one_set));
}

/****************************************************************************************************/

} // namespace

/****************************************************************************************************/

int main() {
    std::cout << "test1:" << std::endl;
    test1();
    std::cout << "\n";

    std::cout << "test2:" << std::endl;
    test2();
    std::cout << "\n";

    std::cout << "test3:" << std::endl;
    test3();
    std::cout << "\n";

    std::cout << "test4:" << std::endl;
    test4();
    std::cout << "\n";


    std::cout << "test5:" << std::endl;
    test4();
    std::cout << "\n";

    return 0;
}

/****************************************************************************************************/
