include(CTest)
enable_testing()

function(asl_test)
  set(options SKIPPED BOOST BENCHMARK)
  set(oneValueArgs NAME)
  set(multiValueArgs SOURCES ARGS)
  cmake_parse_arguments(asl_test "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  add_executable(${asl_test_NAME} ${asl_test_SOURCES})

  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_compile_options(${asl_test_NAME} PRIVATE "/W4")
    target_compile_options(${asl_test_NAME} PRIVATE "/WX")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND WIN32)
    target_compile_options(${asl_test_NAME} PRIVATE
      "/W4"
      "/WX"
      "/EHsc"
      "/DNOMINMAX"
      "-Wno-unused-variable"
      "-Wno-unknown-pragmas"
      "-Wno-uninitialized"
      "-Wno-return-type"
      "-Wno-unused-function"
      "-Wno-unused-local-typedefs"
      "-Wno-unused-parameter"
    )
  else()
    target_compile_options(${asl_test_NAME} PRIVATE "-Wno-unused-variable")
    target_compile_options(${asl_test_NAME} PRIVATE "-Wno-unknown-pragmas")
    target_compile_options(${asl_test_NAME} PRIVATE "-Wno-uninitialized")
    target_compile_options(${asl_test_NAME} PRIVATE "-Wno-return-type")
    target_compile_options(${asl_test_NAME} PRIVATE "-Wno-unused-function")
    target_compile_options(${asl_test_NAME} PRIVATE "-Wno-unused-local-typedefs")
    target_compile_options(${asl_test_NAME} PRIVATE "-Wno-unused-parameter")
  endif()

  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(${asl_test_NAME} PRIVATE "-Wno-unused-comparison")
  endif()

  target_link_libraries(${asl_test_NAME} PRIVATE asl)

  if(asl_test_BOOST)
    target_link_boost_test(${asl_test_NAME})
  endif(asl_test_BOOST)

  if(NOT asl_test_SKIPPED)
    if(asl_test_BENCHMARK)
      add_test(NAME ${asl_test_NAME} COMMAND ${asl_test_NAME} ${asl_test_ARGS} CONFIGURATIONS Release)
    else(asl_test_BENCHMARK)
      add_test(NAME ${asl_test_NAME} COMMAND ${asl_test_NAME} ${asl_test_ARGS})
    endif(asl_test_BENCHMARK)
  endif(NOT asl_test_SKIPPED)
endfunction(asl_test)

add_subdirectory(adam_smoke)
add_subdirectory(adam_tutorial)
add_subdirectory(algorithm)
add_subdirectory(any_regular)
add_subdirectory(arg_stream)
add_subdirectory(closed_hash)
add_subdirectory(cmath)
add_subdirectory(conversion)
add_subdirectory(copy_on_write)
add_subdirectory(dictionary_benchmark)
add_subdirectory(enum_ops)
add_subdirectory(equal_range)
add_subdirectory(erase)
add_subdirectory(eve_smoke)
add_subdirectory(expression_parser)
add_subdirectory(fnv)
add_subdirectory(forest_smoke)
add_subdirectory(forest)
add_subdirectory(functional)
add_subdirectory(json)
add_subdirectory(lex_stream)
add_subdirectory(lower_bound)
add_subdirectory(md5)
add_subdirectory(n_queens)
add_subdirectory(name)
add_subdirectory(poly)
add_subdirectory(property_model_eval)
add_subdirectory(reduction)
add_subdirectory(selection)
add_subdirectory(serialization)
add_subdirectory(sha)
add_subdirectory(stable_partition_selection)
add_subdirectory(to_string)
add_subdirectory(unicode)
add_subdirectory(xml_parser)
add_subdirectory(zuidgen)
