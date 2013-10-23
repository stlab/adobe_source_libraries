/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_dancing_links_t_HPP
#define ADOBE_dancing_links_t_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <vector>

#if ADOBE_STD_SERIALIZATION
#include <iostream>
#endif

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <adobe/implementation/toroid.hpp>

#define ADOBE_DLX_VERBOSE 0

#if ADOBE_STD_SERIALIZATION && ADOBE_DLX_VERBOSE
#include <adobe/iomanip.hpp>
#endif

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

#ifndef ADOBE_NO_DOCUMENTATION

/****************************************************************************************************/

namespace implementation {

/****************************************************************************************************/

struct do_nothing_callback_t
{
    inline void operator () (std::size_t, bool) const
    { }
};

/****************************************************************************************************/

struct select_right_heuristic_t
{
    inline toroid_header_t* operator () (byte_toroid_t& toroid) const
        { return toroid.right_of(&toroid.header_m); }
};

/****************************************************************************************************/

struct select_most_constrained_heuristic_t
{
    inline toroid_header_t* operator () (byte_toroid_t& toroid) const
    {
        toroid_header_t* c(toroid.right_of(&toroid.header_m));
        std::size_t      sz(c->size_m);

        for (toroid_header_t* p(toroid.right_of(c)); p != &toroid.header_m; p = toroid.right_of(p))
        {
            if (p->size_m < sz)
            {
                c = p;
                sz = p->size_m;
            }

            if (sz == 1) break;
        }

        return c;
    }
};

/****************************************************************************************************/

} // namespace implementation

/****************************************************************************************************/

#endif

/****************************************************************************************************/

class dancing_links_t : boost::noncopyable
{
public:
#ifndef ADOBE_NO_DOCUMENTATION
    dancing_links_t(std::size_t row_count, std::size_t column_count) :
        toroid_m(row_count, column_count),
        output_m(column_count, 0),
        solutions_m(0)
#if ADOBE_DLX_VERBOSE
        , tab_count_m(0)
#endif
    { }
#endif

    inline void set(std::size_t row, std::size_t col, char color = 0)
        { toroid_m.set(row, col, color); }

    inline void set_secondary_column(std::size_t col)
        { toroid_m.set_secondary_column(col); }

    template <typename ResultCallback, typename SearchHeuristic>
    inline std::size_t search(std::size_t     max_solutions,
                              ResultCallback  callback,
                              SearchHeuristic heuristic)
    {
        max_solutions_m = max_solutions;

        toroid_m.finalize();

        do_search(0, callback, heuristic);

        return solutions_m;
    }

    inline std::size_t search(std::size_t max_solutions)
    {
        return search(max_solutions,
                      implementation::do_nothing_callback_t(),
                      implementation::select_most_constrained_heuristic_t());
    }

#if ADOBE_STD_SERIALIZATION
    friend std::ostream& operator<<(std::ostream& s, const dancing_links_t& dancing_links_t)
        { return s << dancing_links_t.toroid_m; }
#endif

private:
    template <typename ResultCallback, typename SearchHeuristic>
    void do_search(std::size_t k, ResultCallback callback, SearchHeuristic heuristic)
    {
        if (toroid_m.right_of(&toroid_m.header_m) == &toroid_m.header_m)
        {
            ++solutions_m;

            #if ADOBE_DLX_VERBOSE
            std::cout << adobe::indents(tab_count_m) << "<solved/>" << std::endl;
            #endif

            for (std::size_t i(0); i < k; ++i)
                callback(toroid_m.row_index_of(output_m[i]), i + 1 == k);

            return;
        }

        std::size_t next_k(k + 1);

        toroid_header_t* c(heuristic(toroid_m));

        #if ADOBE_DLX_VERBOSE
        ++tab_count_m;
        std::cout << adobe::indents(tab_count_m) << "<c" << toroid_m.column_index_of(toroid_m.down_of(c)) << ">" << std::endl;
        #endif

        toroid_m.cover_column(c);

        // branch on each node in this column
        for (toroid_node_t* r(toroid_m.down_of(c)); r != c; r = toroid_m.down_of(r))
        {
            #if ADOBE_DLX_VERBOSE
            std::cout << adobe::indents(tab_count_m) << "<r" << toroid_m.row_index_of(r) << ">" << std::endl;
            #endif

            output_m[k] = r;

            // cover or purify each node on the same row as the node we're branching on
            for (toroid_node_t* j(toroid_m.right_of(r)); j != r; j = toroid_m.right_of(j))
            {
                if (j->color_m == 0)
                {
                    #if ADOBE_DLX_VERBOSE
                    std::cout << adobe::indents(tab_count_m) << "<c" << toroid_m.column_index_of(j) << ">" << std::endl;
                    #endif
                    toroid_m.cover_column(toroid_m.column_of(j));
                }
                else if (j->color_m > 0)
                {
                    #if ADOBE_DLX_VERBOSE
                    std::cout << adobe::indents(tab_count_m) << "<p" << toroid_m.column_index_of(j) << ">" << std::endl;
                    #endif
                    toroid_m.purify(j);
                }
            }

            #if ADOBE_DLX_VERBOSE
            // std::cout << *this << std::endl;
            #endif

            do_search(next_k, callback, heuristic);

            if (solutions_m >= max_solutions_m)
                return;

            r = output_m[k];

            c = toroid_m.column_of(r);

            // undo the cover/purify
            for (toroid_node_t* j(toroid_m.left_of(r)); j != r; j = toroid_m.left_of(j))
            {
                if (j->color_m == 0)
                {
                    #if ADOBE_DLX_VERBOSE
                    std::cout << adobe::indents(tab_count_m) << "</c" << toroid_m.column_index_of(j) << ">" << std::endl;
                    #endif
                    toroid_m.uncover_column(toroid_m.column_of(j));
                }
                else if (j->color_m > 0)
                {
                    #if ADOBE_DLX_VERBOSE
                    std::cout << adobe::indents(tab_count_m) << "</p" << toroid_m.column_index_of(j) << ">" << std::endl;
                    #endif
                    toroid_m.unpurify(j);
                }
            }

            #if ADOBE_DLX_VERBOSE
            std::cout << adobe::indents(tab_count_m) << "</r" << toroid_m.row_index_of(r) << ">" << std::endl;
            #endif
        }

        toroid_m.uncover_column(c);

        #if ADOBE_DLX_VERBOSE
        std::cout << adobe::indents(tab_count_m) << "</c" << toroid_m.column_index_of(toroid_m.down_of(c)) << ">" << std::endl;
        --tab_count_m;
        #endif
    };

    byte_toroid_t               toroid_m;
    std::vector<toroid_node_t*> output_m;
    std::size_t                 solutions_m;
    std::size_t                 max_solutions_m;
#if ADOBE_DLX_VERBOSE
    std::size_t                 tab_count_m;
#endif
};

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
