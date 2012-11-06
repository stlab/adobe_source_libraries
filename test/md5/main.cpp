/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#include <adobe/config.hpp>
#include <adobe/md5.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/version.hpp>

#include <iostream>
#include <fstream>

/****************************************************************************************************/

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        std::cout << "usage: " << argv[0] << " [ file ]" << std::endl;
        std::cout << "       generate the MD5 hash of a file" << std::endl;
        std::cout << "       Adobe Source Libraries v" << ADOBE_VERSION_MAJOR << "." << ADOBE_VERSION_MINOR << "." << ADOBE_VERSION_SUBMINOR << std::endl;
        std::cout << "       Boost v" << BOOST_VERSION / 100000 << "." << BOOST_VERSION / 100 % 1000 << "." << BOOST_VERSION % 100 << std::endl;

        return 1;
    }

    bool success(false);

    try
    {
        adobe::md5_t            m;
        boost::filesystem::path file_path(argv[1], boost::filesystem::native);
        std::ifstream           stream(file_path.native_file_string().c_str(), std::ios::binary | std::ios::in);

        while (stream.good())
        {
            boost::array<std::ifstream::char_type, 256*1024> buffer;

            stream.read(&buffer[0], static_cast<std::streamsize>(buffer.size()));

            std::streamsize gcount(stream.gcount());

            if (gcount > 0) m.update(&buffer[0], static_cast<std::size_t>(gcount));
        }

        adobe::md5_t::digest_t              hash(m.final());
        adobe::md5_t::digest_t::iterator    first(hash.begin());
        adobe::md5_t::digest_t::iterator    last(hash.end());

        std::cout << "MD5(" << argv[1] << ")= ";

        for(; first != last; ++first)
        {
            std::cout.width(2);
            std::cout.fill('0');
            std::cout << std::hex << static_cast<int>(*first);
        }

        std::cout << std::endl;

        success = true;
    }
    catch( const std::exception& error )
    { std::cerr << "Exception: " << error.what() << std::endl; }
    catch( ... )
    { std::cerr << "Unknown exception" << std::endl; }

    return success ? 0 : 1;
}
