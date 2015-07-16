#ifndef DCC_SEMANTIC_HPP
#define DCC_SEMANTIC_HPP

#include <iostream>

#include <boost/variant/apply_visitor.hpp>

#include "include/ast/ast.hpp"
#include "include/ast/ast_fwd.hpp"
#include "include/ast/visitor.hpp"


namespace dcc {
namespace semantic {


struct semantic_visitor : public dcc::ast::helper::visitor{
public:

  virtual void visit(dcc::ast::translation_unit_ptr &tu) override {
    // 
  }

  virtual void visit(dcc::ast::function_declaration_ptr &fdecl) override {
    // 

    // 
  }


  virtual void visit(dcc::ast::function_definition_ptr &fdef) override{
    // 
  }


  virtual void visit(dcc::ast::prototype_ptr &proto) override {
    //
  }


  virtual void visit(dcc::ast::parameter_ptr &param) override {
    //
  }


  virtual void visit(dcc::ast::function_statement_ptr &stmt) override {
    // 
  }


  virtual void visit(dcc::ast::variable_declaration_ptr &vdecl) override {
    //
  }


  virtual void visit(dcc::ast::binary_expression_ptr &expr) override {
    //
  }


  virtual void visit(dcc::ast::call_expression_ptr &expr) override {
    //
  }


  virtual void visit(dcc::ast::jump_statement_ptr &stmt) override {
    //
  }


  virtual void visit(dcc::ast::identifier_ptr &id) override {
    //
  }


  virtual void visit(dcc::ast::number_literal_ptr &num) override {
    //
  }
};


class semantic_analayzer {
public:
  static bool analyze(dcc::ast::module &mod);
private:
};


}
}

#endif
