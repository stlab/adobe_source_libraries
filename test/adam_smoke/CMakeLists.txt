add_executable (adam_smoke_test adam_smoke_test.cpp)
target_link_libraries(adam_smoke_test PRIVATE asl)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options (adam_smoke_test PRIVATE "/W4")
    target_compile_options (adam_smoke_test PRIVATE "/WX")
endif()

add_test (NAME adam_smoke_test_rtd COMMAND adam_smoke_test ${CMAKE_CURRENT_SOURCE_DIR}/rtd.adm)
