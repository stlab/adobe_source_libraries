/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
#include <adobe/config.hpp>
/****************************************************************************************************/

#ifndef SUDOKU_HPP
#define SUDOKU_HPP

/****************************************************************************************************/

#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_adaptor.hpp>

#include <string>
#include <stdexcept>

/****************************************************************************************************/

namespace bfs = boost::filesystem;

/****************************************************************************************************/

namespace sudoku {

/****************************************************************************************************/

template <typename Iterator>
int get_box(Iterator i)
{
#if 0
    switch (i.row())
    {
        case 0: case 1: case 2:
        switch (i.col())
        {
            case 0: case 1: case 2: return 0;
            case 3: case 4: case 5: return 1;
            case 6: case 7: case 8: return 2;
        }

        case 3: case 4: case 5:
        switch (i.col())
        {
            case 0: case 1: case 2: return 3;
            case 3: case 4: case 5: return 4;
            case 6: case 7: case 8: return 5;
        }

        case 6: case 7: case 8:
        switch (i.col())
        {
            case 0: case 1: case 2: return 6;
            case 3: case 4: case 5: return 7;
            case 6: case 7: case 8: return 8;
        }
    }

    throw std::runtime_error("get_box");
#else
    return (i.row() / 3) * 3 + (i.col() / 3);
#endif
}

/****************************************************************************************************/

template <typename Iterator>
class row_iterator_adaptor : public boost::iterator_adaptor<row_iterator_adaptor<Iterator>, Iterator>
{
public:
    typedef typename boost::iterator_adaptor<row_iterator_adaptor<Iterator>, Iterator>::reference reference;

    row_iterator_adaptor() { }

    row_iterator_adaptor(Iterator iterator) : row_iterator_adaptor::iterator_adaptor_(iterator)
    { }

    template <class U> row_iterator_adaptor(const row_iterator_adaptor<U>& iterator) :
        row_iterator_adaptor::iterator_adaptor_(iterator.base())
    { }

    int col() const { return this->base_reference().col_m; }
    int row() const { return this->base_reference().row_m; }

    const char* traversal_subject_type() const { return "Row"; }
    std::size_t traversal_subject_identifier() const { return row()+1; }

private:
    friend class boost::iterator_core_access;

    void increment() { this->base_reference().inc_col(); }
    void decrement() { this->base_reference().dec_col(); }
    reference dereference() const { return *this->base(); }
};

/****************************************************************************************************/

template <typename Iterator>
class col_iterator_adaptor : public boost::iterator_adaptor<col_iterator_adaptor<Iterator>, Iterator>
{
public:
    typedef typename boost::iterator_adaptor<col_iterator_adaptor<Iterator>, Iterator>::reference reference;

    col_iterator_adaptor() { }

    col_iterator_adaptor(Iterator iterator) : col_iterator_adaptor::iterator_adaptor_(iterator)
    { }

    template <class U> col_iterator_adaptor(const col_iterator_adaptor<U>& iterator) :
        col_iterator_adaptor::iterator_adaptor_(iterator.base())
    { }

    int col() const { return this->base_reference().col_m; }
    int row() const { return this->base_reference().row_m; }

    const char* traversal_subject_type() const { return "Col"; }
    std::size_t traversal_subject_identifier() const { return col()+1; }

private:
    friend class boost::iterator_core_access;

    void increment() { this->base_reference().inc_row(); }
    void decrement() { this->base_reference().dec_row(); }
    reference dereference() const { return *this->base(); }
};

/****************************************************************************************************/

template <typename Iterator>
class box_iterator_adaptor : public boost::iterator_adaptor<box_iterator_adaptor<Iterator>, Iterator>
{
public:
    typedef typename boost::iterator_adaptor<box_iterator_adaptor<Iterator>, Iterator>::reference reference;

    box_iterator_adaptor() { }

    box_iterator_adaptor(Iterator iterator) : box_iterator_adaptor::iterator_adaptor_(iterator)
    { }

    template <class U> box_iterator_adaptor(const box_iterator_adaptor<U>& iterator) :
        box_iterator_adaptor::iterator_adaptor_(iterator.base())
    { }

    int col() const { return this->base_reference().col_m; }
    int row() const { return this->base_reference().row_m; }

    const char* traversal_subject_type() const { return "Box"; }
    std::size_t traversal_subject_identifier() const { return get_box(*this)+1; }

private:
    friend class boost::iterator_core_access;

    void increment()
    {
        if (++(this->base_reference().col_m) % 3 == 0)
        {
            this->base_reference().col_m -= 3;
            this->base_reference().inc_row();
        }
    }

    void decrement()
    {
        if ((this->base_reference().col_m)-- % 3 == 0)
        {
            this->base_reference().col_m += 3;
            this->base_reference().dec_row();
        }
    }

    reference dereference() const { return *this->base(); }
};

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

class possibility_set_t
{
public:
    typedef boost::array<bool, 9>   set_t;
    typedef set_t::iterator         iterator;
    typedef set_t::const_iterator   const_iterator;

