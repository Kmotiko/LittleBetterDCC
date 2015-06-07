add_executable(parser_test ${PROJECT_SOURCE_DIR}/test/parser/parser_test.cpp)
target_link_libraries(parser_test parser ${Boost_LIBRARIES})
add_test(parser_test parser_test)
