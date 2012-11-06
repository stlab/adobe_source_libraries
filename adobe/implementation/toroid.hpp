/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/****************************************************************************************************/

#ifndef ADOBE_TOROID_HPP
#define ADOBE_TOROID_HPP

/****************************************************************************************************/

#include <adobe/config.hpp>

#include <boost/array.hpp>
#include <boost/noncopyable.hpp>

#include <list>

/****************************************************************************************************/

namespace adobe {

/****************************************************************************************************/

struct toroid_header_t;

/****************************************************************************************************/

struct toroid_node_t
{
    toroid_node_t*      left_m;     // node to the left
    toroid_node_t*      right_m;    // node to the right
    toroid_node_t*      up_m;       // node to the up
    toroid_node_t*      down_m;     // node to the down
    toroid_header_t*    column_m;   // header at head of this column
    toroid_header_t*    row_m;      // header at head of this row
    char                color_m;
#ifndef NDEBUG
    bool                covered_m;  // debugging; is this node covered
#endif
};

/****************************************************************************************************/

struct toroid_header_t : public toroid_node_t
{
    std::size_t size_m; // number of 1s in the row/column
};

/****************************************************************************************************/

class byte_toroid_t : boost::noncopyable
{
private:
    enum
    {
        block_size_k = 512
    };

    typedef boost::array<toroid_node_t, block_size_k> node_block_t;
    typedef std::list<node_block_t>                   data_set_t;
    typedef std::vector<toroid_header_t>              row_header_set_t;
    typedef std::vector<toroid_header_t>              col_header_set_t;
    typedef row_header_set_t::iterator                row_header_set_iterator_t;
    typedef col_header_set_t::iterator                col_header_set_iterator_t;

public:
    byte_toroid_t(std::size_t row_count, std::size_t column_count) :
        data_set_m(1, node_block_t()),
        row_header_set_m(row_count, toroid_header_t()),
        col_header_set_m(column_count, toroid_header_t()),
        node_block_count_m(0),
        finalized_m(false)
    {
        for (row_header_set_iterator_t first(row_header_set_m.begin()),
             last(row_header_set_m.end()); first != last; ++first)
        {
            toroid_header_t* this_row(&(*first));

            first->left_m = this_row;
            first->right_m = this_row;
        }

        col_header_set_iterator_t front_i(col_header_set_m.begin());
        col_header_set_iterator_t back_i(col_header_set_m.end() - 1);

        for (col_header_set_iterator_t first(col_header_set_m.begin()),
             last(col_header_set_m.end()); first != last; ++first)
        {
            toroid_header_t* this_col(&(*first));

            first->up_m = this_col;
            first->down_m = this_col;

            if (first != front_i)
                first->left_m = &(*(first - 1));

            if (first != back_i)
                first->right_m = &(*(first + 1));

            first->size_m = 0;
        }

        front_i->left_m = &header_m;
        back_i->right_m = &header_m;

        header_m.down_m = &(row_header_set_m.front());
        header_m.up_m = &(row_header_set_m.back());
        header_m.right_m = &(col_header_set_m.front());
        header_m.left_m = &(col_header_set_m.back());
        header_m.color_m = 0;
#ifndef NDEBUG
        header_m.covered_m = false;
#endif
    }

    void set(std::size_t row, std::size_t col, char color = 0)
    {
        // prerequisite: no other node can be set to the right or below this one.
        // i.e., you must start with the top-left node and work to the right, then
        // down. i.e., this must be the right-bottom-most node you have set.

        assert (row < row_size() && col < column_size());
        assert (!finalized_m);

        toroid_header_t& row_h(row_header_set_m[row]);
        toroid_header_t& col_h(col_header_set_m[col]);

        if (node_block_count_m == block_size_k)
        {
            data_set_m.push_back(node_block_t());
            node_block_count_m = 0;
        }

        toroid_node_t& node(data_set_m.back()[node_block_count_m]);

        ++node_block_count_m;

        node.left_m = row_h.left_m;
        node.right_m = &row_h;
        row_h.left_m->right_m = &node;
        row_h.left_m = &node;

        node.up_m = col_h.up_m;
        node.down_m = &col_h;
        col_h.up_m->down_m = &node;
        col_h.up_m = &node;

        node.column_m = &col_h;
        node.row_m = &row_h;

        node.color_m = color; // we set the color even if this is a primary node;
                              // in such case it is a user error and will be ignored.
#ifndef NDEBUG
        node.covered_m = false;
#endif
        //++(row_h.size_m); // not used
        ++(col_h.size_m);
    }

