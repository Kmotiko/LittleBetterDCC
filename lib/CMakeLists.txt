add_subdirectory(ast)
add_subdirectory(parser)
add_subdirectory(semantic)
add_subdirectory(codegen)

add_executable(dcc dcc.cpp)
target_link_libraries(dcc
              ast
              parser
              semantic_analyzer
              codegen
              boost_program_options
              ${LLVM_LIB_NAMES}
              )
install(TARGETS dcc
  DESTINATION bin)
