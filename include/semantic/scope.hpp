#ifndef DCC_SCOPE_HPP
#define DCC_SCOPE_HPP

#include<map>
#include<vector>
#include<boost/optional.hpp>

#include "include/ast/ast_fwd.hpp"
#include "include/semantic/type.hpp"
#include "include/semantic/scope_fwd.hpp"



namespace dcc{
namespace semantic{
namespace scope{

// basic scope class
struct base_scope{
public:
  explicit base_scope() noexcept {}
  virtual ~base_scope(){}

  // resolver
  virtual boost::optional<func_scope_ptr> resolve_func(std::string name){return boost::none;}
  virtual boost::optional<func_scope_ptr> resolve_func(std::string name, std::vector<dcc::semantic::type::one_of_type> params){return boost::none;}
  virtual boost::optional<symbol::variable_symbol_ptr> resolve_variable(std::string name){return boost::none;}

  // enclosing scope
  one_of_scope enclosing_scope;
};


// global scope
struct global_scope : public base_scope{
public:
  explicit global_scope() noexcept{}

  virtual boost::optional<func_scope_ptr> resolve_func(std::string name);
  virtual boost::optional<func_scope_ptr> resolve_func(std::string name, std::vector<dcc::semantic::type::one_of_type> params);
  virtual boost::optional<symbol::variable_symbol_ptr> resolve_variable(std::string name){return boost::none;}

  // add function to global scope
  void add_function(func_scope_ptr const &new_func){
    // TODO: duplication check
    func_table.push_back(new_func);
  }

  // get builtin type specified by type name
  static boost::optional<dcc::semantic::type::builtin_type_ptr> 
    get_builtin_type(std::string name);


  // table
  std::vector<func_scope_ptr> func_table;
  dcc::ast::translation_unit_wptr ast;
private:
  // builtin types
  static const std::vector<dcc::semantic::type::builtin_type_ptr> builtin_types;
};


// symbol table
struct symbol_table{
public:
  explicit symbol_table(global_scope_ptr root) noexcept : root(root) {};
  global_scope_ptr root;
};



} // end of scope
} // end of semantic
} // end of dcc


#endif
