cmake_minimum_required(VERSION 3.0)
include(ExternalProject)
include(CMakeParseArguments)
enable_testing()

project(adobe_source_libraries CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

# There are two big choices this file makes - how to include Boost, and how to
# include double-conversion, respectively. Build environments vary and we're
# trying to support all of them.
#
# If BOOST_ROOT is defined, that means the headers and libraries have been built
# and are ready for use by CMake. Otherwise, we'll have to roll them ourselves.
#
# For double-conversion, the unix makefile setup is able to download the git
# repo and build it. For Xcode, this isn't the case, and we'll have to roll
# our own there, too.

if (${CMAKE_GENERATOR} STREQUAL "Xcode")
    set(ASL_DOUBLECONV_SUPPORT ON)

    message("ASL_INFO: Building double-conversion ourselves.")
else()
    set(ASL_DOUBLECONV_SUPPORT OFF)
endif()

if (${ASL_DOUBLECONV_SUPPORT})
    file(GLOB ASL_DOUBELCONV_SRC ../double-conversion/src/*.cc)

    add_library(doubleconv_support STATIC ${ASL_DOUBELCONV_SRC})

    get_filename_component(ASL_DOUBLECONV_PATH ../ ABSOLUTE)

    target_include_directories(doubleconv_support PUBLIC ${ASL_DOUBLECONV_PATH})
else()
    ExternalProject_Add(double-conversion_prj
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/double-conversion
        GIT_REPOSITORY https://github.com/google/double-conversion.git
        # GIT_REPOSITORY https://github.com/stlab/double-conversion.git
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})

    link_directories(${CMAKE_INSTALL_PREFIX}/lib)
endif()

function(target_link_doubleconversion target)
    if (${ASL_DOUBLECONV_SUPPORT})
        add_dependencies(${target} doubleconv_support)

        target_link_libraries(${target} PUBLIC doubleconv_support)
    else()
        #tweak on include double-conversion path
        target_compile_definitions(${target} PUBLIC -DADOBE_BUILT_WITH_CMAKE)
        add_dependencies(${target} double-conversion_prj)
        target_include_directories(${target} PUBLIC ${CMAKE_INSTALL_PREFIX}/include)
        target_link_libraries(${target} PUBLIC double-conversion)
    endif()
endfunction(target_link_doubleconversion)

function(target_link_boost target)
    if (${BOOST_ROOT})
        target_link_libraries(${target} PUBLIC ${Boost_SYSTEM_LIBRARY})
    else()
        add_dependencies(${target} boost_glob)

        target_link_libraries(${target} PUBLIC boost_glob)
    endif()
endfunction(target_link_boost)

function(target_link_boost_test target)
    if (${BOOST_ROOT})
        target_compile_definitions(${asl_test_NAME} PRIVATE BOOST_TEST_DYN_LINK)
        target_link_libraries(${asl_test_NAME} PRIVATE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
    else()
        add_dependencies(${target} boost_unit_test)

        target_link_libraries(${target} PUBLIC boost_unit_test)
    endif()
endfunction(target_link_boost_test)

if (${BOOST_ROOT})
    find_package(Boost COMPONENTS system thread unit_test_framework program_options REQUIRED)
else()
    message("ASL_INFO: Building boost ourselves.")

    # There really isn't a need to separate out these sources, so we build them
    # in one large static library.

    file(GLOB ASL_BOOST_FILESYSTEM_SRC ../boost_libraries/libs/filesystem/src/*.cpp)
    file(GLOB ASL_BOOST_PROGRAM_OPTIONS_SRC ../boost_libraries/libs/program_options/src/*.cpp)
    file(GLOB ASL_BOOST_SYSTEM_SRC ../boost_libraries/libs/system/src/*.cpp)
    file(GLOB ASL_BOOST_THREAD_SRC ../boost_libraries/libs/thread/src/*.cpp)

    if (${UNIX})
        file(GLOB ASL_BOOST_THREAD_TSS ../boost_libraries/libs/thread/src/pthread/*.cpp)
    elseif (${WIN32})
        set(ASL_BOOST_THREAD_TSS
            ../boost_libraries/libs/thread/src/win32/thread.cpp
            ../boost_libraries/libs/thread/src/win32/tss_pe.cpp # no idea if this is right...
        )
    endif()

    add_library(
        boost_glob STATIC
        ${ASL_BOOST_FILESYSTEM_SRC}
        ${ASL_BOOST_PROGRAM_OPTIONS_SRC}
        ${ASL_BOOST_SYSTEM_SRC}
        ${ASL_BOOST_THREAD_SRC}
        ${ASL_BOOST_THREAD_TSS}
    )

    target_include_directories(boost_glob PUBLIC ../boost_libraries)

    # We separate out the unit test framework from the rest of the boost
    # add_library support because it has its own main routine which we only need
    # when building unit tests.

    # I can't find a way to glob these because one infernal file, cpp_main.cpp,
    # mucks with my ability to use it.

    add_library(
        boost_unit_test STATIC
        ../boost_libraries/libs/test/src/compiler_log_formatter.cpp
        ../boost_libraries/libs/test/src/debug.cpp
        ../boost_libraries/libs/test/src/exception_safety.cpp
        ../boost_libraries/libs/test/src/execution_monitor.cpp
        ../boost_libraries/libs/test/src/framework.cpp
        ../boost_libraries/libs/test/src/interaction_based.cpp
        ../boost_libraries/libs/test/src/logged_expectations.cpp
        ../boost_libraries/libs/test/src/plain_report_formatter.cpp
        ../boost_libraries/libs/test/src/progress_monitor.cpp
        ../boost_libraries/libs/test/src/results_collector.cpp
        ../boost_libraries/libs/test/src/results_reporter.cpp
        ../boost_libraries/libs/test/src/test_main.cpp
        ../boost_libraries/libs/test/src/test_tools.cpp
        ../boost_libraries/libs/test/src/unit_test_log.cpp
        ../boost_libraries/libs/test/src/unit_test_main.cpp
        ../boost_libraries/libs/test/src/unit_test_monitor.cpp
        ../boost_libraries/libs/test/src/unit_test_parameters.cpp
        ../boost_libraries/libs/test/src/unit_test_suite.cpp
        ../boost_libraries/libs/test/src/xml_log_formatter.cpp
        ../boost_libraries/libs/test/src/xml_report_formatter.cpp
    )

    target_include_directories(boost_unit_test PRIVATE -DBOOST_TEST_MAIN)
    target_include_directories(boost_unit_test PUBLIC ../boost_libraries)
endif()

add_subdirectory(source)
add_subdirectory(test)
