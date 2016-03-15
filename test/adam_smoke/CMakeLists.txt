add_executable (adam_smoke_test adam_smoke_test.cpp)
target_link_libraries(adam_smoke_test PRIVATE asl)

add_test (NAME adam_smoke_test_rtd COMMAND adam_smoke_test ut_mode ${CMAKE_CURRENT_SOURCE_DIR}/rtd.adm)
add_test (NAME adam_smoke_test_io COMMAND adam_smoke_test ut_mode ${CMAKE_CURRENT_SOURCE_DIR}/io.adm)
