/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>
#include <boost/test/unit_test.hpp>
#include <adobe/utility/ignore_unused.hpp>


namespace adobe {

/*! 
\addtogroup testing
@{
*/

template <typename T, typename Op>
void check_transitive(const T& x, const T& y, const T& z, Op op)
{
    ignore_unused(y);
    assert(op(x,y) && op(y,z));
    BOOST_CHECK_MESSAGE(op(x,z), "tranisitive");
}

template <typename T, typename Op>
void check_irreflexive(const T& x, Op op)
{
    BOOST_CHECK_MESSAGE(!op(x,x), "irreflexive");
}

template <typename T, typename Op>
void check_antisymmetric(const T& x, const T& y, Op op)
{
    BOOST_CHECK_MESSAGE(!(op(x,y) && op(y,x)), "anti-symmetric");
}


template <typename T, typename Op>
void check_less_than_comparable(const T& x, const T& y, const T& z, Op op)
{
    check_irreflexive(x, op);
    check_antisymmetric(x, y, op);
    check_transitive(x, y, z, op);
}

//!@}
}