    explicit possibility_set_t(bool default_to = true)
        { reset_to(default_to); }

    possibility_set_t(const possibility_set_t& rhs) :
        set_m(rhs.set_m)
    { }

    possibility_set_t& operator = (const possibility_set_t& rhs)
    {
        set_m = rhs.set_m;
        return *this;
    }

    void reset_to(bool value)
    {
        for (iterator first(set_m.begin()), last(set_m.end()); first != last; ++first)
            *first = value;
    }

    iterator begin() { return set_m.begin(); }
    iterator end() { return set_m.end(); }

    const_iterator begin() const { return set_m.begin(); }
    const_iterator end() const { return set_m.end(); }

    std::size_t size() const { return set_m.size(); }

    std::size_t count_possibilities() const
        { return std::accumulate(set_m.begin(), set_m.end(), 0); }
    std::size_t count_impossibilities() const
        { return 9 - count_possibilities(); }

    bool includes(const possibility_set_t& rhs) const
    {
        for (const_iterator first1(set_m.begin()), last1(set_m.end()),
            first2(rhs.set_m.end()); first1 != last1; ++first1)
            if (*first1 && !*first2) return false;

        return true;
    }

    // This is the reason we have a possibility_set_t : off-by-one access

    bool& operator [] (std::size_t x)
        { return set_m[x - 1]; }
    const bool& operator [] (std::size_t x) const
        { return set_m[x - 1]; }

private:
    set_t set_m;
};

/****************************************************************************************************/

inline std::ostream& operator << (std::ostream& s, const possibility_set_t& x)
{
    s << "{";

    std::size_t count(0);

    for (possibility_set_t::const_iterator first(x.begin()), last(x.end());
        first != last; ++first)
    {
        ++count;
        if (*first)
            s << " " << count;
    }

    s << " }";

    return s;
}

/****************************************************************************************************/

inline possibility_set_t operator | (const possibility_set_t& x, const possibility_set_t& y)
{
    possibility_set_t result(x);

    for (int i(1); i <= 9; ++i)
        result[i] = result[i] || y[i];

    return result;
}

/****************************************************************************************************/

inline possibility_set_t& operator |= (possibility_set_t& x, const possibility_set_t& y)
{
    x = x | y;
    return x;
}

/****************************************************************************************************/

inline possibility_set_t operator & (const possibility_set_t& x, const possibility_set_t& y)
{
    possibility_set_t result(x);

    for (int i(1); i <= 9; ++i)
        result[i] = result[i] && y[i];

    return result;
}

/****************************************************************************************************/

inline possibility_set_t& operator &= (possibility_set_t& x, const possibility_set_t& y)
{
    x = x & y;
    return x;
}

/****************************************************************************************************/

inline possibility_set_t operator - (const possibility_set_t& x, const possibility_set_t& y)
{
    possibility_set_t result;

    for (int i(1); i <= 9; ++i)
        result[i] = x[i] & !y[i];

    return result;
}

/****************************************************************************************************/

inline possibility_set_t& operator -= (possibility_set_t& x, const possibility_set_t& y)
{
    x = x - y;
    return x;
}

/****************************************************************************************************/

inline possibility_set_t operator + (const possibility_set_t& x, const possibility_set_t& y)
{ return x | y; }

/****************************************************************************************************/

inline possibility_set_t& operator += (possibility_set_t& x, const possibility_set_t& y)
{
    x = x + y;
    return x;
}

/****************************************************************************************************/

inline bool operator == (const possibility_set_t& x, const possibility_set_t& y)
{
    for (int i(1); i <= 9; ++i)
        if (x[i] != y[i])
            return false;

    return true;
}

/****************************************************************************************************/

inline bool operator != (const possibility_set_t& x, const possibility_set_t& y)
    { return !(x == y); }

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

class square_t
{
public:
    square_t() :
        value_m(0)
    { }

    square_t(const square_t& rhs) :
        value_m(rhs.value_m), possibilities_m(rhs.possibilities_m)
    { }

    square_t& operator = (const square_t& rhs)
    {
        value_m = rhs.value_m;
        possibilities_m = rhs.possibilities_m;
        return *this;
    }

    int                 value_m;
    possibility_set_t   possibilities_m;
};

/****************************************************************************************************/

#if 0
#pragma mark -
#endif

/****************************************************************************************************/

class sudoku_t
{
public:
    square_t& square(int row, int col)
        { return grid_m[col * 9 + row]; }
    const square_t& square(int row, int col) const
        { return grid_m[col * 9 + row]; }

    std::string name_m;

