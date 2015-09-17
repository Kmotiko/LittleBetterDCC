#include "include/semantic/analyze_visitor.hpp"

namespace dcc {
namespace semantic {


// visit translation_unit
void scope_builder::visit(dcc::ast::translation_unit_ptr &tu) {
  // create global scope
  tu->scope = this->root;
  this->current_scope = tu->scope.lock();
}

// visit function declaration
void scope_builder::visit(dcc::ast::function_declaration_ptr &fdecl) {
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
    auto scope =
        boost::get<dcc::semantic::scope::global_scope_ptr>(this->current_scope);
    new_func->enclosing_scope = scope;

    // add new function to global scope
    scope->add_function(new_func);
  }
  return;
}

// visit function definition
void scope_builder::visit(dcc::ast::function_definition_ptr &fdef) {
  // get current scope as function_scope
  if (this->current_scope.type() ==
      typeid(dcc::semantic::scope::global_scope_ptr)) {
    // get current scope as function_scope
    auto scope =
        boost::get<dcc::semantic::scope::global_scope_ptr>(this->current_scope);

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
      auto f = [](auto node) {
        auto sig_node = node->signature;
        std::string param_str;
        for (auto param : sig_node->param_list)
          param_str += param->type_name + " " + param->param_name->value;
        return std::to_string(node->start_loc.line) + ":" +
               std::to_string(node->start_loc.col) + sig_node->type_name + " " +
               sig_node->func_name->value + "(" + param_str + ")";
      };
      auto wrapped_f = dcc::helper::make_wrapped_function<std::string>(f);
      std::string msg = boost::apply_visitor(wrapped_f, (*exist_func)->node);

      this->error_messages.push_back(
          "error: function " + (*exist_func)->name + " is already defined.\n" +
          "\t previous definition is here \n" + "\t" + msg);

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
void scope_builder::visit(dcc::ast::variable_declaration_ptr &vdecl) {
  // get current scope as function_scope
  auto scope = dcc::helper::get<dcc::semantic::scope::func_scope_ptr>(
      this->current_scope);
  if (!scope)
    return;

  // get type
  auto type =
      dcc::semantic::scope::global_scope::get_builtin_type(vdecl->type_name);
  // get vairable name
  auto var_name = vdecl->name->value;

  // create symbol
  auto new_symbol =
      std::make_shared<dcc::semantic::symbol::variable_symbol>(var_name, *type);
  vdecl->symbol = new_symbol;

  // push to function scope
  if (!(*scope)->add_variable(new_symbol))
    ; // TODO: make error message
  return;
}

// visit jump statement
void scope_builder::visit(dcc::ast::jump_statement_ptr &stmt) {
  // switch scope
  if (this->current_scope.type() ==
      typeid(dcc::semantic::scope::func_scope_ptr)) {
    this->current_scope = boost::get<dcc::semantic::scope::func_scope_ptr>(
                              this->current_scope)->enclosing_scope;
  } else {
    // TBD: error message ?
  }
}

// proccess after function definition
void scope_builder::handle_post_func_def() {
  // switch scope
  if (this->current_scope.type() ==
      typeid(dcc::semantic::scope::func_scope_ptr)) {
    this->current_scope = boost::get<dcc::semantic::scope::func_scope_ptr>(
                              this->current_scope)->enclosing_scope;
  } else {
    // TBD: error message ?
  }
}

// create new function symbol(scope)
dcc::semantic::symbol::func_symbol_ptr
scope_builder::create_function(std::string name, std::string type_name,
                               std::vector<dcc::ast::parameter_ptr> params,
                               bool is_declaration) {
  std::vector<dcc::semantic::symbol::variable_symbol_ptr> param_symbols;
  // iterate params
  for (auto param : params) {
    // get name and type of param;
    std::string param_name = param->param_name->value;
    dcc::semantic::type::builtin_type_ptr type =
        std::make_shared<dcc::semantic::type::builtin_type>(param->type_name);

    // create var_symbol
    auto var_symbol = std::make_shared<dcc::semantic::symbol::variable_symbol>(
        param_name, type);
    param->symbol = var_symbol;
    // push to vector
    param_symbols.push_back(var_symbol);
  }

  // create and return function
  auto type = dcc::semantic::scope::global_scope::get_builtin_type(type_name);
  return std::make_shared<dcc::semantic::symbol::func_symbol>(
      name, param_symbols, *type, is_declaration);
}

// visit function definition
void symbol_resolver::visit(dcc::ast::function_definition_ptr &fdef) {
  // switch scope
  this->current_scope = fdef->scope.lock();
}

// visit call expression
void symbol_resolver::visit(dcc::ast::call_expression_ptr &expr) {
  std::string name = expr->func_name->value;

  std::vector<dcc::semantic::type::one_of_type> param_types;
  for (auto expr : expr->argument_list) {
    // TODO: resolve type of expression.
    //("int" type is hard coded in current code...)
    auto type = dcc::semantic::scope::global_scope::get_builtin_type("int");
    // in DummyC, the type is only "int"
    param_types.push_back(*type);

    // symbol reference will be set to each argument when invoke visit(argument_list)
  }

  auto wrapper_f = dcc::helper::make_wrapped_function<
      boost::optional<dcc::semantic::scope::func_scope_ptr>>(
      [name, param_types](auto scope) {
        return scope->resolve_func(name, param_types);
      });
  auto func_symbol = boost::apply_visitor(wrapper_f, this->current_scope);
  if (func_symbol != boost::none) {
    expr->symbol = *func_symbol;
  } else {
    // make error message
    error_messages.push_back("function " + name + " is not declared.\n" +
                             "referenced at " +
                             std::to_string(expr->start_loc.line) + ":" +
                             std::to_string(expr->start_loc.col));
  }
  return;
}

void symbol_resolver::visit(dcc::ast::jump_statement_ptr &stmt) {
  // TODO: resolve return type and check it is same with function type if.
}


void symbol_resolver::visit(dcc::ast::identifier_ptr &identifier) {
  auto variable_symbol = this->resolve_variable(identifier->value);
  if (variable_symbol == boost::none) {
    // TODO: make more detail about binary expr
    error_messages.push_back("variable " + identifier->value +
                             " is not declared.\n" + "referenced at " +
                             std::to_string(identifier->start_loc.line) + ":" +
                             std::to_string(identifier->start_loc.col));
    // set symbol reference
  } else {
    identifier->symbol = *variable_symbol;
  }
}


}
}
