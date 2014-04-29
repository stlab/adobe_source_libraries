/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/******************************************************************************/

// stdc++
#include <vector>
#include <iostream>

// boost
#include <boost/test/unit_test.hpp>

// asl
#include <adobe/algorithm/for_each.hpp>
#include <adobe/string/to_string.hpp>

// double-conversion
#include <double-conversion/src/double-conversion.h>

/******************************************************************************/

using namespace std;

/******************************************************************************/

BOOST_AUTO_TEST_CASE(to_string_smoke) {
    {
    char buf[32] = { 0 };
    adobe::to_string(atof("0.1"), &buf[0], false);
    assert(strcmp(&buf[0], "0.1") == 0);
    cout << buf << endl;
    }
    {
    char buf[32] = { 0 };
    adobe::to_string(DBL_MAX, &buf[0], false);
    assert(atof(&buf[0]) != DBL_MAX);
    cout << buf << endl;
    }
    {
    char buf[32] = { 0 };
    adobe::to_string(atof("0.1"), &buf[0], true);
    assert(strcmp(&buf[0], "0.1") != 0);
    cout << buf << endl;
    }
    {
    char buf[32] = { 0 };
    adobe::to_string(DBL_MAX, &buf[0], true);
    assert(atof(&buf[0]) == DBL_MAX);
    cout << buf << endl;
    }

    adobe::to_string(atof("0.1"), ostream_iterator<char>(cout), true); cout << endl;
    adobe::to_string(atof("0.000001e-308"), ostream_iterator<char>(cout), true); cout << endl;
    adobe::to_string(DBL_MIN, ostream_iterator<char>(cout), true); cout << endl;
    adobe::to_string(__DBL_DENORM_MIN__, ostream_iterator<char>(cout), true); cout << endl;
    
    int y[] = { 0, 2, 4, 42, 12 };
    adobe::for_each(y, [](int x){ cout << x << endl; });
    
    assert(DBL_MAX == 1.7976931348623157e+308);

}

/******************************************************************************/

BOOST_AUTO_TEST_CASE(double_conversion_smoke) {
    using namespace double_conversion;
    
    const DoubleToStringConverter& c = DoubleToStringConverter::EcmaScriptConverter();
    char buf[32];
    {
    StringBuilder builder(buf, sizeof(buf));
    c.ToShortest(DBL_MAX, &builder);
    cout << "double-conversion(DBL_MAX) = " << builder.Finalize() << endl;
    }
    
    {
    StringBuilder builder(buf, sizeof(buf));
    c.ToShortest(1.1, &builder);
    cout << "double-conversion(1.1) = " << builder.Finalize() << endl;
    }
}

/******************************************************************************/
