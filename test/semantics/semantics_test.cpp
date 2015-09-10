#define BOOST_TEST_MODULE semantics_test
#include "include/parser/parser.hpp"
#include "include/semantic/semantic_analayzer.hpp"
#include<boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(semantics_test)

BOOST_AUTO_TEST_CASE(variable_symbol)
{
  // check resolve symbol with no error
  std::string resolve_var = R"( 
      int main()
      {
        int i;
        i = 0;
        i = i+1;
        return 0;
      }
  )";
  dcc::ast::module mod = dcc::parser::parser::parse(resolve_var, "normal");
  BOOST_CHECK_NO_THROW(dcc::semantic::semantic_analayzer::analyze(mod));
}

BOOST_AUTO_TEST_CASE(function_symbol)
{
  // check resolve symbol with no error
  std::string resolve_func = R"( 
      int hoge(int i){
        return i;
      }
      int main()
      {
        int i;
        i = 0;
        hoge(i);
        return 0;
      }
  )";
  dcc::ast::module mod = dcc::parser::parser::parse(resolve_func, "normal");
  BOOST_CHECK_NO_THROW(dcc::semantic::semantic_analayzer::analyze(mod));
}


BOOST_AUTO_TEST_CASE(func_parameter)
{
  std::string invalid_func_ref = R"( 
      int hoge(int i){
        return i;
      }
      int main()
      {
        int i;
        i = 0;
        hoge();
        return 0;
      }
  )";
  dcc::ast::module mod = dcc::parser::parser::parse(invalid_func_ref, "invalid_func_ref");
  // expect fail beacuse of invalid function invocation
  BOOST_CHECK_THROW(dcc::semantic::semantic_analayzer::analyze(mod), dcc::semantic::semantic_exception);
}


BOOST_AUTO_TEST_CASE(invalid_reference)
{
  std::string invalid_var_ref = R"( 
      int main()
      {
        int i;
        i = 0;
        return j;
      }
  )";
  dcc::ast::module mod = dcc::parser::parser::parse(invalid_var_ref, "invalid_var_ref");
  // expect fail beacuse of invalid variable reference
  BOOST_CHECK_THROW(dcc::semantic::semantic_analayzer::analyze(mod), dcc::semantic::semantic_exception);
}

BOOST_AUTO_TEST_SUITE_END()

