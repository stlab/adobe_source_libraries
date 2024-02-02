cmake_minimum_required(VERSION 3.0)
include(FindGit)
include(CMakeParseArguments)
enable_testing()

project(adobe_source_libraries CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT DEFINED CMAKE_CXX_STANDARD)
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17")
endif(NOT DEFINED CMAKE_CXX_STANDARD)

set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

set(VCPKG_FEATURE_FLAGS "versions")
project(adobe-source-libraries CXX)

add_definitions ("-Wall")
#add_definitions ("-Werror")

# There are two big choices this file makes - how to include Boost, and how to
# include double-conversion, respectively. Build environments vary and we're
# trying to support all of them.
#
# For double-conversion, the unix makefile setup is able to download the git
# repo and build it

option(USE_STLAB_DOUBLECONV "use stlab mirror of double-conversion repository" OFF)

set(root_path ${CMAKE_SOURCE_DIR}/..)

function(setup_dep)
    set(options IS_CMAKE)
    set(oneValueArgs URL BRANCH TAG NAME)
    set(multiValueArgs SUBMODULES)
    cmake_parse_arguments(setup_dep "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if ("${setup_dep_NAME}" STREQUAL "")
        get_filename_component(name ${setup_dep_URL} NAME_WE)
    else()
        set(name ${setup_dep_NAME})
    endif()

    set(dep_path ${root_path}/${name})

    if(NOT IS_DIRECTORY ${dep_path})
        message("ASL_INFO: Setting up dep " ${name} " into " ${dep_path})
        execute_process(COMMAND ${GIT_EXECUTABLE} clone  ${setup_dep_URL} ${name} WORKING_DIRECTORY ${root_path})

        if("${setup_dep_BRANCH}" STREQUAL "")
            message(FATAL_ERROR "ASL_INFO: No branch given for dep " ${name})
        endif()

        execute_process(COMMAND ${GIT_EXECUTABLE} checkout ${setup_dep_BRANCH} WORKING_DIRECTORY ${dep_path})

        foreach(submodule ${setup_dep_SUBMODULES})
            message("ASL_INFO: Setting up submodule " ${submodule} " for " ${name})
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init ${submodule} WORKING_DIRECTORY ${dep_path})
        endforeach(submodule)
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
    target_link_libraries(${target} PUBLIC Boost::system)
endfunction(target_link_boost)

function(target_link_boost_test target)
    target_compile_definitions(${target} PRIVATE BOOST_TEST_DYN_LINK)
    target_link_libraries(${target} PRIVATE Boost::unit_test_framework)
endfunction(target_link_boost_test)

message("ASL_INFO: Using system boost.")
set(ASL_BOOST_COMPONENTS system unit_test_framework program_options)
find_package(Boost COMPONENTS ${ASL_BOOST_COMPONENTS} REQUIRED)

add_subdirectory(source)
add_subdirectory(test)
