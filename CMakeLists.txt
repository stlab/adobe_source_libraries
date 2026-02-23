cmake_minimum_required(VERSION 3.29)
include(cmake/CPM.cmake)

# Add cpp-library for set version
# CPMAddPackage(
#     NAME cpp-library
#     SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../cpp-library"
# )
CPMAddPackage("gh:stlab/cpp-library@5.2.0")
include(${cpp-library_SOURCE_DIR}/cpp-library.cmake)

project(adobe-source-libraries CXX)

cpp_library_set_version()

################################################################################

include(FindGit)
include(CMakeParseArguments)

if (PROJECT_IS_TOP_LEVEL)
    include(CTest)
endif()

if(WIN32)
    set(CMAKE_OBJECT_PATH_MAX 500)
endif()

################################################################################
# Dependencies

# https://github.com/stlab/copy-on-write/releases
CPMAddPackage("gh:stlab/copy-on-write@1.1.0")

# https://github.com/stlab/enum-ops/releases
CPMAddPackage(gh:stlab/enum-ops@1.2.0)
# CPMAddPackage(NAME enum-ops
#     URL "${CMAKE_SOURCE_DIR}/../enum-ops"
# )

################################################################################

set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif(NOT DEFINED CMAKE_CXX_STANDARD)

set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

# https://github.com/boostorg/boost/releases
CPMAddPackage(
    NAME Boost
    VERSION 1.90.0
    URL https://github.com/boostorg/boost/releases/download/boost-1.90.0/boost-1.90.0-cmake.7z
    URL_HASH SHA256=218e74c4aa362a994b7b7a23b2920f455a00205c656405fcf262cf60b8871921
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
    target_link_libraries(${target} PUBLIC stlab::copy-on-write)
    target_link_libraries(${target} PUBLIC stlab::enum-ops)
endfunction(target_link_boost)

function(target_link_boost_test target)
    target_link_libraries(${target} PRIVATE Boost::unit_test_framework)
    target_link_libraries(${target} PRIVATE Boost::program_options)
endfunction(target_link_boost_test)

add_subdirectory(source)

if(PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
    add_subdirectory(test)
endif()

# Documentation with Doxygen
option(BUILD_DOCS "Build documentation" OFF)

if(BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    
    if(DOXYGEN_FOUND)
        # Download doxygen-awesome-css theme via CPM
        # https://github.com/jothepro/doxygen-awesome-css
        CPMAddPackage(
            URI gh:jothepro/doxygen-awesome-css@2.4.1
            DOWNLOAD_ONLY YES
        )
        set(AWESOME_CSS_DIR ${doxygen-awesome-css_SOURCE_DIR})
        
        # Configure Doxyfile
        set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/documentation/doxyfile.in)
        set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        
        # Set variables for Doxyfile template
        set(PROJECT_NAME "Adobe Source Libraries")
        set(PROJECT_BRIEF "A collection of C++ libraries.")
        set(PROJECT_VERSION ${PROJECT_VERSION})
        set(INPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        set(OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
        set(AWESOME_CSS_PATH "${AWESOME_CSS_DIR}")
        set(CMAKE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        
        configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)
        
        # Add custom target for documentation
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
        
        # Ensure the output directory exists
        file(MAKE_DIRECTORY ${OUTPUT_DIR})
        
        message(STATUS "Documentation target 'docs' configured")
        message(STATUS "Run 'cmake --build . --target docs' to generate documentation")
    else()
        message(WARNING "Doxygen not found. Documentation will not be built.")
    endif()
endif() 
