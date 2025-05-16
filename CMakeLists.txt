cmake_minimum_required(VERSION 3.29)
include(FindGit)
include(CMakeParseArguments)
include(./cmake/CPM.cmake)

if(WIN32)
    set(CMAKE_OBJECT_PATH_MAX 500)
endif()

project(adobe_source_libraries CXX)
include(CTest)

set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif(NOT DEFINED CMAKE_CXX_STANDARD)

set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

project(adobe-source-libraries CXX)

CPMAddPackage(
    NAME Boost
    VERSION 1.87.0
    URL https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.7z
    URL_HASH SHA256=61d61a3f33283bab8f3736745a4e47c37141f07e413986cd05d1f942c69d529a
    OPTIONS
    "BOOST_ENABLE_CMAKE ON"
    "CMAKE_CXX_CLANG_TIDY ;"
)


function(target_link_boost target)
    target_link_libraries(${target} PUBLIC Boost::system)
    target_link_libraries(${target} PUBLIC Boost::signals2)
    target_link_libraries(${target} PUBLIC Boost::range)
    target_link_libraries(${target} PUBLIC Boost::multiprecision)
    target_link_libraries(${target} PUBLIC Boost::date_time)
endfunction(target_link_boost)

function(target_link_boost_test target)
    target_link_libraries(${target} PRIVATE Boost::unit_test_framework)
    target_link_libraries(${target} PRIVATE Boost::program_options)
endfunction(target_link_boost_test)

add_subdirectory(source)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
