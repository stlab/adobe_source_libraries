/*
    Copyright 2008 Adobe Systems Incorporated
    Distributed under the Boost Software License - Version 1.0 (see the accompanying file LICENSE
    or a copy at https://stlab.github.io/adobe_source_libraries/licenses.html)
*/

/**************************************************************************************************/

#include <adobe/algorithm.hpp>
#include <adobe/forest.hpp>

int main() {

    // qualified_usage
    {
        // Fully qualified names

        adobe::forest<int> my_forest;

        adobe::find(my_forest, 5);
    }

    // alias_usage
    {
        // Using aliases to shorten qualifiers

        namespace asl = adobe;

        asl::forest<int> my_forest;

        asl::find(my_forest, 5);
    }

    // using_usage
    {
        // Qualifying names to avoid ambiguity

        using namespace std;
        using namespace adobe;

        bool my_predicate(int);
        forest<int> my_forest;

        // qualifier required on find_if to avoid ambiguity
        adobe::find_if(my_forest.begin(), my_forest.end(), &my_predicate);
    }
}
