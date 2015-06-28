asl_test (BOOST NAME sha_smoketest SOURCES main.cpp)
asl_test (BOOST NAME sha_shavs SOURCES shavs.cpp)

asl_test (NAME sha_benchmark SOURCES bench.cpp)
include(FindOpenSSL)
target_include_directories(sha_benchmark PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(sha_benchmark PRIVATE ${OPENSSL_LIBRARIES})
