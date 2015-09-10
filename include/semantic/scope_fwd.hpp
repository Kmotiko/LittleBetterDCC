#ifndef DCC_SCOPE_FWD_HPP
#define DCC_SCOPE_FWD_HPP

#include <memory>
#include<boost/variant/variant.hpp>
#include<include/semantic/symbol_fwd.hpp>


namespace dcc{
namespace semantic{
namespace scope{

struct base_scope;
struct global_scope;

using global_scope_ptr  = std::shared_ptr<global_scope>;
using func_scope_ptr   = dcc::semantic::symbol::func_symbol_ptr;

using global_scope_wptr  = std::weak_ptr<global_scope>;
using func_scope_wptr    = dcc::semantic::symbol::func_symbol_wptr;

using one_of_scope =
    boost::variant<global_scope_ptr, func_scope_ptr>;

} // end of namespace scope
} // end of namespace ast
} // end of namespace dcc


#endif
