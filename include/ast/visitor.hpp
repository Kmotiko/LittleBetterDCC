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

  virtual void visit(dcc::ast::module &){}
  virtual void visit(dcc::ast::translation_unit_ptr &){}
  virtual void visit(dcc::ast::function_declaration_ptr &){}
  virtual void visit(dcc::ast::function_definition_ptr &){}
  virtual void visit(dcc::ast::prototype_ptr &){}
  virtual void visit(dcc::ast::parameter_ptr &){}
  virtual void visit(dcc::ast::function_statement_ptr &){}
  virtual void visit(dcc::ast::variable_declaration_ptr &){}
  virtual void visit(dcc::ast::binary_expression_ptr &){}
  virtual void visit(dcc::ast::call_expression_ptr &){}
  virtual void visit(dcc::ast::jump_statement_ptr &){}
  virtual void visit(dcc::ast::identifier_ptr &){}
  virtual void visit(dcc::ast::number_literal_ptr &){}


  virtual void handle_pre_module(){}
  virtual void handle_post_module(){}
  virtual void handle_pre_translation_unit(){}
  virtual void handle_post_translation_unit(){}
  virtual void handle_pre_func_def(){}
  virtual void handle_post_func_def(){}
  virtual void handle_pre_func_stmt(){}
  virtual void handle_post_func_stmt(){}


  virtual void handle_pre_func_decl(){}
  virtual void handle_post_func_decl(){}
  virtual void handle_pre_prototype(){}
  virtual void handle_post_prototype(){}
  virtual void handle_pre_parameter(){}
  virtual void handle_post_parameter(){}
  virtual void handle_pre_variable_decl(){}
  virtual void handle_post_variable_decl(){}
  virtual void handle_pre_binary_expr(){}
  virtual void handle_post_binary_expr(){}
  virtual void handle_pre_call_expr(){}
  virtual void handle_post_call_expr(){}
  virtual void handle_pre_jump_stmt(){}
  virtual void handle_post_jump_stmt(){}
  virtual void handle_pre_identifier(){}
  virtual void handle_post_identifier(){}
  virtual void handle_pre_number(){}
  virtual void handle_post_number(){}
private:
};


// accept ast and traverse it.
// TODO: rewrite...
class ast_accepter{
public:
  class visitor &visitor;
  ast_accepter(class visitor &visitor) : visitor(visitor){}

  void accept(dcc::ast::module &mod){
    visitor.handle_pre_module();
    visitor.visit(mod);
    accept(mod.tunit);
    visitor.handle_post_module();
  }

  void accept(dcc::ast::translation_unit_ptr &tu){
    visitor.handle_pre_translation_unit();
    visitor.visit(tu);
    for(auto func: tu->func_decls)
      accept(func);
    for(auto func: tu->functions)
      accept(func);
    visitor.handle_post_translation_unit();
  }

  void accept(dcc::ast::function_declaration_ptr &fdecl){
    visitor.handle_pre_func_decl();
    visitor.visit(fdecl);
    accept(fdecl->signature);
    visitor.handle_post_func_decl();
  }

  void accept(dcc::ast::function_definition_ptr &func){
    visitor.handle_pre_func_def();
    visitor.visit(func);
    accept(func->signature);
    accept(func->statements);
    visitor.handle_post_func_def();
  }

  void accept(dcc::ast::prototype_ptr &proto){
    visitor.handle_pre_prototype();
    visitor.visit(proto);
    for(auto param : proto->param_list)
      accept(param);
    visitor.handle_post_prototype();
  }

  void accept(dcc::ast::parameter_ptr &param){
    visitor.handle_pre_parameter();
    visitor.visit(param);
    visitor.handle_post_parameter();
  }
  
  void accept(dcc::ast::function_statement_ptr &fstmt){
    visitor.handle_pre_func_stmt();
    visitor.visit(fstmt);
    for(auto stmt: fstmt->var_decls)
      accept(stmt);
    for(auto stmt: fstmt->statements)
      accept(stmt);
    visitor.handle_post_func_stmt();
  }
  
  void accept(dcc::ast::variable_declaration_ptr &vdecl){
    visitor.handle_pre_variable_decl();
    visitor.visit(vdecl);
    visitor.handle_post_variable_decl();
  }
  
  void accept(dcc::ast::binary_expression_ptr &expr){
    visitor.handle_pre_binary_expr();
    visitor.visit(expr);
    accept(expr->lhs);
    accept(expr->rhs);
    visitor.handle_post_binary_expr();
  }
  

  void accept(dcc::ast::call_expression_ptr &expr){
    visitor.handle_pre_call_expr();
    visitor.visit(expr);
    for(auto expr: expr->argument_list)
      accept(expr);
    visitor.handle_post_call_expr();
  }
  
  void accept(dcc::ast::jump_statement_ptr &jstmt){
    visitor.handle_pre_jump_stmt();
    visitor.visit(jstmt);
    accept(jstmt->ret_expr);
    visitor.handle_post_jump_stmt();
  }
  
  void accept(dcc::ast::identifier_ptr &id){
    visitor.handle_pre_identifier();
    visitor.visit(id);
    visitor.handle_post_identifier();
  }
  
  void accept(dcc::ast::number_literal_ptr & numlit){
    visitor.handle_pre_number();
    visitor.visit(numlit);
    visitor.handle_post_number();
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


  // traverse ast node
  template<typename T>
  void traverse(T & node){accept(node);}
private:
};


template<typename T>
void traverse(visitor &visitor, T && node){
  dcc::ast::helper::ast_accepter acceptor(visitor);
  acceptor.traverse(node);
  return;
}


} // end of namespace helper
} // end of namespace ast
} // end of namespace dcc

#endif
