/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>
#include <boost/test/unit_test.hpp>

namespace adobe {
/*!
\addtogroup testing
@{
*/

template <typename T>
void check_null(const T& x)
{
    T t = T();
    if(t)
        BOOST_ERROR("operator bool");
    BOOST_CHECK_MESSAGE(!t, "operator!");

    BOOST_CHECK_MESSAGE(x, "operator!");
    if(!x)
        BOOST_ERROR("operator bool");

}
//! @}
}

