/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <vector>

#include "name.hpp"

/****************************************************************************************************/

template <typename T>
void type_and_value(const char* name, const T& value)
{
    std::cout << name << " (" << typeid(value).name() << "): " << value << '\n';
}

/****************************************************************************************************/

int main()
try
{
    using namespace adobe::literals;

#if ADOBE_NAME_DEBUG
    std::cout << "Built with ADOBE_NAME_DEBUG=1\n";
    std::cout << '\n';
#endif

    constexpr std::size_t myhash = adobe::detail::name_hash("Hello, world!");

    std::cout << "hash: 0x" << std::hex << myhash << std::dec << "\n";

    std::cout << "\"Hello, world!\"_name should hash to 0x38d1334144987bf4:\n";
    std::cout << "Hello, world!"_name << "\n";

    adobe::static_name_t hello_world("Hello, world!"_name);
    adobe::static_name_t foo("Red Sox"_name);
    adobe::name_t        bar("Red Sox");
    adobe::name_t        baz("Cardinals");
    adobe::name_t        nullname(""_name);

    type_and_value("hello_world", hello_world);
    type_and_value("foo", foo);
    type_and_value("bar", bar);
    type_and_value("baz", baz);
    type_and_value("nullname", nullname);
    std::cout << '\n';
    type_and_value("static_cast<adobe::name_t>(foo)", static_cast<adobe::name_t>(foo));
    std::cout << '\n';
    std::cout << "foo == bar: " << std::boolalpha << (foo == bar) << '\n';
    std::cout << "foo != bar: " << std::boolalpha << (foo != bar) << '\n';
    std::cout << '\n';
    std::cout << "foo == baz: " << std::boolalpha << (foo == baz) << '\n';
    std::cout << "foo != baz: " << std::boolalpha << (foo != baz) << '\n';
    std::cout << '\n';
    std::cout << "bar == baz: " << std::boolalpha << (bar == baz) << '\n';
    std::cout << "bar != baz: " << std::boolalpha << (bar != baz) << '\n';
    std::cout << '\n';
    std::cout << "foo < bar: " << std::boolalpha << (foo < bar) << '\n';
    std::cout << "bar < foo: " << std::boolalpha << (bar < foo) << '\n';
    std::cout << '\n';
    std::cout << "foo < baz: " << std::boolalpha << (foo < baz) << '\n';
    std::cout << "baz < foo: " << std::boolalpha << (baz < foo) << '\n';
    std::cout << '\n';
    std::cout << "bar < baz: " << std::boolalpha << (bar < baz) << '\n';
    std::cout << "baz < bar: " << std::boolalpha << (baz < bar) << '\n';
    std::cout << '\n';
    std::cout << "     foo is null: " << !static_cast<bool>(foo) << '\n';
    std::cout << "nullname is null: " << !static_cast<bool>(nullname) << '\n';
    std::cout << '\n';

    std::vector<adobe::name_t> name_set =
    {
        "fig"_name,
        "apple"_name,
        "banana"_name,
        "eggplant"_name,
        "grape"_name,
        "horseradish"_name,
        "daikon"_name,
        "carrot"_name
    };

    std::cout << "Before slow sort:\n";
    for (auto& entry : name_set)
        std::cout << "  " << entry << '\n';

    std::sort(begin(name_set), end(name_set));

    std::cout << "After slow sort:\n";
    for (auto& entry : name_set)
        std::cout << "  " << entry << '\n';

    std::random_shuffle(begin(name_set), end(name_set));

    std::cout << "Before fast sort:\n";
    for (auto& entry : name_set)
        std::cout << "  " << entry << '\n';

    std::sort(begin(name_set), end(name_set), adobe::name_t::fast_sort);

    std::cout << "After fast sort:\n";
    for (auto& entry : name_set)
        std::cout << "  " << entry << '\n';

    return 0;
}
catch (std::exception& error)
{
    std::cout << "Error: " << error.what() << '\n';

    return 1;
}
catch (...)
{
    std::cout << "Error: unknown\n";

    return 1;
}

/****************************************************************************************************/
