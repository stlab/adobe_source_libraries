asl_test(NAME zuidgen SOURCES main.cpp)
target_link_libraries(zuidgen PRIVATE ${Boost_THREAD_LIBRARY})
