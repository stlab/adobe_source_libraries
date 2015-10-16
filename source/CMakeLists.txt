aux_source_directory(. SRC_LIST)
file(GLOB_RECURSE INC_LIST ../adobe/*.hpp)
add_library(asl ${SRC_LIST} ${INC_LIST})

target_include_directories(asl PUBLIC ..)
target_compile_definitions(asl PUBLIC ADOBE_STD_SERIALIZATION)

#otherwise boost concept checking fails on adobe::selection_t because it lacks non const begin/end
target_compile_definitions(asl PUBLIC -DBOOST_RANGE_ENABLE_CONCEPT_ASSERT=0)

#otherwise boost::move and std::move are ambiguous
target_compile_definitions(asl PUBLIC -DBOOST_MOVE_USE_STANDARD_LIBRARY_MOVE)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
    target_compile_options(asl PUBLIC -fconstexpr-depth=1024)
    target_compile_options(asl PUBLIC -ftemplate-depth=1024)
endif()

target_link_doubleconversion(asl)

target_link_boost(asl)
