/*
    Copyright 2007 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/config.hpp>

#define BOOST_TEST_MAIN

#include <cstdlib>
#include <string>

#include <boost/test/unit_test.hpp>

#include <adobe/poly_copyable.hpp>
#include <adobe/poly_regular.hpp>

double adobe_test_size(int x) { return static_cast<double>(std::abs(static_cast<int>(x))); }

#include "poly_measurable.hpp"
#include "poly_measurable2d.hpp"

using namespace adobe_test;

/**************************************************************************************************/

// size for some basic types


// not needed, default: inline double size(const std::string& x) { return x.size(); }

// Compile time polymorphism requiring MeasurableConcept

template <class M1, class M2> // M1 models Measurable, M2 models Measurable
double max_size(const M1& x, const M2& y) {
    boost::function_requires<MeasurableConcept<M1>>();
    boost::function_requires<MeasurableConcept<M2>>();

    return std::max(MeasurableConcept<M1>::size(x), MeasurableConcept<M2>::size(y));
}

// Compile time test

BOOST_AUTO_TEST_CASE(compile_time_poly) {
    // tests compile-time generic max_size function template
    BOOST_CHECK_EQUAL(max_size(4, 5), 5.0);
    BOOST_CHECK_EQUAL(max_size(4, std::string("hello")), 5.0);
}


/**************************************************************************************************/

// *runtime* generic function
// (not a template)

double max_amount_poly(const poly_measurable& x, const poly_measurable& y) {
    return std::max(x.size(), y.size());
}

BOOST_AUTO_TEST_CASE(run_time_poly) {
    // tests compile-time generic max_size function template
    poly_measurable four(4), five(5);
    poly_measurable y(std::string("hello"));

    BOOST_CHECK_EQUAL(max_amount_poly(four, five), 5.0);
    BOOST_CHECK_EQUAL(max_amount_poly(four, y), 5.0);
}


/**************************************************************************************************/

struct concrete_measurable2d {
    concrete_measurable2d(double size, double height) : size_m(size), height_m(height) {}
    concrete_measurable2d() : size_m(0.0), height_m(0.0) {}
    double size() const { return size_m; }
    double height() const { return height_m; }

    double size_m;
    double height_m;
};

bool operator==(const concrete_measurable2d& x, const concrete_measurable2d& y) {
    return x.size() == y.size() && x.height() == y.height();
}

bool operator!=(const concrete_measurable2d& x, const concrete_measurable2d& y) {
    return !(x == y);
}

inline double perimeter(const poly_measurable2d& x) { return 2 * (x.size() + x.height()); }

BOOST_AUTO_TEST_CASE(refinement) {
    concrete_measurable2d a(2, 3), b(1, 4);

    poly_measurable ma(a), mb(b);
    BOOST_CHECK_EQUAL(ma.size(), mb.size() + 1);
    BOOST_CHECK_THROW(adobe::poly_cast<poly_measurable2d&>(ma), std::bad_cast);
    BOOST_CHECK_EQUAL(adobe::poly_cast<poly_measurable2d*>(&ma) == NULL, true);
    poly_measurable2d m2a(a), m2b(b);
    BOOST_CHECK_EQUAL(ma.size(), m2a.size());
    BOOST_CHECK_EQUAL(perimeter(m2a), perimeter(m2b));
    BOOST_CHECK_EQUAL(max_amount_poly(adobe::poly_cast<poly_measurable&>(m2a),
                                      adobe::poly_cast<poly_measurable&>(m2b)),
                      2);
    poly_measurable x(m2a);
    BOOST_CHECK_EQUAL(x.size(), m2a.size());
}

BOOST_AUTO_TEST_CASE(assign) {
    concrete_measurable2d a(2, 3), b(1, 4);
    poly_measurable ma(a), mb(b);
    poly_measurable2d m2a(a), m2b(b);

    m2b.assign(a);
    BOOST_CHECK_EQUAL(m2a.size(), m2b.size());
    BOOST_CHECK_EQUAL(m2a.height(), m2b.height());

    int i = 1;

    ma.assign(i);
}

BOOST_AUTO_TEST_CASE(move_test) {
    concrete_measurable2d a(2, 3), b(1, 4);
    poly_measurable ma(a), mb(b);
    poly_measurable2d m2a(a), m2b(b);

    poly_measurable mc(std::move(ma));
}


BOOST_AUTO_TEST_CASE(poly_copyable) {
    using namespace adobe;
    concrete_measurable2d a(2, 3), b(1, 4);
    {
        poly<copyable> ma(a), mb(b);
        poly<copyable> m2a(a), m2b(b);

        poly<copyable> mc(std::move(ma));
    }
    {
        poly<regular> ma(a), mb(b);
        poly<regular> m2a(a), m2b(b);

        // poly<regular> mc(ma, move_ctor());
        BOOST_CHECK(true);
        BOOST_CHECK(ma == m2a);
        BOOST_CHECK(ma != poly<regular>());
    }
}
