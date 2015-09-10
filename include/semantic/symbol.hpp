#ifndef DCC_SYMBOL_HPP
#define DCC_SYMBOL_HPP

#include<include/ast/ast.hpp>
#include<include/helper.hpp>
#include<include/semantic/type.hpp>
#include<include/semantic/scope.hpp>

namespace dcc{
namespace semantic{
namespace symbol{


// basic symbol
struct symbol{
public:
  explicit symbol(
      std::string name) noexcept : name(name) {}

  explicit symbol(
      std::string name, 
      dcc::semantic::type::one_of_type type) noexcept :
    name(name), type(type) {}
  virtual ~symbol(){}

  std::string name;
  dcc::semantic::type::one_of_type type;
private:
};


// variable symbol
struct variable_symbol : public symbol{
public:
  explicit variable_symbol(
      std::string name, 
      dcc::semantic::type::one_of_type type) noexcept : 
    symbol(name, type){}

  dcc::ast::identifier_ptr variable;
private:
};


// function is symbol but also scope.
struct func_symbol : public symbol, dcc::semantic::scope::base_scope{
public:
  // ctor
  explicit func_symbol(
      std::string name,
      std::vector<dcc::semantic::symbol::variable_symbol_ptr> const &params,
      dcc::semantic::type::one_of_type type,
      bool is_declaration = false) noexcept : 
      symbol(name, type), dcc::semantic::scope::base_scope(), 
      params(params), is_declaration(is_declaration){}
  // dtor
  ~func_symbol(){}


  virtual boost::optional<dcc::semantic::scope::func_scope_ptr> resolve_func (std::string name) override;
  virtual boost::optional<dcc::semantic::scope::func_scope_ptr> 
    resolve_func(std::string name, std::vector<dcc::semantic::type::one_of_type> params) override;
  virtual boost::optional<variable_symbol_ptr> resolve_variable(std::string name) override;

  // define variable
  bool add_variable(variable_symbol_ptr new_symbol){
    // TODO: check dupulication
    var_table[new_symbol->name] = new_symbol;
    return true;
  }


  std::vector<variable_symbol_ptr> params;
  std::map<std::string, variable_symbol_ptr> var_table;
  bool is_declaration;

  boost::variant<
   dcc::ast::function_declaration_ptr,
   dcc::ast::function_definition_ptr> node;
  
private:
};


} // end of namespace symbol
} // end of namespace ast
} // end of namespace dcc

#endif

