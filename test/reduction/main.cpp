/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/algorithm.hpp>
#include <adobe/algorithm/rotate.hpp>
#include <adobe/algorithm/reduce.hpp>
#include <adobe/algorithm/other_of.hpp>
#include <adobe/iterator/value_iterator.hpp>

#include <boost/next_prior.hpp>

#include <utility>
#include <vector>
#include <iostream>

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

template <typename I> // I models ForwardIterator
struct combine_ranges : std::binary_function<std::pair<I, I>, std::pair<I, I>, std::pair<I, I> >
{
    typedef std::pair<I, I> second_argument_type;
    typedef std::pair<I, I> first_argument_type;
    typedef std::pair<I, I> result_type;

    result_type operator()(const first_argument_type& x, const second_argument_type& y) const
    {
        std::cout << "merging ( ";
        std::copy(x.first, x.second, std::ostream_iterator<ADOBE_VALUE_TYPE(I)>(std::cout, " "));
        std::cout << ") and ( ";
        std::copy(y.first, y.second, std::ostream_iterator<ADOBE_VALUE_TYPE(I)>(std::cout, " "));
        std::cout << ") yields ( ";

        result_type result(make_pair(adobe::other_of(adobe::rotate(x.first, x.second, y.first), x.second), y.second));

        std::copy(result.first, result.second, std::ostream_iterator<ADOBE_VALUE_TYPE(I)>(std::cout, " "));
        std::cout << ")" << std::endl;

        return result;
    }
};

/**************************************************************************************************/

template <typename T>
struct myplus : std::binary_function<T, T, T>
{
    typedef T second_argument_type;
    typedef T first_argument_type;
    typedef T result_type;

    result_type operator()(const first_argument_type& x, const second_argument_type& y) const
    {
        result_type result(x+y);

        std::cout << x << " + " << y << " = " << result << std::endl;

        return result;
    }
};

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

template <typename T>
struct identity_element<myplus<T> >
{
    typedef T result_type;

    result_type operator()() const
    { return T(0); }
};

/**************************************************************************************************/

} // namespace adobe

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

template <typename I, // I models ForwardIterator
          typename P> // P models UnaryPredicate
struct partition_trivial : std::unary_function<I, std::pair<I, I> >
{
    P p;

    partition_trivial(const P & x) : p(x)
    { }

    std::pair<I, I> operator()(I i) const
    {
        if (p(*i))
            return std::make_pair(i, i);
        else
            return std::make_pair(i, boost::next(i));
    }
};

/**************************************************************************************************/

template <typename I, // I models ForwardIterator
          typename P> // P models UnaryPredicate
I stable_partition_inplace_iterative(I f, I l, P p)
{
    typedef partition_trivial<I, P>                  fun_t;
    typedef typename adobe::value_iterator<I, fun_t> val_iter;

    fun_t fun(p);

    std::pair<I, I> z(l, l);

    combine_ranges<I> op;

    val_iter f1(f, fun);

    val_iter l1(l, fun);

    std::pair<I, I> result(adobe::reduce_balanced(f1, l1, op, z));

    return result.first;
}  

/**************************************************************************************************/

template <typename T>
struct is_odd : std::unary_function<T, bool>
{
    typedef bool result_type;

    bool operator()(const T& x) const
    { return x & 1; }
};

/**************************************************************************************************/

void test1()
{
    std::vector<int> set(20);

    adobe::iota(set.begin(), set.end(), 1);

    adobe::copy(set, std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;

    stable_partition_inplace_iterative(set.begin(), set.end(), is_odd<int>());

    adobe::copy(set, std::ostream_iterator<int>(std::cout, " "));

    std::cout << std::endl;
}

/**************************************************************************************************/

void test2()
{
    std::vector<double> set(20);

    adobe::iota(set.begin(), set.end(), 1);

    adobe::copy(set, std::ostream_iterator<double>(std::cout, " "));
    std::cout << std::endl;

    // just to test std::plus
    std::cout << adobe::reduce_balanced(set.begin(), set.end(), std::plus<double>()) << std::endl;

    // just to test std::multiplies
    std::cout << adobe::reduce_balanced(set.begin(), set.end(), std::multiplies<double>()) << std::endl;

    // now a more verbose version
    std::cout << adobe::reduce_balanced(set.begin(), set.end(), myplus<double>()) << std::endl;
}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

int main()
{
    test1();

    test2();
}
