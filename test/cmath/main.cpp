/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <cassert>
#include <iostream>

#include <adobe/cmath.hpp>

#include <adobe/algorithm.hpp>

/*************************************************************************************************/

int main()
{
    bool result(true);
    
    // test that the types are visible:
    
    std::cout << "adobe::float_t(10.5f) ==" << adobe::float_t(10.5f) << std::endl;
    std::cout << "adobe::double_t(10.5) ==" << adobe::double_t(10.5) << std::endl;
    
    // round will round halfway point towards zero.
    
    const std::size_t test_cases(8);
    
    const double table[] = { 1.0, 1.2, 1.5, 1.8, -1.0, -1.2, -1.5, -1.8 };
    const float tablef[] = { 1.0f, 1.2f, 1.5f, 1.8f, -1.0f, -1.2f, -1.5f, -1.8f };
    
    assert(sizeof(table)/sizeof(double) == test_cases);
    assert(sizeof(tablef)/sizeof(float) == test_cases);
    
    double result_table[test_cases];
    float result_tablef[test_cases];
    long result_tablel[test_cases];
    
    {
    const double round_result[] = { 1.0, 1.0, 2.0, 2.0, -1.0, -1.0, -2.0, -2.0 };
    adobe::transform(table, &result_table[0], (double(*)(double))(&adobe::round));
    result &= adobe::equal(result_table, round_result);
    }
    
    {
    const long round_result[] = { 1L, 1L, 2L, 2L, -1L, -1L, -2L, -2L };
    adobe::transform(table, &result_tablel[0], (long(*)(double))(&adobe::lround));
    result &= adobe::equal(result_tablel, round_result);
    }
    
    {
    const double round_result[] = { 1.0, 1.0, 1.0, 1.0, -1.0, -1.0, -1.0, -1.0 };
    adobe::transform(table, &result_table[0], (double(*)(double))(&adobe::trunc));
    result &= adobe::equal(result_table, round_result);
    }
    
    {
    const double round_result[] = { 1.0, 1.0, 2.0, 2.0, -1.0, -1.0, -1.0, -2.0 };
    adobe::transform(table, &result_table[0], (double(*)(double))(&adobe::round_half_up));
    result &= adobe::equal(result_table, round_result);
    }
    
    {
    const long round_result[] = { 1L, 1L, 2L, 2L, -1L, -1L, -1L, -2L };
    adobe::transform(table, &result_tablel[0], (long(*)(double))(&adobe::lround_half_up));
    result &= adobe::equal(result_tablel, round_result);
    }
    
    {
    const float round_result[] = { 1.0f, 1.0f, 2.0f, 2.0f, -1.0f, -1.0f, -2.0f, -2.0f };
    adobe::transform(tablef, &result_tablef[0], (float(*)(float))(&adobe::round));
    result &= adobe::equal(result_tablef, round_result);
    }
    
    {
    const long round_result[] = { 1L, 1L, 2L, 2L, -1L, -1L, -2L, -2L };
    adobe::transform(tablef, &result_tablel[0], (long(*)(float))(&adobe::lround));
    result &= adobe::equal(result_tablel, round_result);
    }
    
    {
    const float round_result[] = { 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f };
    adobe::transform(tablef, &result_tablef[0], (float(*)(float))(&adobe::trunc));
    result &= adobe::equal(result_tablef, round_result);
    }
    
    {
    const float round_result[] = { 1.0f, 1.0f, 2.0f, 2.0f, -1.0f, -1.0f, -1.0f, -2.0f };
    adobe::transform(tablef, &result_tablef[0], (float(*)(float))(&adobe::round_half_up));
    result &= adobe::equal(result_tablef, round_result);
    }
    
    {
    const long round_result[] = { 1L, 1L, 2L, 2L, -1L, -1L, -1L, -2L };
    adobe::transform(tablef, &result_tablel[0], (long(*)(float))(&adobe::lround_half_up));
    result &= adobe::equal(result_tablel, round_result);
    }

    return result ? 0 : 1;
}
