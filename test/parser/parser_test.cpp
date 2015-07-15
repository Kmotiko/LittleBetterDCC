#define BOOST_TEST_MODULE parser_test
#include "include/parser/parser.hpp"
#include<boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(parser_test)

BOOST_AUTO_TEST_CASE(normal)
{
  std::string normal_test = R"( 
      int main()
      {
        int i;
        i = 0;
        i=i+1;
        return 0;
      }
  )";
  BOOST_CHECK_NO_THROW(dcc::parser::parser::parse(normal_test, "normal"));
}


BOOST_AUTO_TEST_CASE(call_expr)
{
  std::string call_test1 = R"( 
      int hoge()
      {
        return 1;
      }
      int main()
      {
        hoge();
        return 0;
      }
  )";
  BOOST_CHECK_NO_THROW(dcc::parser::parser::parse(call_test1, "call_test 1"));

  std::string call_test2 = R"( 
      int hoge()
      {
        return 1;
      }
      int main()
      {
        hoge();
        return 0;
      }
  )";
  BOOST_CHECK_NO_THROW(dcc::parser::parser::parse(call_test2, "call_test2"));
}


BOOST_AUTO_TEST_CASE(bin_expr_test)
{
  std::string bin_expr1 = R"( 
      int main()
      {
        int i;
        i = 0;
        i = i + 1;
        i = i + 1 + 1;
        return 0;
      }
  )";
  BOOST_CHECK_NO_THROW(dcc::parser::parser::parse(bin_expr1, "bin_expr 1"));

  std::string bin_expr2 = R"( 
      int main()
      {
        int i;
        i = 10;
        i = i - 1;
        i = i - 1 - 1;
        return 0;
      }
  )";
  BOOST_CHECK_NO_THROW(dcc::parser::parser::parse(bin_expr2, "bin_expr2"));


  std::string bin_expr3 = R"( 
      int main()
      {
        int i;
        i = 1;
        i = i * 1;
        i = i * 10 * 2;
        return 0;
      }
  )";
  BOOST_CHECK_NO_THROW(dcc::parser::parser::parse(bin_expr3, "bin_expr3"));


  std::string bin_expr4 = R"( 
      int main()
      {
        int i;
        i = 100;
        i = i / 10 / 2;
        return 0;
      }
      )";
  BOOST_CHECK_NO_THROW(dcc::parser::parser::parse(bin_expr4, "bin_expr4"));

  std::string bin_expr5 = R"( 
      int main()
      {
        int i;
        i = 100;
        i = i / 10 / 2;
        i = (i - 10 / 5) + ( 10 / 2 + 5);
        return 0;
      }
  )";
  BOOST_CHECK_NO_THROW(dcc::parser::parser::parse(bin_expr5, "bin_expr5"));
}

BOOST_AUTO_TEST_CASE(comment_test)
{
  std::string skip_comment = R"( 
      int main()
      {
        // this is single line comment
        i = i * 1;
        
        /*
        this is multi line comment
        */
        i = i * 10;
        return 0;
      }
  )";
  BOOST_CHECK_NO_THROW(dcc::parser::parser::parse(skip_comment, "skip_comment"));
}


BOOST_AUTO_TEST_SUITE_END()