    template <class Value>
    class iter_base :
        public boost::iterator_facade<iter_base<Value>, Value, std::bidirectional_iterator_tag>
    {
    public:
        iter_base() :
            row_m(0), col_m(0), g_m(0)
        { }

        explicit iter_base(Value* p, int row = 0, int col = 0) :
            row_m(row), col_m(col), g_m(p)
        { }

        void inc_row() { ++row_m; }
        void dec_row() { --row_m; }

        void inc_col() { ++col_m; }
        void dec_col() { --col_m; }

        int col() { return col_m; }
        int row() { return row_m; }

        int row_m;
        int col_m;

    private:
        friend class boost::iterator_core_access;

        void increment()
        {
            if (++col_m == 9)
            {
                col_m = 0;
                ++row_m;
            }
        }

        void decrement()
        {
            if (col_m-- == 0)
            {
                col_m = 8;
                --row_m;
            }
        }

        bool equal(iter_base const& other) const
            { return this->row_m == other.row_m && this->col_m == other.col_m; }

        Value& dereference() const
            { return g_m[ col_m * 9 + row_m ]; }

    private:
        Value*  g_m; // always refers to square (0, 0) in the puzzle
    };

    typedef iter_base<square_t>         iterator;
    typedef iter_base<const square_t>   const_iterator;

    iterator        at(int row, int col)        { return iterator(&grid_m[0], row, col); }
    const_iterator  at(int row, int col) const  { return const_iterator(&grid_m[0], row, col); }

    iterator begin()    { return at(0, 0); }
    iterator end()      { return at(9, 0); }

    const_iterator begin() const    { return at(0, 0); }
    const_iterator end() const      { return at(9, 0); }

    // REVISIT (fbrereto) : What is to be done about the differing
    //                      end() iterator points between the types?
    //                      end().base() doesn't return the same result
    //                      for the different types; does it matter?

    typedef row_iterator_adaptor<iterator>          row_iterator;
    typedef row_iterator_adaptor<const_iterator>    const_row_iterator;

    row_iterator    row_begin(int row) { return at(row, 0); }
    row_iterator    row_end(int row) { return at(row, 9); }

    const_row_iterator  row_begin(int row) const { return at(row, 0); }
    const_row_iterator  row_end(int row) const { return at(row, 9); }

    typedef col_iterator_adaptor<iterator>          col_iterator;
    typedef col_iterator_adaptor<const_iterator>    const_col_iterator;

    col_iterator    col_begin(int col) { return at(0, col); }
    col_iterator    col_end(int col) { return at(9, col); }

    const_col_iterator  col_begin(int col) const { return at(0, col); }
    const_col_iterator  col_end(int col) const { return at(9, col); }

    typedef box_iterator_adaptor<iterator>          box_iterator;
    typedef box_iterator_adaptor<const_iterator>    const_box_iterator;

    box_iterator box_begin(int box)
        { std::pair<int, int> c(box_begin_coord(box)); return at(c.first, c.second); }
    box_iterator box_end(int box)
        { std::pair<int, int> c(box_end_coord(box)); return at(c.first, c.second); }

    const_box_iterator box_begin(int box) const
        { std::pair<int, int> c(box_begin_coord(box)); return at(c.first, c.second); }
    const_box_iterator box_end(int box) const
        { std::pair<int, int> c(box_end_coord(box)); return at(c.first, c.second); }

private:
    static std::pair<int, int> box_begin_coord(int box)
    {
        switch (box)
        {
            case 0: return std::make_pair(0, 0);
            case 1: return std::make_pair(0, 3);
            case 2: return std::make_pair(0, 6);
            case 3: return std::make_pair(3, 0);
            case 4: return std::make_pair(3, 3);
            case 5: return std::make_pair(3, 6);
            case 6: return std::make_pair(6, 0);
            case 7: return std::make_pair(6, 3);
            case 8: return std::make_pair(6, 6);
            default: throw std::runtime_error("box_begin_coord");
        }
    }

    static std::pair<int, int> box_end_coord(int box)
    {
        switch (box)
        {
            case 0: return std::make_pair(3, 0);
            case 1: return std::make_pair(3, 3);
            case 2: return std::make_pair(3, 6);
            case 3: return std::make_pair(6, 0);
            case 4: return std::make_pair(6, 3);
            case 5: return std::make_pair(6, 6);
            case 6: return std::make_pair(9, 0);
            case 7: return std::make_pair(9, 3);
            case 8: return std::make_pair(9, 6);
            default: throw std::runtime_error("box_end_coord");
        }
    }

    square_t grid_m[81];
};

/****************************************************************************************************/

std::ostream& operator << (std::ostream& s, const sudoku_t& x)
{
    sudoku_t::const_iterator    first(x.begin());
    sudoku_t::const_iterator    last(x.end());
    int                         col_count(0);
    int                         row_count(0);
    const char*                 hr("------+-------+------");

    for (; first != last; ++first)
    {
        if (first->value_m)
            s << first->value_m;
        else
            s << '.';

        ++col_count;

        if (col_count % 9 == 0)
        {
            ++row_count;

            if (row_count == 3 || row_count == 6)
                s << std::endl << hr;

            s << std::endl;
        }
        else if (col_count % 3 == 0)
            s << " | ";
        else
            s << ' ';
    }

    return s;
}

/****************************************************************************************************/

} // namespace sudoku

/****************************************************************************************************/

#endif

/****************************************************************************************************/
