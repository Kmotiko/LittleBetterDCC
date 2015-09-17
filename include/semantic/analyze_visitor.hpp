#ifndef DCC_SEMANTIC_VISITOR_HPP
#define DCC_SEMANTIC_VISITOR_HPP

#include <exception>
#include <iostream>
#include <boost/optional.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "include/ast/ast.hpp"
#include "include/ast/ast_fwd.hpp"
#include "include/helper.hpp"
#include "include/semantic/scope.hpp"
#include "include/semantic/scope_fwd.hpp"
#include "include/semantic/symbol.hpp"
#include "include/ast/visitor.hpp"

namespace dcc {
namespace semantic {

// TODO: implement detail of exception
class semantic_exception : public std::exception{};


///
struct scope_builder : public dcc::ast::helper::visitor {
public:
  explicit scope_builder(dcc::semantic::scope::global_scope_ptr const &root) noexcept : root(root) {
    // create global scope
    this->current_scope = this->root;
  }
  ~scope_builder(){}


  virtual void visit(dcc::ast::translation_unit_ptr &tu) override;
  virtual void visit(dcc::ast::function_declaration_ptr &fdecl) override;
  virtual void visit(dcc::ast::function_definition_ptr &fdef) override;
  virtual void visit(dcc::ast::variable_declaration_ptr &vdecl) override;
  virtual void visit(dcc::ast::jump_statement_ptr &stmt) override;
  virtual void handle_post_func_def() override;

  // count errs
  uint error_counts(){
    return this->error_messages.size();
  }

  // dump erro messages
  void dump_errors(){
    for(auto message: this->error_messages)
      std::cout << message << std::endl;
  }
private:
  //
  // create new function symbol(scope)
  dcc::semantic::symbol::func_symbol_ptr
  create_function(std::string name, std::string type_name,
                  std::vector<dcc::ast::parameter_ptr> params,
                  bool is_declaration = false);

  // variable
  dcc::semantic::scope::one_of_scope current_scope;
  dcc::semantic::scope::global_scope_ptr root;
  std::vector<std::string> error_messages;
};



// symbol resolver class
// This class run after building scope
struct symbol_resolver : public dcc::ast::helper::visitor {
public:
  explicit symbol_resolver(dcc::semantic::scope::global_scope_ptr root) noexcept : current_scope(root) {}


  virtual void visit(dcc::ast::function_definition_ptr &fdef) override;
  virtual void visit(dcc::ast::call_expression_ptr &expr) override;
  virtual void visit(dcc::ast::jump_statement_ptr &stmt) override;
  virtual void visit(dcc::ast::identifier_ptr &identifier) override;

  // get error count
  uint error_counts(){
    return this->error_messages.size();
  }


  // dump all of error messages
  void dump_errors(){
    for(auto message: this->error_messages)
      std::cout << message << std::endl;
  }


private:
  // variable resolver
  boost::optional<dcc::semantic::symbol::variable_symbol_ptr>
  resolve_variable(std::string name) {
    auto wrapper_f = dcc::helper::make_wrapped_function<
        boost::optional<dcc::semantic::symbol::variable_symbol_ptr>>(
        [name](auto scope) { return scope->resolve_variable(name); });
    return boost::apply_visitor(wrapper_f, this->current_scope);
  }

  // variable
  dcc::semantic::scope::one_of_scope current_scope;
  std::vector<std::string> error_messages;
};

}
}

#endif
