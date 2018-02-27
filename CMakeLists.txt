cmake_minimum_required(VERSION 3.0)
include(FindGit)
include(CMakeParseArguments)
enable_testing()

project(adobe_source_libraries CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

add_definitions ("-Wall")
add_definitions ("-Werror")

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" AND
    ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "-lc++abi")
endif()

# There are two big choices this file makes - how to include Boost, and how to
# include double-conversion, respectively. Build environments vary and we're
# trying to support all of them.
#
# If USE_SYSTEM_BOOST is defined, that means the headers and libraries have been built
# and are ready for use by CMake. Otherwise, we'll have to roll them ourselves.
#
# For double-conversion, the unix makefile setup is able to download the git
# repo and build it

option(USE_SYSTEM_BOOST "use cmake found boost instead of ../boost_libraries" OFF)
option(USE_STLAB_DOUBLECONV "use stlab mirror of double-conversion repository" OFF)

# with boost 1.60 boost.thread uses boost.move unique_ptr
# which is broken with BOOST_MOVE_USE_STANDARD_LIBRARY_MOVE flag...
# fix is to conditionally remove this last boost thread dependency
# NB: AppleClang lacks thread_local
option(USE_STD_THREAD_LOCAL "use C++11 thread_local instead of Boost" OFF)
if (USE_STD_THREAD_LOCAL AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
    message (WARNING "C++11 thread_local is not supported by AppleClang")
endif()


set(root_path ${CMAKE_SOURCE_DIR}/..)

function(setup_dep)
    set(options IS_CMAKE)
    set(oneValueArgs URL BRANCH TAG NAME)
    cmake_parse_arguments(setup_dep "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if ("${setup_dep_NAME}" STREQUAL "")
        get_filename_component(name ${setup_dep_URL} NAME_WE)
    else()
        set(name ${setup_dep_NAME})
    endif()

    set(dep_path ${root_path}/${name})

    if(NOT IS_DIRECTORY ${dep_path})
        message("ASL_INFO: Setting up dep " ${name} " into " ${dep_path})
        execute_process(COMMAND ${GIT_EXECUTABLE} clone --recursive ${setup_dep_URL} ${name} WORKING_DIRECTORY ${root_path})

        if("${setup_dep_BRANCH}" STREQUAL "")
            message(FATAL_ERROR "ASL_INFO: No branch given for dep " ${name})
        endif()

        execute_process(COMMAND ${GIT_EXECUTABLE} checkout ${setup_dep_BRANCH} WORKING_DIRECTORY ${dep_path})
    else()
        message("ASL_INFO: Found dep " ${name} " into " ${dep_path})
    endif()

    if (setup_dep_IS_CMAKE)
        add_subdirectory(${dep_path} ${CMAKE_BINARY_DIR}/imported/${name})
    endif()

endfunction()

if (USE_STLAB_DOUBLECONV)
  setup_dep(URL https://github.com/stlab/double-conversion.git BRANCH master IS_CMAKE)
  target_include_directories(double-conversion PUBLIC $<BUILD_INTERFACE:${root_path}>)
else()
  setup_dep(URL https://github.com/google/double-conversion.git BRANCH master IS_CMAKE)
  target_include_directories(double-conversion PUBLIC $<BUILD_INTERFACE:${root_path}/double-conversion>)
  # poorly named flag to fix include path
  target_compile_definitions(double-conversion PUBLIC ADOBE_BUILT_WITH_CMAKE)
endif()

function(target_link_boost target)
    if (USE_SYSTEM_BOOST)
        target_link_libraries(${target} PUBLIC ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
        if(NOT USE_STD_THREAD_LOCAL)
            target_link_libraries(${target} PUBLIC ${Boost_THREAD_LIBRARY})
        endif()
    else()
        target_link_libraries(${target} PUBLIC boost_glob)
    endif()
endfunction(target_link_boost)

function(target_link_boost_test target)
    if (USE_SYSTEM_BOOST)
        target_compile_definitions(${target} PRIVATE BOOST_TEST_DYN_LINK)
        target_link_libraries(${target} PRIVATE ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
    else()
        target_link_libraries(${target} PUBLIC boost_unit_test)
    endif()
endfunction(target_link_boost_test)

if (USE_SYSTEM_BOOST)
    message("ASL_INFO: Using system boost.")
    set(ASL_BOOST_COMPONENTS system filesystem unit_test_framework program_options)
    if(NOT USE_STD_THREAD_LOCAL)
        list(APPEND ASL_BOOST_COMPONENTS thread)
    endif()
    find_package(Boost COMPONENTS ${ASL_BOOST_COMPONENTS} REQUIRED)
else()
    message("ASL_INFO: Building boost ourselves.")

    setup_dep(NAME boost_libraries URL https://github.com/boostorg/boost.git BRANCH boost-1.60.0)

    # There really isn't a need to separate out these sources, so we build them
    # in one large static library.

    file(GLOB ASL_BOOST_FILESYSTEM_SRC ../boost_libraries/libs/filesystem/src/*.cpp)
    file(GLOB ASL_BOOST_PROGRAM_OPTIONS_SRC ../boost_libraries/libs/program_options/src/*.cpp)
    file(GLOB ASL_BOOST_SYSTEM_SRC ../boost_libraries/libs/system/src/*.cpp)

    set(BOOST_GLOB_SOURCES
          ${ASL_BOOST_FILESYSTEM_SRC}
          ${ASL_BOOST_PROGRAM_OPTIONS_SRC}
          ${ASL_BOOST_SYSTEM_SRC})

    if(NOT USE_STD_THREAD_LOCAL)
        file(GLOB ASL_BOOST_THREAD_SRC ../boost_libraries/libs/thread/src/*.cpp)

        if (${UNIX})
            file(GLOB ASL_BOOST_THREAD_TSS ../boost_libraries/libs/thread/src/pthread/*.cpp)
        elseif (${WIN32})
            set(ASL_BOOST_THREAD_TSS
                ../boost_libraries/libs/thread/src/win32/thread.cpp
                ../boost_libraries/libs/thread/src/win32/tss_pe.cpp # no idea if this is right...
            )
        endif()
        list(APPEND BOOST_GLOB_SOURCES
            ${ASL_BOOST_THREAD_SRC}
            ${ASL_BOOST_THREAD_TSS})
    endif()

    add_library(boost_glob STATIC ${BOOST_GLOB_SOURCES})

    target_include_directories(boost_glob PUBLIC ../boost_libraries)

    # We separate out the unit test framework from the rest of the boost
    # add_library support because it has its own main routine which we only need
    # when building unit tests.

    file(GLOB ASL_BOOST_TEST_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ../boost_libraries/libs/test/src/*.cpp)
    list(REMOVE_ITEM ASL_BOOST_TEST_SRC ../boost_libraries/libs/test/src/cpp_main.cpp)

    add_library(boost_unit_test STATIC ${ASL_BOOST_TEST_SRC})

    target_include_directories(boost_unit_test PRIVATE -DBOOST_TEST_MAIN)
    target_include_directories(boost_unit_test PUBLIC ../boost_libraries)
endif()

include_directories(../stlab)

add_subdirectory(source)
add_subdirectory(test)

