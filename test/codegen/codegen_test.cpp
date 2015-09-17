#define BOOST_TEST_MODULE codegen_test
#include "include/parser/parser.hpp"
#include "include/semantic/semantic_analayzer.hpp"
#include "include/codegen/codegen.hpp"
#include "include/codegen/ir_generator.hpp"
#include<boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(codegen_test)

BOOST_AUTO_TEST_CASE(normal_case)
{
  std::string normal = R"( 
      int main()
      {
        int i;
        i = 0;
        i = i+1;
        return 0;
      }
  )";
  dcc::ast::module mod = dcc::parser::parser::parse(normal, "normal");
  dcc::semantic::scope::symbol_table scope = dcc::semantic::semantic_analayzer::analyze(mod);
  BOOST_CHECK_NO_THROW(dcc::codegen::ir_generator().generate(mod, scope));
}


BOOST_AUTO_TEST_CASE(function_call)
{
  std::string func_call = R"( 
      int foo(int i){
        return i;
      }

      int main()
      {
        int i;
        i = 0;
        i = i+1;
        foo(i);
        return 0;
      }
  )";
  dcc::ast::module mod = dcc::parser::parser::parse(func_call, "func_call");
  dcc::semantic::scope::symbol_table scope = dcc::semantic::semantic_analayzer::analyze(mod);
  BOOST_CHECK_NO_THROW(dcc::codegen::ir_generator().generate(mod, scope));
}


BOOST_AUTO_TEST_SUITE_END()


