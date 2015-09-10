#ifndef DCC_SYMBOL_FWD_HPP
#define DCC_SYMBOL_FWD_HPP

namespace dcc{
namespace semantic{
namespace symbol{

struct symbol;
struct variable_symbol;
struct func_symbol;

// shared_ptr
using variable_symbol_ptr = std::shared_ptr<variable_symbol>;
using func_symbol_ptr = std::shared_ptr<func_symbol>;

// weak_ptr
using variable_symbol_wptr = std::weak_ptr<variable_symbol>;
using func_symbol_wptr = std::weak_ptr<func_symbol>;


} // end of namespace symbol
} // end of namespace ast
} // end of namespace dcc

#endif


