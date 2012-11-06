/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/**************************************************************************************************/

#include <iostream>
#include <fstream>
#include <map>
#include <ext/hash_map>

#include <adobe/dictionary.hpp>
#include <adobe/zuid.hpp>
#include <adobe/future/timer.hpp>

/**************************************************************************************************/

namespace {

/**************************************************************************************************/

template <std::size_t N = 4>
struct large    // : boost::totally_ordered< large<N> >
{
    large()
    { }

    double a[N];
};

template <std::size_t N>
bool operator==(const large<N>&, const large<N>&)
{ return true; }

// GCC isn't happy without this operator
template <std::size_t N>
bool operator!=(const large<N>&, const large<N>&)
{ return false; }

template <std::size_t N>
bool operator<(const large<N>&, const large<N>&)
{ return false; }

template <std::size_t N>
std::ostream& operator<<(std::ostream& s, const large<N>&)
{ return s; }

typedef large<> large_t;

/**************************************************************************************************/

adobe::name_t random_key()
{
#if 0

    // random low ASCII string -- faster than a zuid, more randomness to the characters (less aliasing by the hash function)
    // isalnum(c) | ispunct(c) | isspace(c)
    // unfortunately, I can't guarantee that this will not cycle and give 2 identical keys (which it may have done)
    
    const int keyLength = 36;
    char keyBuffer[ keyLength+1 ];
    const char allowedChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890`~!@#$%^&*()-_=+[{]}\\|;:'\",<.>/? ";
    const int maxIndex = sizeof(allowedChars) / sizeof(char);
    
    // last few chars are an incremented number to guarantee uniqueness
    static unsigned long counter = 1;
    const int counterLength = 6;
    
    for (std::size_t i(0); i <= (keyLength - counterLength); ++i)
    {
        int c = ((unsigned long)(rand() >> 2) & 0x7FFF) % maxIndex;
        keyBuffer[i] = allowedChars[c];
    }
    
    // convert counter to a string 6 bits at a time
    keyBuffer[keyLength - 6] = allowedChars[ (counter>>26) & 63 ];
    keyBuffer[keyLength - 5] = allowedChars[ (counter>>24) & 63 ];
    keyBuffer[keyLength - 4] = allowedChars[ (counter>>18) & 63 ];
    keyBuffer[keyLength - 3] = allowedChars[ (counter>>12) & 63 ];
    keyBuffer[keyLength - 2] = allowedChars[ (counter>>6) & 63 ];
    keyBuffer[keyLength - 1] = allowedChars[ (counter>>0) & 63 ];
    
    ++counter;
    
    keyBuffer[keyLength] = 0;   // make sure the key is NULL terminated
    
    
    return adobe::name_t( keyBuffer );
    
#else

    // random zuid string - slightly slower, because it uses only a subset of ASCII (more aliasing by the hash function)
    static adobe::zuid_t base_zuid("3AD0DB88-4B21-4E42-A899-A8338C19BF16");
    adobe::zuid_t        tmp_zuid;
    adobe::zuid_t        final_zuid(base_zuid, tmp_zuid.str());

    return adobe::name_t(final_zuid.c_str());
    
#endif
}

/**************************************************************************************************/

inline const adobe::any_regular_t& get_regular(int type)
{
    static const large_t              large;
    static const adobe::any_regular_t small_s(42);
    static const adobe::any_regular_t large_s(large);

    if (type == 0)
        return small_s;
    else if (type == 1)
        return large_s;
    else
        throw std::runtime_error("!");
}

/**************************************************************************************************/

struct less_str {
    bool operator() ( const char *x, const char *y) const
    { return (strcmp(x, y) < 0); }
};

typedef  std::map< const char *, adobe::any_regular_t, less_str> std_map_dictionary_t;

std::pair<double, double> test_std_map(std::size_t n, int type, const std::string label, std::size_t Repeat)
{
   std_map_dictionary_t d;

    // initialize the dictionary with semi-random keys
    for (std::size_t i(0); i < n; ++i)
        d[ random_key().c_str() ] = get_regular(type);

    // sanity check the dictionary size
    assert(d.size() == n);


    // time the dictionary constructor overhead
    adobe::timer_t timer;

    timer.reset();
    for (std::size_t i(0); i < Repeat; ++i)
    {
        std_map_dictionary_t cctor_test(d);
        
        // sanity check!
        assert(cctor_test.size() == n);
    }

    double cctor_avg(timer.split()/Repeat);


    
    // create a random lookup order table to prevent order bias in the searches
    // that could be a huge problem with a binary search or tree structure search
    std_map_dictionary_t::iterator iter(d.begin());
    std::vector< const char * > randomKeyList;
    
    while (iter != d.end())
    {
        randomKeyList.push_back( iter->first );
        ++iter;
    }
    
    // randomize the key list
    std::random_shuffle( randomKeyList.begin(), randomKeyList.end() );


    // time the dictionary lookup (vector iterator should have VERY little overhead)
    timer.reset();
    for (std::size_t i(0); i < Repeat; ++i)
    {
        for ( std::vector< const char * >::iterator j(randomKeyList.begin()); j != randomKeyList.end(); ++j )
        {
            std_map_dictionary_t::iterator found( d.find( *j ) );
            
            // sanity check!  (if this is optimized out, then the search may be optimized out)
            if ( found->first != *j )
                throw -1;
        }
    }

    double find_avg( timer.split()/(n*Repeat) );
    
    // report quick results to stdout
    std::cout << n << "/"
              << label << ":"
              << " cctor: " << cctor_avg << "ms"
              << " lookup: " << find_avg << "ms"
              << std::endl;

    // and return a pair of results
    return std::make_pair(cctor_avg, find_avg);
}


/**************************************************************************************************/

struct equal_str {
    bool operator()(const char* x, const char* y) const
    { return std::strcmp(x, y) == 0; }
};

typedef  __gnu_cxx::hash_map< const char *, adobe::any_regular_t, __gnu_cxx::hash<const char*>,
    equal_str> hash_map_dictionary_t;

std::pair<double, double> test_hash_map(std::size_t n, int type, const std::string label, std::size_t Repeat)
{
   hash_map_dictionary_t d;

    // initialize the dictionary with semi-random keys
    for (std::size_t i(0); i < n; ++i)
        d[ random_key().c_str() ] = get_regular(type);

    // sanity check the dictionary size
    assert(d.size() == n);


    // time the dictionary constructor overhead
    adobe::timer_t timer;

    timer.reset();
    for (std::size_t i(0); i < Repeat; ++i)
    {
        hash_map_dictionary_t cctor_test(d);
        
        // sanity check!
        assert(cctor_test.size() == n);
    }

    double cctor_avg(timer.split()/Repeat);


    
    // create a random lookup order table to prevent order bias in the searches
    // that could be a huge problem with a binary search or tree structure search
    hash_map_dictionary_t::iterator iter(d.begin());
    std::vector< const char * > randomKeyList;
    
    while (iter != d.end())
    {
        randomKeyList.push_back( iter->first );
        ++iter;
    }
    
    // randomize the key list
    std::random_shuffle( randomKeyList.begin(), randomKeyList.end() );


    // time the dictionary lookup (vector iterator should have VERY little overhead)
    timer.reset();
    for ( std::size_t i(0); i < Repeat; ++i)
    {
        for ( std::vector< const char * >::iterator j(randomKeyList.begin()); j != randomKeyList.end(); ++j )
        {
            hash_map_dictionary_t::iterator found( d.find( *j ) );
            
            // sanity check!  (if this is optimized out, then the search may be optimized out)
            if ( found->first != *j )
                throw -1;
        }
    }

    double find_avg( timer.split()/(n*Repeat) );
    
    // report quick results to stdout
    std::cout << n << "/"
              << label << ":"
              << " cctor: " << cctor_avg << "ms"
              << " lookup: " << find_avg << "ms"
              << std::endl;

    // and return a pair of results
    return std::make_pair(cctor_avg, find_avg);
}

/**************************************************************************************************/

std::pair<double, double> test_adobe_dictionary(std::size_t n, int type, const std::string label, std::size_t Repeat)
{
    adobe::dictionary_t d;

    // initialize the dictionary with semi-random keys
    for (std::size_t i(0); i < n; ++i)
        d.insert(adobe::dictionary_t::value_type(random_key(), get_regular(type)));

    // sanity check the dictionary size
    assert(d.size() == n);


    // time the dictionary constructor overhead
    adobe::timer_t timer;

    timer.reset();
    for (std::size_t i(0); i < Repeat; ++i)
    {
        adobe::dictionary_t cctor_test(d);
        
        // sanity check!
        assert(cctor_test.size() == n);
    }

    double cctor_avg(timer.split()/Repeat);


    
    // create a random lookup order table to prevent order bias in the searches
    // that could be a huge problem with a binary search or tree structure search
    adobe::dictionary_t::iterator iter(d.begin());
    std::vector< adobe::name_t > randomKeyList;
    
    while (iter != d.end())
    {
        randomKeyList.push_back( iter->first );
        ++iter;
    }
    
    // randomize the key list
    std::random_shuffle( randomKeyList.begin(), randomKeyList.end() );


    // time the dictionary lookup (vector iterator should have VERY little overhead)
    timer.reset();
    for ( std::size_t i(0); i < Repeat; ++i)
    {
        for ( std::vector< adobe::name_t >::iterator j(randomKeyList.begin()); j != randomKeyList.end(); ++j )
        {
            adobe::dictionary_t::iterator found( d.find( *j ) );
            
            // sanity check!  (if this is optimized out, then the search may be optimized out)
            if ( found->first != *j )
                throw -1;
        }
    }

    double find_avg( timer.split()/(n*Repeat) );

    // report quick results to stdout
    std::cout << n << "/"
              << label << ":"
              << " cctor: " << cctor_avg << "ms"
              << " lookup: " << find_avg << "ms"
              << std::endl;

    // and return a pair of results
    return std::make_pair(cctor_avg, find_avg);
}

/**************************************************************************************************/

void do_test(std::size_t n, std::ofstream& results, int Repeat)
{
    std::pair<double, double> small(test_adobe_dictionary(n, 0, "small", Repeat));
    std::pair<double, double> large(test_adobe_dictionary(n, 1, "large", Repeat));
    std::pair<double, double> hashed_small(test_hash_map(n, 0, "hash_small", Repeat));
    std::pair<double, double> hashed_large(test_hash_map(n, 0, "hash_large", Repeat));
    std::pair<double, double> std_small(test_std_map(n, 0, "std_small", Repeat));
    std::pair<double, double> std_large(test_std_map(n, 0, "std_large", Repeat));

    // report the results in a comma separated file
    results << n << ","
            << small.first << ","
            << large.first << ","
            << small.second << ","
            << large.second << ","
            << hashed_small.first << ","
            << hashed_large.first << ","
            << hashed_small.second << ","
            << hashed_large.second << ","
            << std_small.first << ","
            << std_large.first << ","
            << std_small.second << ","
            << std_large.second << ","
            << std::endl;

}

/**************************************************************************************************/

} // namespace

/**************************************************************************************************/

int main()
{
    std::cerr << "dictionary_benchmark compiled " << __DATE__ << " " << __TIME__ << std::endl;

    //std::srand(std::time(0));
    std::srand(4242);       // we need repeatable results

#ifndef NDEBUG
    std::ofstream output("results_debug.csv");
#else
    std::ofstream output("results_release.csv");
#endif

    output << "elements,small cctor,large cctor,small find,large find,"
        << "hash_map small cctor,hash_map large cctor,hash_map small find,hash_map large find,"
        << "std::map small cctor,std::map large cctor,std::map small find,std::map large find"
        << std::endl;

#if 0
    for (int i(0); i < 20; ++i)
        std::cout << random_key().c_str() << std::endl;
#else
    const std::size_t maximumSize = 2500;
    const std::size_t sizeIncrement = 25;
    const std::size_t repeatMaximum = 500000000L / maximumSize;

    for (std::size_t i(0); i <= maximumSize; i += sizeIncrement)
    {
        // try to keep the execution time close to constant per size
        // this is entirely for the sanity and patience of the
        // person running and debugging this code :-)
        std::size_t iterations = repeatMaximum / (i+1);

        if (iterations < 2)
            iterations = 2;

        do_test(i, output, iterations);
    }
#endif
}

/**************************************************************************************************/
