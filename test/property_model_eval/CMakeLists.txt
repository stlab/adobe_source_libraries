asl_test(NAME property_model_eval
SOURCES pmeval.cpp adam_test_parser.cpp queryable_sheet.cpp iomanip_flat.cpp
ARGS ${CMAKE_CURRENT_SOURCE_DIR}/default.pme ${CMAKE_CURRENT_SOURCE_DIR}/minmax.pme ${CMAKE_CURRENT_SOURCE_DIR}/pm_tut.pme)

target_link_libraries(property_model_eval PRIVATE ${Boost_PROGRAM_OPTIONS_LIBRARY})
