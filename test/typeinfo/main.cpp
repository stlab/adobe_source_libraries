/*
	Copyright 2005-2007 Adobe Systems Incorporated
	Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
	or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <boost/test/unit_test.hpp>

#include <adobe/typeinfo.hpp>

#include <cstring>
#include <iostream>

/*************************************************************************************************/
namespace {

class type { };
class forward;

class named { };
template <typename T0, typename T1>
class parameterized { };

} // namespace

ADOBE_NAME_TYPE_0("named", named);
ADOBE_NAME_TYPE_2("parameterized", parameterized<T0, T1>);

void typeinfo_test()
{
    using namespace adobe;

    { // basic test
    type_info_t x = adobe::type_info<double>();
    type_info_t y = adobe::type_info<float>();
    
    double z;
    
    BOOST_CHECK(x.before(y));
    BOOST_CHECK(std::less<type_info_t>()(x, y));
    BOOST_CHECK(std::strcmp(x.name(), "double") == 0);
    BOOST_CHECK(!x.requires_std_rtti());
    BOOST_CHECK(x == adobe::type_info(z));
    }
    { // rtti test
    type_info_t x = adobe::type_info<type>();
    type_info_t y = adobe::type_info<int>();
    type_info_t z = adobe::type_info<forward>();
    
    BOOST_CHECK(!x.before(y));
    BOOST_CHECK(x.requires_std_rtti());
    BOOST_CHECK(x != z);
    BOOST_CHECK(x.before(z) || z.before(x));
    }
    { // check named types
    named x;
    BOOST_CHECK(std::strcmp(adobe::type_info(x).name(), "named") == 0);
    BOOST_CHECK(!adobe::type_info(x).requires_std_rtti());
    }
    { // check complex types
    type_info_t x = adobe::type_info<parameterized<int, float> >();
    type_info_t y = adobe::type_info<parameterized<double, char> >();
    type_info_t z = adobe::type_info<parameterized<double, type> >();
    
    parameterized<int, float> xv;

    BOOST_CHECK(std::strcmp(x.name(), y.name()) == 0);
    BOOST_CHECK(!x.requires_std_rtti() && !y.requires_std_rtti());
    BOOST_CHECK(z.requires_std_rtti());
    BOOST_CHECK(x != y && x != z);
    BOOST_CHECK(std::strcmp(adobe::type_info(&xv).name(), "const") == 0);
    BOOST_CHECK(!adobe::type_info(&xv).requires_std_rtti());
    }
    { // check operator << and serialization
    type_info_t x = adobe::type_info<parameterized<double, type> >();
#ifndef NDEBUG
	//simple coverage test for serialization (debug only)
	std::cout << x << std::endl;
#endif
    serialize(x, std::ostream_iterator<char>(std::cout));
    std::cout << std::endl;
    }
}


using namespace boost::unit_test;

test_suite*
init_unit_test_suite( int , char* [] ) 
{
    framework::master_test_suite().
        add( BOOST_TEST_CASE( &typeinfo_test ) );

    return 0;
}

