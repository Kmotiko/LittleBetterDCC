#ifndef DCC_VISITOR_HPP
#define DCC_VISITOR_HPP

#include "include/helper.hpp"
#include "include/ast/ast.hpp"

#include <boost/variant/apply_visitor.hpp>

namespace dcc{
namespace ast{
namespace helper{

class visitor{
public:
  explicit visitor ()noexcept{}
  virtual ~visitor(){}

  virtual void visit(dcc::ast::translation_unit_ptr &) = 0;
  virtual void visit(dcc::ast::function_declaration_ptr &) = 0;
  virtual void visit(dcc::ast::function_definition_ptr &) = 0;
  virtual void visit(dcc::ast::prototype_ptr &) = 0;
  virtual void visit(dcc::ast::parameter_ptr &) = 0;
  virtual void visit(dcc::ast::function_statement_ptr &) = 0;
  virtual void visit(dcc::ast::variable_declaration_ptr &) = 0;
  virtual void visit(dcc::ast::binary_expression_ptr &) = 0;
  virtual void visit(dcc::ast::call_expression_ptr &) = 0;
  virtual void visit(dcc::ast::jump_statement_ptr &) = 0;
  virtual void visit(dcc::ast::identifier_ptr &) = 0;
  virtual void visit(dcc::ast::number_literal_ptr &) = 0;
};


template<typename Visitor>
class accepter{
public:
  Visitor &visitor;
  accepter(Visitor &visitor) : visitor(visitor){}

  void accept(dcc::ast::translation_unit_ptr &tu){
    visitor.visit(tu);
    for(auto func: tu->func_decls)
      accept(func);
    for(auto func: tu->functions)
      accept(func);
  }

  void accept(dcc::ast::function_declaration_ptr &fdecl){
    visitor.visit(fdecl);
    accept(fdecl->signature);
  }

  void accept(dcc::ast::function_definition_ptr &func){
    visitor.visit(func);
    accept(func->signature);
    accept(func->statements);
  }

  void accept(dcc::ast::prototype_ptr &proto){
    visitor.visit(proto);
    for(auto param : proto->param_list)
      accept(param);
  }

  void accept(dcc::ast::parameter_ptr &param){
    visitor.visit(param);
  }
  
  void accept(dcc::ast::function_statement_ptr &fstmt){
    visitor.visit(fstmt);
    for(auto stmt: fstmt->var_decls)
      accept(stmt);
    for(auto stmt: fstmt->statements)
      accept(stmt);
  }
  
  void accept(dcc::ast::variable_declaration_ptr &vdecl){
    visitor.visit(vdecl);
  }
  
  void accept(dcc::ast::binary_expression_ptr &expr){
    visitor.visit(expr);
    accept(expr->lhs);
    accept(expr->rhs);
  }
  

  void accept(dcc::ast::call_expression_ptr &expr){
    visitor.visit(expr);
    for(auto expr: expr->argument_list)
      accept(expr);
  }
  
  void accept(dcc::ast::jump_statement_ptr &jstmt){
    visitor.visit(jstmt);
    accept(jstmt->ret_expr);
  }
  
  void accept(dcc::ast::identifier_ptr &id){
    visitor.visit(id);
  }
  
  void accept(dcc::ast::number_literal_ptr & numlit){
    visitor.visit(numlit);
  }
  
  void accept(dcc::ast::one_of_expr &expr){
    auto f = dcc::helper::make_wrapped_function<void>(
                [this](auto &expr){accept(expr);}
            );
    boost::apply_visitor(f, expr);
  }
  
  void accept(dcc::ast::one_of_statement &stmt){
    auto f = dcc::helper::make_wrapped_function<void>(
                [this](auto &stmt){accept(stmt);}
            );
    boost::apply_visitor(f, stmt);
  }
  

private:

};



} // end of namespace helper
} // end of namespace ast
} // end of namespace dcc

#endif
