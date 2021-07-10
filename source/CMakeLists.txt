aux_source_directory(. SRC_LIST)
file(GLOB_RECURSE INC_LIST ../adobe/*.hpp)
add_library(asl ${SRC_LIST} ${INC_LIST})

if(MSVC)
    target_compile_options(asl PRIVATE /W4 /WX)
else()
    target_compile_options(asl PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

target_include_directories(asl PUBLIC ..)
target_compile_definitions(asl PUBLIC ADOBE_STD_SERIALIZATION)

# otherwise boost concept checking fails on adobe::selection_t because it lacks non const begin/end
target_compile_definitions(asl PUBLIC -DBOOST_RANGE_ENABLE_CONCEPT_ASSERT=0)

# otherwise boost::move and std::move are ambiguous
# commented out here because ADOBE_STD_ flags below make it irrelevant
#target_compile_definitions(asl PUBLIC BOOST_MOVE_USE_STANDARD_LIBRARY_MOVE)

if (USE_STD_THREAD_LOCAL)
    target_compile_definitions(asl PUBLIC ADOBE_STD_THREAD_LOCAL)
endif()

# GCC6 warning on boost 1.60
# right operand of shift expression '(1u << 63u)' is >= than the precision of the left operand [-fpermissive]
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_compile_definitions(asl PUBLIC ADOBE_FNV_NO_BIGINTS)
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
    target_compile_options(asl PUBLIC -fconstexpr-depth=1024)
    target_compile_options(asl PUBLIC -ftemplate-depth=1024)
endif()

target_link_libraries(asl PUBLIC double-conversion)

target_link_boost(asl)

# thread linking
# worse that missing symbols, without this, libstdc++'s std::call_once can crash with system_error
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=55394
# seems we get it for free when linking to system's boost but let's make sure
include(FindThreads)
target_link_libraries(asl PUBLIC ${CMAKE_THREAD_LIBS_INIT})
