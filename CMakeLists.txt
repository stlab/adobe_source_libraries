cmake_minimum_required(VERSION 3.0)
include(ExternalProject)
include(CMakeParseArguments)
enable_testing()

project(adobe_source_libraries CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)

link_directories(${CMAKE_INSTALL_PREFIX}/lib)

ExternalProject_Add(double-conversion_prj
    PREFIX ${CMAKE_BINARY_DIR}/thirdparty/double-conversion
    GIT_REPOSITORY https://github.com/google/double-conversion.git
    #GIT_REPOSITORY https://github.com/stlab/double-conversion.git
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})

function(target_link_doubleconversion target)

    #tweak on include double-conversion path
    target_compile_definitions(${target} PUBLIC -DADOBE_BUILT_WITH_CMAKE)

    add_dependencies(${target} double-conversion_prj)
    target_include_directories(${target} PUBLIC ${CMAKE_INSTALL_PREFIX}/include)
    target_link_libraries(${target} PUBLIC double-conversion)
endfunction(target_link_doubleconversion)

find_package(Boost COMPONENTS system thread unit_test_framework program_options REQUIRED)

add_subdirectory(source)
add_subdirectory(test)
