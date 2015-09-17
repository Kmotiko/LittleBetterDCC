#ifndef DCC_PARSER_HPP
#define DCC_PARSER_HPP

#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_RESULT_OF_USE_DECLTYPE


#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_core.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "include/helper.hpp"
#include "include/ast/ast.hpp"


namespace dcc {
namespace parser {

template<typename T, typename ... Params>
auto make_shared_ptr(Params && ... param){
  return boost::phoenix::bind([] (auto && ... param)
      {return std::make_shared<T>(std::forward<decltype(param)>(param)...);}, std::forward<Params>(param)...
  );
}

/// call back handler for success parsing
struct on_success_handler{

  /// ctor
  explicit on_success_handler() noexcept{}

  template<typename T, typename Iterator>
  void operator()(std::shared_ptr<T> const &ptr, 
                  Iterator const first,
                  Iterator const end,
                  Iterator const begin) const noexcept{
    // start loc
    ptr->start_loc.line = boost::spirit::get_line(first);
    ptr->start_loc.col = boost::spirit::get_column(begin, first);

    // end loc
    ptr->end_loc.line = boost::spirit::get_line(end);
    ptr->end_loc.col = boost::spirit::get_column(begin, end);
  }

  template<typename Iterator>
  void operator()(dcc::ast::one_of_expr const &ptr,
                  Iterator const first,
                  Iterator const end,
                  Iterator const begin) const{
    auto f = dcc::helper::make_wrapped_function<void>([first, end, begin, this](auto const& node){(*this)(node, first, end, begin);});
    boost::apply_visitor(f, ptr);
  }
};


template<typename Iterator>
struct on_error_handler{
  explicit on_error_handler() noexcept{}
  void operator()(Iterator const first, 
                  Iterator const pos, 
                  Iterator const begin) const{
    // TODO 
    // implement error handling
  }
};



/// grammar to skip comment
template <typename Iterator>
class comment_skipper : public boost::spirit::qi::grammar<Iterator>{
public:
  comment_skipper() : comment_skipper::base_type(skipper){
    skipper = boost::spirit::ascii::space | 
              single_line_comment | 
              multi_line_comment;
    single_line_comment = "//" >> *(boost::spirit::qi::char_ - boost::spirit::qi::eol);
    multi_line_comment = "/*" >> 
                          *(boost::spirit::qi::char_ - &(boost::spirit::qi::lit("*") > boost::spirit::qi::lit("/")))
                           >> boost::spirit::qi::lit("*/");
  }
private:
  boost::spirit::qi::rule<Iterator> skipper;
  boost::spirit::qi::rule<Iterator> single_line_comment;
  boost::spirit::qi::rule<Iterator> multi_line_comment;
};



// TODO
// implement detail of exception
/// my own exception class
class parse_exception : public std::exception{};


/// grammar of dcc syntax
template <typename Iterator, typename Skipper>
class dcc_grammar : public boost::spirit::qi::grammar<
                         Iterator, 
                         dcc::ast::translation_unit_ptr(), 
                         boost::spirit::qi::locals<
                             std::vector<dcc::ast::function_declaration_ptr>,
                             std::vector<dcc::ast::function_definition_ptr>
                        >,
                        Skipper
                     > {
public:
  dcc_grammar(Iterator begin);

private : 
  Iterator begin;
  // top level rules
  boost::spirit::qi::rule<
              Iterator, dcc::ast::translation_unit_ptr(),
              boost::spirit::qi::locals<
                  std::vector<dcc::ast::function_declaration_ptr>,
                  std::vector<dcc::ast::function_definition_ptr>>,
              Skipper> translation_unit;
  boost::spirit::qi::rule<Iterator, dcc::ast::function_declaration_ptr(), Skipper> function_declaration;
  boost::spirit::qi::rule<Iterator, dcc::ast::function_definition_ptr(), Skipper> function_definition;
  boost::spirit::qi::rule<Iterator, dcc::ast::prototype_ptr(),
      boost::spirit::qi::locals<std::vector<dcc::ast::parameter_ptr>>, Skipper> prototype;


  // statement
  boost::spirit::qi::rule<Iterator, dcc::ast::function_statement_ptr(), Skipper> function_statement;
  boost::spirit::qi::rule<Iterator, dcc::ast::one_of_statement(), Skipper> statement;
  boost::spirit::qi::rule<Iterator, dcc::ast::jump_statement_ptr(), Skipper> jump_statement;
  boost::spirit::qi::rule<Iterator, dcc::ast::one_of_expr(), Skipper> expression_statement;
  boost::spirit::qi::rule<Iterator, std::vector<dcc::ast::one_of_statement>(), Skipper> variable_declaration_list;
  boost::spirit::qi::rule<Iterator, dcc::ast::variable_declaration_ptr(), Skipper> variable_declaration;
  boost::spirit::qi::rule<Iterator, std::vector<dcc::ast::one_of_statement>(), Skipper> statement_list;
  boost::spirit::qi::rule<Iterator, dcc::ast::parameter_ptr(), Skipper> parameter;
  boost::spirit::qi::rule<Iterator, std::vector<dcc::ast::parameter_ptr>(), Skipper> parameter_list;
  boost::spirit::qi::rule<Iterator, std::vector<dcc::ast::one_of_expr>(), Skipper> argument_list;

  // expression
  boost::spirit::qi::rule<Iterator, dcc::ast::one_of_expr(), Skipper> assignment_expression;
  boost::spirit::qi::rule<Iterator, dcc::ast::one_of_expr(), Skipper> additive_expression;
  boost::spirit::qi::rule<Iterator, dcc::ast::one_of_expr(), Skipper> multipricative_expression;
  boost::spirit::qi::rule<Iterator, dcc::ast::one_of_expr(), Skipper> postfix_expression;
  boost::spirit::qi::rule<Iterator, dcc::ast::one_of_expr(), Skipper> primary_expression;
  boost::spirit::qi::rule<Iterator, dcc::ast::one_of_expr(), Skipper> number_literal;
  boost::spirit::qi::rule<Iterator, std::string(), Skipper> type_specifier;
  boost::spirit::qi::rule<Iterator, dcc::ast::identifier_ptr(), Skipper, boost::spirit::qi::locals<std::string>> identifier;
  boost::spirit::qi::rule<Iterator, int(), Skipper> integer_literal;

  boost::spirit::qi::rule<Iterator, std::string(), Skipper> reserved_words;
};


/// parser entry
class parser {
public:
  parser();
  static dcc::ast::module parse(std::string const &code, std::string const &name);
private:
};


} // end of namespace
} // end of namespace

#endif

