#include "include/parser/parser.hpp"

namespace dcc{
namespace parser{

/// ctor
template <typename Iterator, typename Skipper>
dcc_grammar<Iterator, Skipper>::dcc_grammar(Iterator begin)
    : dcc_grammar::base_type(translation_unit), begin(begin){

  // reserved_words
  reserved_words = boost::spirit::qi::lexeme
                    [boost::spirit::qi::lit("return") | boost::spirit::qi::lit("int")];

  // number_literal
  number_literal = integer_literal [boost::spirit::qi::_val = 
                        make_shared_ptr<dcc::ast::number_literal>(boost::spirit::qi::_1)];
  integer_literal = boost::spirit::qi::lexeme[boost::spirit::qi::int_];

  // primary expression
  // VARIABLE_IDENTIFIER
  // | CONSTANT
  // | '(' , assignment_expression , ')'
  primary_expression = (identifier - reserved_words)[boost::spirit::qi::_val = boost::spirit::qi::_1] |
                        number_literal[boost::spirit::qi::_val = boost::spirit::qi::_1] |
                        ('(' > assignment_expression > ')')[boost::spirit::qi::_val = boost::spirit::qi::_1];

  // type_specifier
  type_specifier = boost::spirit::qi::lit("int");

  // identifier
  identifier = (boost::spirit::qi::lexeme [(boost::spirit::qi::alpha |
                boost::spirit::qi::char_( '_'))[boost::spirit::_a += boost::spirit::qi::_1] >> 
                *(boost::spirit::qi::alnum | boost::spirit::qi::char_( '_'))
                [boost::spirit::_a += boost::spirit::qi::_1]])
                [boost::spirit::qi::_val = make_shared_ptr<dcc::ast::identifier>(boost::spirit::_a)];

  // postfix expression
  // | identifier , '(' , [ argument_list  ] , ')'
  // primary_expression
  postfix_expression = (identifier >> '(' >> argument_list >> ')')
                        [boost::spirit::qi::_val = 
                          make_shared_ptr<dcc::ast::call_expression>(boost::spirit::qi::_1, boost::spirit::qi::_2)] | 
                          primary_expression[boost::spirit::qi::_val = boost::spirit::qi::_1];

  // multipricative expression
  // | postfix_expression , [ { "*" , postfix_expression  |  "/" ,
  // postfix_expression } ]
  multipricative_expression =
      (postfix_expression)[boost::spirit::qi::_val = boost::spirit::qi::_1] >>
      *(('*' > postfix_expression)[boost::spirit::qi::_val =
                                 make_shared_ptr<dcc::ast::binary_expression>(
                                     std::string("*"), boost::spirit::qi::_val, boost::spirit::qi::_1)] |
        ('/' > postfix_expression)[boost::spirit::qi::_val =
                                 make_shared_ptr<dcc::ast::binary_expression>(
                                     std::string("/"), boost::spirit::qi::_val, boost::spirit::qi::_1)]);

  // additive expression
  // multiplicative_expression , [ { "+" , multiplicative_expression  |  "-" ,
  // multiplicative_expression } ]
  additive_expression =
      (multipricative_expression)[boost::spirit::qi::_val = boost::spirit::qi::_1] >>
      *(("+" > multipricative_expression) [boost::spirit::qi::_val =
                 make_shared_ptr<dcc::ast::binary_expression>(
                     std::string("+"), boost::spirit::qi::_val, boost::spirit::qi::_1)] |
        ("-" > multipricative_expression) [boost::spirit::qi::_val =
                 make_shared_ptr<dcc::ast::binary_expression>(
                     std::string("-"), boost::spirit::qi::_val, boost::spirit::qi::_1)]);

  // assignment expression
  // identifier , '=' , additive_expression
  // | additive_expression
  assignment_expression =
      (identifier >> '=' > additive_expression)[boost::spirit::qi::_val =
                                make_shared_ptr<dcc::ast::binary_expression>(
                                    std::string("="), boost::spirit::qi::_1, boost::spirit::qi::_2)] |
       additive_expression[boost::spirit::qi::_val = boost::spirit::qi::_1];

  // argument list
  // assignment_expression , [ { "," , assignment_expression } ]
  argument_list = -(assignment_expression[boost::phoenix::push_back(boost::spirit::qi::_val, boost::spirit::qi::_1)] % ',');

  // [ assignment_expression ] , ';'
  expression_statement = assignment_expression[boost::spirit::qi::_val = boost::spirit::qi::_1] > ';';

  // statement
  // expression_statement |
  // jump_statement
  statement = (expression_statement | jump_statement);

  // parameter list
  // statement list
  // { statement }
  statement_list = (*(statement[boost::phoenix::push_back(
      boost::spirit::qi::_val, boost::spirit::qi::_1)]));

  // jump statement
  // RETURN , assignment_expression , ';'
  jump_statement = boost::spirit::qi::lit("return") > assignment_expression
                  [boost::spirit::qi::_val = make_shared_ptr<dcc::ast::jump_statement>(boost::spirit::qi::_1)] > ';';

  // variable declaration
  // type_specifier , IDENTIFIER , ";"
  variable_declaration = (type_specifier >> identifier >> ';')[boost::spirit::qi::_val =
                          make_shared_ptr<dcc::ast::variable_declaration>(
                              boost::spirit::qi::_1, boost::spirit::qi::_2)];

  // variable declaration list
  // { variable_declaration }
  variable_declaration_list = (*(variable_declaration[boost::phoenix::push_back(
                                boost::spirit::qi::_val, boost::spirit::qi::_1)]));

  // function statement
  // "{" , [ variable_declaration_list ] , statement_list , "}"
  function_statement = ('{' >> variable_declaration_list >> statement_list >>
                        '}')[boost::spirit::qi::_val =
                                make_shared_ptr<dcc::ast::function_statement>(
                                    boost::spirit::qi::_1, boost::spirit::qi::_2)];

  // parameter
  // type_specifier , IDENTIFIER
  parameter = type_specifier > identifier
              [boost::spirit::qi::_val = make_shared_ptr<dcc::ast::parameter>(boost::spirit::qi::_1)];

  // expression statement
  // parameter , [ { "," , parameter } ]
  parameter_list = -(parameter[boost::phoenix::push_back(boost::spirit::qi::_val, boost::spirit::qi::_1)] % ',');

  // prototype
  // type_specifier , IDENTIFIER , '(' , [ parameter , { "," , parameter} ] ,
  // ')'
  prototype = (type_specifier >> identifier >> '(' >> parameter_list >> ')')
              [boost::spirit::qi::_val = make_shared_ptr<dcc::ast::prototype>(
                         boost::spirit::qi::_2, boost::spirit::qi::_3)];

  // function declaration
  // prototype , ";"
  function_declaration = (prototype >> ';')[boost::spirit::qi::_val =
                          make_shared_ptr<dcc::ast::function_declaration>( boost::spirit::qi::_1)];

  // function defenition
  // prototype , function statement
  function_definition = (prototype >> function_statement)
                        [boost::spirit::qi::_val = 
                            make_shared_ptr<dcc::ast::function_definition>(
                              boost::spirit::qi::_1, boost::spirit::qi::_2)];

  // translation unit
  // { function_declaration | function_definition }
  translation_unit = (*(function_declaration[boost::phoenix::push_back(
                                                boost::spirit::_a, boost::spirit::qi::_1)] |
                        function_definition[boost::phoenix::push_back(
                                                boost::spirit::_b, boost::spirit::qi::_1)]) >
                        boost::spirit::qi::eoi)[boost::spirit::qi::_val =
                                                make_shared_ptr<dcc::ast::translation_unit>(
                                                  boost::spirit::_a, boost::spirit::_b)];

  /// call_back on success
  using on_success_handler = boost::phoenix::function<on_success_handler>;
  boost::spirit::qi::on_success(translation_unit,
                                on_success_handler()(boost::spirit::qi::_val,
                                                     boost::spirit::qi::_1,
                                                     boost::spirit::qi::_3,
                                                     begin));
  boost::spirit::qi::on_success(variable_declaration,
                                on_success_handler()(boost::spirit::qi::_val,
                                                     boost::spirit::qi::_1,
                                                     boost::spirit::qi::_3,
                                                     begin));
  boost::spirit::qi::on_success(jump_statement,
                                on_success_handler()(boost::spirit::qi::_val,
                                                     boost::spirit::qi::_1,
                                                     boost::spirit::qi::_3,
                                                     begin));
  boost::spirit::qi::on_success(assignment_expression,
                                on_success_handler()(boost::spirit::qi::_val,
                                                     boost::spirit::qi::_1,
                                                     boost::spirit::qi::_3,
                                                     begin));
  boost::spirit::qi::on_success(primary_expression,
                                on_success_handler()(boost::spirit::qi::_val,
                                                     boost::spirit::qi::_1,
                                                     boost::spirit::qi::_3,
                                                     begin));
  boost::spirit::qi::on_success(identifier,
                                on_success_handler()(boost::spirit::qi::_val,
                                                     boost::spirit::qi::_1,
                                                     boost::spirit::qi::_3,
                                                     begin));

  // error handler
  using on_error_handler = boost::phoenix::function<on_error_handler<Iterator>>;
  boost::spirit::qi::on_error<boost::spirit::qi::fail>(
      translation_unit,
      on_error_handler()(boost::spirit::qi::_1, boost::spirit::qi::_3, begin));
}


///
dcc::ast::module parser::parse(std::string const &code,
                               std::string const &name) {
  boost::spirit::line_pos_iterator<std::string::const_iterator> const iter =
      boost::spirit::line_pos_iterator<std::string::const_iterator>(
          code.begin());
  boost::spirit::line_pos_iterator<std::string::const_iterator> const end =
      boost::spirit::line_pos_iterator<std::string::const_iterator>(code.end());

  // TODO
  // implement comment skipper instead of ascii::space_type
  dcc::parser::dcc_grammar<
      boost::spirit::line_pos_iterator<std::string::const_iterator>,
      boost::spirit::ascii::space_type> grammar(iter);
  dcc::ast::translation_unit_ptr result;
  bool success = boost::spirit::qi::phrase_parse(
      iter, end, grammar, boost::spirit::ascii::space, result);
  if (!success || iter == end) {
    // TODO
    // set more information...
    throw parse_exception();
  }
  return dcc::ast::module(result, name);
}


} // end of namespace parser
} // end of namespace dcc

