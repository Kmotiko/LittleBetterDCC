#include "include/semantic/scope.hpp"
#include "include/semantic/symbol.hpp"


namespace dcc{
namespace semantic{
namespace scope{


// builtin_types
// now, builtin_type is only 'int'...
// other type is future work.
const std::vector<dcc::semantic::type::builtin_type_ptr> 
global_scope::builtin_types = {
  std::make_shared<dcc::semantic::type::builtin_type>("int"),
};

// resolve function with only it's name
boost::optional<func_scope_ptr> global_scope::resolve_func(std::string name, std::vector<dcc::semantic::type::one_of_type> params){
  for(auto func : func_table){
    // now, only check name and param size
    if(func->name == name && params.size() == func->params.size()){
      return boost::optional<func_scope_ptr>(func);
    }
  }
  return boost::none;
}

// resolve function with only it's name
boost::optional<func_scope_ptr> global_scope::resolve_func(std::string name) {
  for(auto func : func_table){
    if(func->name == name){
      return boost::optional<func_scope_ptr>(func);
    }
  }
  return boost::none;
}

// get builtin_type specified with name
boost::optional<dcc::semantic::type::builtin_type_ptr> 
global_scope::get_builtin_type(std::string name){
  for(auto type : builtin_types){
    if(type->name == name)
      return type;
  }
  return boost::none;
}

}
}
}
