#include "include/semantic/symbol.hpp"

namespace dcc {
namespace semantic {
namespace symbol {

// resolve function
boost::optional<dcc::semantic::scope::func_scope_ptr>
func_symbol::resolve_func(std::string name) {
  auto f = [name](auto scope) { return scope->resolve_func(name); };
  auto wrapper_f = dcc::helper::make_wrapped_function<
      boost::optional<dcc::semantic::scope::func_scope_ptr>>(f);
  return boost::apply_visitor(wrapper_f, this->enclosing_scope);
}


// resolve function
boost::optional<dcc::semantic::scope::func_scope_ptr>
func_symbol::resolve_func(std::string name,
             std::vector<dcc::semantic::type::one_of_type> params) {
  auto f =
      [name, params](auto scope) { return scope->resolve_func(name, params); };
  auto wrapper_f = dcc::helper::make_wrapped_function<
      boost::optional<dcc::semantic::scope::func_scope_ptr>>(f);
  return boost::apply_visitor(wrapper_f, this->enclosing_scope);
}


// resolve variable
boost::optional<variable_symbol_ptr>
func_symbol::resolve_variable(std::string name) {
  // search local
  auto var_sym = var_table.find(name);
  if (var_sym != var_table.end()) {
    // return boost::optional<variable_symbol_ptr>(var_sym->second);
    return var_sym->second;
  }

  // search from parameters
  for (auto sym : params)
    if (sym->name == name)
      return sym;

  // search parents
  auto f = [name](auto scope) { return scope->resolve_variable(name); };
  auto wrapper_f = dcc::helper::make_wrapped_function<
      boost::optional<dcc::semantic::symbol::variable_symbol_ptr>>(f);
  return boost::apply_visitor(wrapper_f, this->enclosing_scope);
}


}
}
}