    void set_secondary_column(std::size_t col)
    {
        assert (col < column_size());

        // Secondary columns take dancing links to the next step,
        // in that they allow for a column to be optionally used
        // zero or one times. This esentially allows for a
        // condition that can be met once, but no more than once.
        // 
        // According to Knuth, the header for a secondary column
        // should have left and right fields that simply point
        // to itself, so we unlink it from the doubly-linked
        // header column list.

        toroid_header_t& column(col_header_set_m[col]);

        column.left_m->right_m = column.right_m;
        column.right_m->left_m = column.left_m;
        column.right_m = &column;
        column.left_m = &column;
    }

    void finalize()
    {
        assert (!finalized_m);

        // This "covers" the row headers in the toroid, leaving a data
        // structure as described by Knuth's Dancing Links Algorithm.

        for (row_header_set_iterator_t first(row_header_set_m.begin()),
             last(row_header_set_m.end()); first != last; ++first)
        {
            first->left_m->right_m = first->right_m;
            first->right_m->left_m = first->left_m;
        }

        finalized_m = true;
    }

    void cover_column(toroid_header_t* c)
    {
        assert (finalized_m);

        right_of(c)->left_m = left_of(c);
        left_of(c)->right_m = right_of(c);

        for (toroid_node_t* i(down_of(c)); i != c; i = down_of(i))
        {
            for (toroid_node_t* j(right_of(i)); j != i; j = right_of(j))
            {
                down_of(j)->up_m = up_of(j);

                up_of(j)->down_m = down_of(j);

                --(column_of(j)->size_m);

#ifndef NDEBUG
                j->covered_m = true;
#endif
            }
        }

        c->covered_m = true;
    }

    void uncover_column(toroid_header_t* c)
    {
        assert (finalized_m);

        c->covered_m = false;

        for (toroid_node_t* i(up_of(c)); i != c; i = up_of(i))
        {
            for (toroid_node_t* j(left_of(i)); j != i; j = left_of(j))
            {
#ifndef NDEBUG
                j->covered_m = false;
#endif
                ++(column_of(j)->size_m);

                down_of(j)->up_m = j;

                up_of(j)->down_m = j;
            }
        }

        right_of(c)->left_m = c;
        left_of(c)->right_m = c;
    }

    void purify(toroid_node_t* p)
    {
        toroid_header_t* c(column_of(p));
        int              x(p->color_m);

        c->color_m = x;

        for (toroid_node_t* rr = down_of(c); rr != c; rr = down_of(rr))
        {
            if (rr->color_m != x)
            {
                for (toroid_node_t* nn = right_of(rr); nn != rr; nn = right_of(nn))
                {
                    up_of(nn)->down_m = down_of(nn);
                    down_of(nn)->up_m = up_of(nn);
                    column_of(nn)->size_m--;
#ifndef NDEBUG
                    nn->covered_m = true;
#endif
                }
            }
            else if (rr != p)
            {
                rr->color_m = -1;
            }
        }
    }

    void unpurify(toroid_node_t* p)
    {
        toroid_header_t* c(column_of(p));
        int              x(p->color_m);

        for (toroid_node_t* rr = up_of(c); rr != c; rr = up_of(rr))
        {
            if (rr->color_m < 0)
            {
                rr->color_m = x;
            }
            else if (rr != p)
            {
                for (toroid_node_t* nn = left_of(rr); nn != rr; nn = left_of(nn))
                {
                    up_of(nn)->down_m = nn;
                    down_of(nn)->up_m = nn;
                    column_of(nn)->size_m++;
#ifndef NDEBUG
                    nn->covered_m = false;
#endif
                }
            }
        }

        c->color_m = 0;
    }

    std::size_t column_size() const
        { return col_header_set_m.size(); }
    std::size_t row_size() const
        { return row_header_set_m.size(); }

