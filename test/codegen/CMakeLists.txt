add_executable(codegen_test ${PROJECT_SOURCE_DIR}/test/codegen/codegen_test.cpp)
target_link_libraries(codegen_test 
                      codegen 
                      semantic_analyzer 
                      parser 
                      ${Boost_LIBRARIES}
                      ${LLVM_LIBS}
                      )
add_test(codegen_test codegen_test)
