/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/zuid.hpp>
#include <adobe/algorithm.hpp>
#include <adobe/config.hpp>

#include <vector>
#include <iostream>

/*************************************************************************************************/

#if defined(BOOST_MSVC) && defined(BOOST_THREAD_USE_LIB)
extern "C" void tss_cleanup_implemented()
{ }
#endif

/****************************************************************************************************/

int main(int argc, char** argv)
{
    if (argc > 1)
    {
        if (std::string(argv[1]) == "--test")
        {
            std::string test_uuid("baadf00d-cafe-1337-d00d-feeddeadbeef");
            std::string test_uuid2("baadf00d-feed-dead-beef-1337cafed00d");

            adobe::zuid_t zuid1(test_uuid);
            adobe::zuid_t zuid2(test_uuid2.c_str());

            std::cout << "Extended test:" << std::endl;

            assert(zuid1.str() == test_uuid);
            assert(zuid2.c_str() == test_uuid2);
            assert(zuid1 == adobe::zuid_t(test_uuid.c_str()));
            assert(zuid1 != zuid2);

            std::string test_uuid3("feeddead-beef-a7da-1337-cafed00d0000");

            adobe::zuid_t zuid3(test_uuid3);

            assert(zuid3 != zuid1 && zuid3 != zuid2);

            zuid3 = zuid1;

            assert(zuid3 == zuid1 && zuid3 != adobe::zuid_t(test_uuid3));

            assert(adobe::zuid_t::null.str() == "00000000-0000-0000-0000-000000000000");

            std::cout << "Default ctor'd zuid: " << adobe::zuid_t().c_str() << std::endl;

            std::cout << "Known bad uuid1: " << adobe::zuid_t("").c_str() << std::endl
                      << "          uuid2: " << adobe::zuid_t("bad").c_str() << std::endl
                      << "          uuid3: " << adobe::zuid_t("bad-f00d").c_str() << std::endl
                      << "          uuid4: " << adobe::zuid_t("bad-f00d-badd").c_str() << std::endl
                      << "          uuid5: " << adobe::zuid_t("bad-f00d-badd-d00d").c_str() << std::endl
                      << "          uuid6: " << adobe::zuid_t("bad-f00d-badd-d00d-deadbeef").c_str() << std::endl;

            std::cout << "Sort Test:" << std::endl;

            typedef std::vector<adobe::zuid_t> zuid_set_t;

            adobe::zuid_t domain(test_uuid);

            zuid_set_t zuid_set;

            zuid_set.push_back(adobe::zuid_t(domain, "Ted,"));
            zuid_set.push_back(adobe::zuid_t(domain, "Something"));
            zuid_set.push_back(adobe::zuid_t(domain, "is"));
            zuid_set.push_back(adobe::zuid_t(domain, "definitely"));
            zuid_set.push_back(adobe::zuid_t(domain, "afoot"));
            zuid_set.push_back(adobe::zuid_t(domain, "at"));
            zuid_set.push_back(adobe::zuid_t(domain, "the"));
            zuid_set.push_back(adobe::zuid_t(domain, "Circle"));
            zuid_set.push_back(adobe::zuid_t(domain, "K"));

            for (zuid_set_t::iterator first(zuid_set.begin()), last(zuid_set.end()); first != last; ++first)
                std::cout << ' ' << first->c_str() << std::endl;

            adobe::sort(zuid_set);

            std::cout << "Sorted:" << std::endl;

            for (zuid_set_t::iterator first(zuid_set.begin()), last(zuid_set.end()); first != last; ++first)
                std::cout << ' ' << first->c_str() << std::endl;
        }
        else
        {
            std::cout << "Adobe Source Libraries ZUID generator; version "
                      << ADOBE_VERSION_MAJOR << '.'
                      << ADOBE_VERSION_MINOR << '.'
                      << ADOBE_VERSION_SUBMINOR << std::endl
                      << "http://stlab.adobe.com"
                      << std::endl << std::endl;
            std::cout << "Usage:" << std::endl;
            std::cout << "  " << argv[0] << std::endl;
            std::cout << "    > generate a ZUID" << std::endl;
            std::cout << "  " << argv[0] << " --test" << std::endl;
            std::cout << "    > Run extended test over adobe::zuid_t API" << std::endl;
        }
    }
    else
    {
        std::cout << adobe::zuid_t().c_str() << std::endl;
    }

    return 0;
}
