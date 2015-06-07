add_subdirectory(ast)
add_subdirectory(parser)
add_subdirectory(semantic)
add_subdirectory(codegen)

add_executable(dcc dcc.cpp)
target_link_libraries(dcc
              ast
              codegen
              parser
              semantic_analyzer
              boost_program_options-mt
              )
install(TARGETS dcc
  DESTINATION bin)
