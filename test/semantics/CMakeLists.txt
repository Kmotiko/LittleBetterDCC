add_executable(semantics_test ${PROJECT_SOURCE_DIR}/test/semantics/semantics_test.cpp)
target_link_libraries(semantics_test semantic_analyzer parser ${Boost_LIBRARIES})
add_test(semantics_test semantics_test)
