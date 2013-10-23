/*************************************************************************************************/

/*

    ADOBE CONFIDENTIAL
    ___________________

    Copyright 2004 - 2004 Adobe Systems Incorporated
    All Rights Reserved.

    NOTICE: All information contained herein is, and remains the property of
    Adobe Systems Incorporated and its suppliers, if any. The intellectual and 
    technical concepts contained herein are proprietary to Adobe Systems Incorporated 
    and its suppliers and may be covered by U.S. and Foreign Patents, patents 
    in process, and are protected by trade secret or copyright law. Dissemination 
    of this information or reproduction of this material is strictly forbidden 
    unless prior written permission is obtained from Adobe Systems Incorporated.
 
*/

/*************************************************************************************************/

#include <vector>

#include <adobe/algorithm.hpp>

int main()
{

// incorrect_namespace
{ 
// This results in an ambiguity and won't compile

    using namespace adobe;
    
    bool my_predicate(int);
    std::vector<int> my_vector;
    
    find_if(my_vector.begin(), my_vector.end(), &my_predicate); // adobe or std?
}

}
