cmake_minimum_required(VERSION 3.29)
include(FindGit)
include(CMakeParseArguments)

if(WIN32)
    set(CMAKE_OBJECT_PATH_MAX 500)
endif()

project(adobe_source_libraries CXX)
include(CTest)

# download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# Enable CPM caching to avoid re-downloading dependencies
set(CPM_SOURCE_CACHE ${CMAKE_SOURCE_DIR}/.cpm-cache CACHE PATH "Directory to cache CPM packages" FORCE)

################################################################################
# Dependencies

CPMAddPackage("gh:stlab/copy-on-write@1.0.3")

################################################################################

set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif(NOT DEFINED CMAKE_CXX_STANDARD)

set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

# Create symlink to compile_commands.json for clangd
if(CMAKE_EXPORT_COMPILE_COMMANDS AND TOP_LEVEL_PROJECT)
    add_custom_target(clangd_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
        COMMENT "Creating symlink to compile_commands.json for clangd"
    )
endif()

project(adobe-source-libraries CXX)

# https://github.com/boostorg/boost/releases
CPMAddPackage(
    NAME Boost
    VERSION 1.88.0
    URL https://github.com/boostorg/boost/releases/download/boost-1.88.0/boost-1.88.0-cmake.7z
    URL_HASH SHA256=feba537f6d32c062ed0f26b4d6e5b8e8450d51ef81492369a101ab99fcead278
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
endfunction(target_link_boost)

function(target_link_boost_test target)
    target_link_libraries(${target} PRIVATE Boost::unit_test_framework)
    target_link_libraries(${target} PRIVATE Boost::program_options)
endfunction(target_link_boost_test)

add_subdirectory(source)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()

# Documentation with Doxygen
option(BUILD_DOCS "Build documentation" OFF)

if(BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    
    if(DOXYGEN_FOUND)
        # Download doxygen-awesome-css theme via CPM
        CPMAddPackage(
            NAME doxygen-awesome-css
            GITHUB_REPOSITORY jothepro/doxygen-awesome-css
            GIT_TAG v2.3.4
            DOWNLOAD_ONLY YES
        )
        set(AWESOME_CSS_DIR ${doxygen-awesome-css_SOURCE_DIR})
        
        # Configure Doxyfile
        set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/documentation/doxyfile.in)
        set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        
        # Set variables for Doxyfile template
        set(PROJECT_NAME "Adobe Source Libraries")
        set(PROJECT_BRIEF "A collection of C++ libraries.")
        set(PROJECT_VERSION "2.0.0")
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
