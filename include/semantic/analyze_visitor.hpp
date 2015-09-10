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


  // visit translation_unit
  virtual void visit(dcc::ast::translation_unit_ptr &tu) override {
    // create global scope
    tu->scope = this->root; 
    this->current_scope = tu->scope.lock();
  }


  // visit function declaration
  virtual void visit(dcc::ast::function_declaration_ptr &fdecl) override {
    // get func_name
    auto name = fdecl->signature->func_name->value;

    // create function
    auto new_func = this->create_function(name, fdecl->signature->type_name,
                                          fdecl->signature->param_list, true);

    // TODO: check dupulication ( may be, this is resolving step work...)

    fdecl->scope = new_func;
    new_func->node = fdecl;
    // define function
    if (this->current_scope.type() ==
        typeid(dcc::semantic::scope::global_scope_ptr)) {
      // get current scope as function_scope
      auto scope = boost::get<dcc::semantic::scope::global_scope_ptr>(
          this->current_scope);
      new_func->enclosing_scope = scope;

      // add new function to global scope
      scope->add_function(new_func);
    }
    return;
  }


  // visit function definition
  virtual void visit(dcc::ast::function_definition_ptr &fdef) override {
    // get current scope as function_scope
    if (this->current_scope.type() ==
        typeid(dcc::semantic::scope::global_scope_ptr)) {
      // get current scope as function_scope
      auto scope = boost::get<dcc::semantic::scope::global_scope_ptr>(
          this->current_scope);

      // resolve function symbol
      boost::optional<dcc::semantic::scope::func_scope_ptr> exist_func =
          scope->resolve_func(fdef->signature->func_name->value);

      // check duplication
      // already exists? and is not declaration?
      // maybe this is resolving step work...
      if (exist_func == boost::none) {
        // define function
        auto name = fdef->signature->func_name->value;
        auto new_func = this->create_function(name, fdef->signature->type_name,
                                              fdef->signature->param_list);
        new_func->enclosing_scope = scope;
        new_func->node = fdef;
        fdef->scope = new_func;
        scope->add_function(new_func);

        // switch current scope
        this->current_scope = fdef->scope.lock();
        return;
      }

      // function is already defined
      if (!(*exist_func)->is_declaration) {
        // create error message
        auto f = [](auto node){
          auto sig_node = node->signature;
          std::string param_str;
          for(auto param : sig_node->param_list)
            param_str += param->type_name + " " + param->param_name->value;
          return std::to_string(node->start_loc.line) + ":" + std::to_string(node->start_loc.col) 
                  + sig_node->type_name + " " +sig_node->func_name->value + "(" + param_str + ")";
        };
        auto wrapped_f = dcc::helper::make_wrapped_function<std::string>(f);
        std::string msg = boost::apply_visitor(wrapped_f, (*exist_func)->node);

        this->error_messages.push_back(
            "error: function " + (*exist_func)->name + " is already defined.\n"
            + "\t previous definition is here \n"
            + "\t" + msg);

      // set function symbol to the ast
      // and mark as function definition.
      } else {
        fdef->scope = (*exist_func);
        (*exist_func)->is_declaration = false;
        this->current_scope = fdef->scope.lock();
      }
    }
    return;
  }

  // visit variable declaration
  virtual void visit(dcc::ast::variable_declaration_ptr &vdecl) override {
    // get current scope as function_scope
    auto scope = dcc::helper::get<dcc::semantic::scope::func_scope_ptr>(this->current_scope);
    if(!scope)
      return;

    // get type
    auto type =
        dcc::semantic::scope::global_scope::get_builtin_type(vdecl->type_name);
    // get vairable name
    auto var_name = vdecl->name->value;

    // create symbol
    auto new_symbol = std::make_shared<dcc::semantic::symbol::variable_symbol>(
        var_name, *type);
    vdecl->symbol = new_symbol;

    // push to function scope
    if(!(*scope)->add_variable(new_symbol))
      ; // TODO: make error message
    return;
  }


  // visit jump statement
  virtual void visit(dcc::ast::jump_statement_ptr &stmt) override {
    // switch scope
    if (this->current_scope.type() ==
        typeid(dcc::semantic::scope::func_scope_ptr)) {
      this->current_scope = boost::get<dcc::semantic::scope::func_scope_ptr>(
                                this->current_scope)->enclosing_scope;
    }else{
      // TBD: error message ?
    }
  }


  // proccess after function definition
  virtual void handle_post_func_def() override{
    // switch scope
    if (this->current_scope.type() ==
        typeid(dcc::semantic::scope::func_scope_ptr)) {
      this->current_scope = boost::get<dcc::semantic::scope::func_scope_ptr>(
                                this->current_scope)->enclosing_scope;
    }else{
      // TBD: error message ?
    }
  }


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
                  bool is_declaration = false) {
    std::vector<dcc::semantic::symbol::variable_symbol_ptr> param_symbols;
    // iterate params
    for (auto param : params) {
      // get name and type of param;
      std::string param_name = param->param_name->value;
      dcc::semantic::type::builtin_type_ptr type_name =
          std::make_shared<dcc::semantic::type::builtin_type>(param->type_name);

      // create var_symbol
      auto var_symbol =
          std::make_shared<dcc::semantic::symbol::variable_symbol>(param_name,
                                                                   type_name);
      param->symbol = var_symbol;
      // push to vector
      param_symbols.push_back(var_symbol);
    }

    // create and return function
    auto type = dcc::semantic::scope::global_scope::get_builtin_type(type_name);
    return std::make_shared<dcc::semantic::symbol::func_symbol>(
        name, param_symbols, *type, is_declaration);
  }

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


  // visit function definition
  virtual void visit(dcc::ast::function_definition_ptr &fdef) override {
    // switch scope
    this->current_scope = fdef->scope.lock();
  }


  // visit binary expression
  virtual void visit(dcc::ast::binary_expression_ptr &expr) override {
    // get lhs
    auto lhs = expr->lhs;

    // if lhs is variable, resolve symbol
    if (lhs.type() == typeid(dcc::ast::identifier_ptr)) {
      auto var_node = dcc::helper::get<dcc::ast::identifier_ptr>(lhs);

      // resolve
      auto variable_symbol = this->resolve_variable((*var_node)->value);
      if (variable_symbol == boost::none){
        // TODO: make more detail about binary expr
        error_messages.push_back(
            "variable " + (*var_node)->value + " is not declared.\n"
            + "referenced at " + std::to_string((*var_node)->start_loc.line) + ":" 
            + std::to_string((*var_node)->start_loc.col) );
      // set symbol reference
      }else{
        (*var_node)->symbol = *variable_symbol;
      }
    }


    // get rhs
    auto rhs = expr->rhs;
    // if rhs is variable, resolve symbol
    if (rhs.type() == typeid(dcc::ast::identifier_ptr)) {
      auto var_node = dcc::helper::get<dcc::ast::identifier_ptr>(rhs);
      // resolve
      auto variable_symbol = this->resolve_variable((*var_node)->value);
      if (variable_symbol == boost::none){
        // TODO: make more detail about binary expr
        error_messages.push_back(
            "variable " + (*var_node)->value + " is not declared.\n"
            + "referenced at " + std::to_string((*var_node)->start_loc.line) + ":" 
            + std::to_string((*var_node)->start_loc.col) );

      // set symbol reference
      }else{
        (*var_node)->symbol = *variable_symbol;
      }
    }
  }


  // visit call expression
  virtual void visit(dcc::ast::call_expression_ptr &expr) override {
    std::string name = expr->func_name->value;

    std::vector<dcc::semantic::type::one_of_type> param_types;
    for(auto expr : expr->argument_list){
      // TODO: resolve type of expression.("int" type is hard coded in current code...)
      auto type = dcc::semantic::scope::global_scope::get_builtin_type("int");

      // in DummyC, the type is only "int"
      param_types.push_back(*type);
    }

    auto wrapper_f = dcc::helper::make_wrapped_function<
        boost::optional<dcc::semantic::scope::func_scope_ptr>>(
        [name, param_types](auto scope) { return scope->resolve_func(name, param_types); });
    auto func_symbol = boost::apply_visitor(wrapper_f, this->current_scope);
    if (func_symbol != boost::none) {
      expr->symbol = *func_symbol;
    } else {
      // make error message
      error_messages.push_back(
            "function " + name + " is not declared.\n"
            + "referenced at " + std::to_string(expr->start_loc.line) 
            + ":" + std::to_string(expr->start_loc.col) );
    }
    return;
  }


  virtual void visit(dcc::ast::jump_statement_ptr &stmt) override {
    auto expr = stmt->ret_expr;
    auto var_node = dcc::helper::get<dcc::ast::identifier_ptr>(expr);
    if(!var_node)
      return;

    auto variable_symbol = this->resolve_variable((*var_node)->value);
    if(variable_symbol == boost::none){
      // make error message
      error_messages.push_back(
          "variable " + (*var_node)->value + " is not declared.\n"
          + "referenced at " + std::to_string((*var_node)->start_loc.line) + ":" 
          + std::to_string((*var_node)->start_loc.col) );
    }
  }


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