    // This leverages the row headers used to initialize the data structure by
    // taking the distance from the start of the row header list to the row
    // header referenced by this node.

    std::size_t row_index_of(toroid_node_t* node)
        { return static_cast<std::size_t>(std::distance(&(*(row_header_set_m.begin())), node->row_m)); }

    std::size_t column_index_of(toroid_node_t* node)
        { return static_cast<std::size_t>(std::distance(&(*(col_header_set_m.begin())), node->column_m)); }

    template <typename ToroidDataPointer>
    static inline ToroidDataPointer left_of(ToroidDataPointer node)
        { assert(node->left_m != 0); return reinterpret_cast<ToroidDataPointer>(node->left_m); }

    template <typename ToroidDataPointer>
    static inline ToroidDataPointer right_of(ToroidDataPointer node)
        { assert(node->right_m != 0); return reinterpret_cast<ToroidDataPointer>(node->right_m); }

    static inline toroid_node_t* up_of(toroid_node_t* node)
        { assert(node->up_m != 0); return node->up_m; }

    static inline toroid_node_t* down_of(toroid_node_t* node)
        { assert(node->down_m != 0); return node->down_m; }

    static inline toroid_header_t* column_of(toroid_node_t* node)
        { assert(node->column_m != 0); return node->column_m; }

    toroid_header_t header_m;

#if ADOBE_STD_SERIALIZATION
    friend std::ostream& operator<< (std::ostream& s, const byte_toroid_t& toroid);
#endif

private:
    data_set_t          data_set_m;
    row_header_set_t    row_header_set_m;
    col_header_set_t    col_header_set_m;
    std::size_t         node_block_count_m;
    bool                finalized_m;
};

#if ADOBE_STD_SERIALIZATION
std::ostream& operator<<(std::ostream& s, const byte_toroid_t& toroid)
{
    const std::size_t cols_k(toroid.column_size());
    const std::size_t rows_k(toroid.row_size());

    byte_toroid_t& nctoroid(const_cast<byte_toroid_t&>(toroid));

    s << "    |";

    for (std::size_t i(0); i < cols_k; ++i)
    {
        s << i/10 << "|";
    }
    s << std::endl << "    |";
    for (std::size_t i(0); i < cols_k; ++i)
    {
        s.width(1);
        s << i%10 << "|";
    }

    s<< std::endl << "    |";

    for (std::size_t i(0); i < cols_k; ++i)
    {
        const toroid_header_t* col(&toroid.col_header_set_m[i]);

        if (toroid.right_of(col) == toroid.left_of(col) &&
            toroid.right_of(col) == col)
        {
            s << "*|";
        }
        else if (col->covered_m)
        {
            s << "X|";
        }
        else
        {
            s << " |";
        }
    }

    s << std::endl << "----|";
    for (std::size_t i(0); i < cols_k; ++i)
        s << "-|";

    for (std::size_t i(0); i < rows_k; ++i)
    {
        toroid_header_t* row(&nctoroid.row_header_set_m[i]);

        s.width(4);

        s << std::endl << i << "|";

        std::size_t cur_col(0);

        toroid_node_t* first(nctoroid.right_of(row));
        toroid_node_t* next(first);

        if (!toroid.finalized_m)
        {
            first = row;
        }

        bool first_time(true);

        while (next != first || first_time)
        {
            first_time = false;

            std::size_t      column(nctoroid.column_index_of(next));
            toroid_header_t* col_h(nctoroid.column_of(next));

            for (; cur_col < column; ++cur_col)
                s << " |";

            if (col_h->covered_m)
            {
                s << "X|";
            }
            else
            {
                if (next->color_m > 0)
                    s << next->color_m << "|"; // colored, not purified
                else if (next->color_m < 0)
                    s << "P|"; // colored and purified
                else
                    s << "*|"; // uncolored
            }

            ++cur_col;

            next = toroid.right_of(next);
        }

        while (cur_col != cols_k)
        {
            s << " |";
            ++cur_col;
        }
    }

    return s;
}
#endif

/****************************************************************************************************/

} // namespace adobe

/****************************************************************************************************/

#endif

/****************************************************************************************************/
