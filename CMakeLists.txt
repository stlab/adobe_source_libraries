cmake_minimum_required(VERSION 3.17)
include(FetchContent)
enable_testing()

project(adobe_source_libraries CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

# There are two big choices this file makes - how to include Boost, and how to
# include double-conversion, respectively. Build environments vary and we're
# trying to support all of them.

# by default we will fetch boost from github (see fetchcontent_declare call for boost below)
# If you want to use a custom boost (e.g. ../boost_libraries), define FETCHCONTENT_SOURCE_DIR_BOOST
# you can also "fetchcontent_declare" your own boost before including adobe_source_libraries in your super CMakeList.txt
option(USE_SYSTEM_BOOST OFF)

# cf boost comment above on how to use a custom double-conversion
fetchcontent_declare(double-conversion GIT_REPOSITORY https://github.com/google/double-conversion.git GIT_TAG master GIT_SHALLOW)
fetchcontent_makeavailable(double-conversion)

function(target_link_boost target)
    if (USE_SYSTEM_BOOST)
        target_link_libraries(${target} PUBLIC Boost::system Boost::filesystem)
    else()
        target_link_libraries(${target} PUBLIC boost_glob)
    endif()
endfunction(target_link_boost)

function(target_link_boost_test target)
    if (USE_SYSTEM_BOOST)
        target_compile_definitions(${target} PRIVATE BOOST_TEST_DYN_LINK)
        target_link_libraries(${target} PRIVATE Boost::unit_test_framework)
    else()
        target_link_libraries(${target} PUBLIC boost_unit_test)
    endif()
endfunction(target_link_boost_test)

if (USE_SYSTEM_BOOST)
    message("ASL_INFO: Using system boost.")
    set(ASL_BOOST_COMPONENTS system filesystem unit_test_framework program_options)
    find_package(Boost COMPONENTS ${ASL_BOOST_COMPONENTS} REQUIRED)
else()
    message("ASL_INFO: Building boost ourselves.")

    set(BOOST_SUBMODULES
        # required by asl
        libs/system
        libs/filesystem
        libs/program_options
        libs/signals2
        libs/variant
        libs/gil
        # required by libs/filesystem
        libs/container_hash
        # required by adobe::fnv unless ADOBE_FNV_NO_BIGINTS is defined
        libs/multiprecision
        # required by boost test or by the one above
        libs/test
        libs/detail
        libs/config
        libs/type_traits
        libs/preprocessor
        libs/utility
        libs/core
        libs/smart_ptr
        libs/assert
        libs/predef
        libs/throw_exception
        libs/function
        libs/integer
        libs/static_assert
        libs/mpl
        libs/bind
        libs/move
        libs/exception
        libs/timer
        libs/io
        libs/iterator
        libs/numeric
        libs/numeric/conversion
        libs/functional
        libs/algorithm
        libs/range
        # required by program_options
        libs/optional
        libs/any
        libs/type_index
        libs/lexical_cast
        libs/tokenizer
        # required by range
        libs/concept_check
        # required by lexical_cast
        libs/array
        libs/container
        libs/math
        # required by multiprecision
        libs/format
        libs/rational
        # required by signals2
        libs/multi_index
        )

    fetchcontent_declare(boost
            GIT_REPOSITORY https://github.com/boostorg/boost.git
            GIT_TAG boost-1.76.0
            GIT_SUBMODULES ${BOOST_SUBMODULES}
            GIT_SHALLOW
            SOURCE_SUBDIR "some value that will prevent cmake from finding root boost CMakeLists.txt (and I don't know why CONFIGURE_COMMAND \"\" does not work)")
    fetchcontent_makeavailable(boost)
    fetchcontent_getproperties(boost SOURCE_DIR boost_root)

    # There really isn't a need to separate out these sources, so we build them
    # in one large static library.

    file(GLOB ASL_BOOST_FILESYSTEM_SRC ${boost_root}/libs/filesystem/src/*.cpp)
    file(GLOB ASL_BOOST_PROGRAM_OPTIONS_SRC ${boost_root}/libs/program_options/src/*.cpp)
    file(GLOB ASL_BOOST_SYSTEM_SRC ${boost_root}/libs/system/src/*.cpp)
    
    set(BOOST_GLOB_SOURCES 
          ${ASL_BOOST_FILESYSTEM_SRC}
          ${ASL_BOOST_PROGRAM_OPTIONS_SRC}
          ${ASL_BOOST_SYSTEM_SRC})

    add_library(boost_glob STATIC ${BOOST_GLOB_SOURCES})

    foreach (submodule ${BOOST_SUBMODULES})
        target_include_directories(boost_glob PUBLIC ${boost_root}/${submodule}/include)
    endforeach(submodule)

    # We separate out the unit test framework from the rest of the boost
    # add_library support because it has its own main routine which we only need
    # when building unit tests.

    file(GLOB ASL_BOOST_TEST_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${boost_root}/libs/test/src/*.cpp)
    set(asl_boost_unit_test_cpp_main_cpp_path ${boost_root}/libs/test/src/cpp_main.cpp)
    cmake_path(RELATIVE_PATH asl_boost_unit_test_cpp_main_cpp_path)
    list(REMOVE_ITEM ASL_BOOST_TEST_SRC ${asl_boost_unit_test_cpp_main_cpp_path})

    add_library(boost_unit_test STATIC ${ASL_BOOST_TEST_SRC})
    target_include_directories(boost_unit_test PUBLIC ${boost_root}/libs/test/include)
    foreach (submodule ${BOOST_SUBMODULES})
        target_include_directories(boost_unit_test PUBLIC ${boost_root}/${submodule}/include)
    endforeach(submodule)
endif()

add_subdirectory(source)
add_subdirectory(test)
