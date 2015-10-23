# quite long pure perf benchmarck. not sure we want to run it automatically
asl_test(BENCHMARK NAME dictionary_benchmark SOURCES main.cpp)
target_link_libraries(dictionary_benchmark PRIVATE ${Boost_THREAD_LIBRARY})

