
#include <iostream>
#include <iomanip>
#include <sstream>

#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

#include <adobe/config.hpp>
#include <adobe/md5.hpp>


int main(int argc, char* argv[])
{
    if (argc != 3) {
      std::cout << "usage: " << argv[0] << " hash file " << std::endl;
      std::cout << "       check the MD5 hash of a file" << std::endl;
      std::cout << "       Adobe Source Libraries v" << ADOBE_VERSION_MAJOR
                << "." << ADOBE_VERSION_MINOR << "." << ADOBE_VERSION_SUBMINOR << std::endl;
      std::cout << "       Boost v" << BOOST_VERSION / 100000 << "." << BOOST_VERSION / 100 % 1000 
                << "." << BOOST_VERSION % 100 << std::endl;
      
      return 1;
    }

    bool success = false;

    try
    {
      adobe::md5_t m;
      boost::filesystem::path file_path(argv[2], boost::filesystem::native);
      boost::filesystem::ifstream stream(file_path, std::ios::binary | std::ios::in);

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

        std::string actual;
        std::ostringstream oss(actual);
        for(; first != last; ++first){
            oss.width(2);
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(*first);
        }
        

        std::string expected(argv[1]);
        std::string::iterator e(expected.begin());
        success = oss.str() == expected;
        if(!success)
            std::cout << "Expected: " << expected
                      << " actual: " << oss.str() << std::endl;
        
    }

    catch( const std::exception& error )
        { 
            std::cerr << "Exception: " << error.what() << std::endl; 
        }
    catch( ... )
        { 
            std::cerr << "Unknown exception" << std::endl; 
        }
    return success ? 0 : 1;
}
