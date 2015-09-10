#ifndef DCC_SEMANTIC_HPP
#define DCC_SEMANTIC_HPP

#include <iostream>
#include <boost/variant/apply_visitor.hpp>

#include "include/ast/ast.hpp"
#include "include/semantic/analyze_visitor.hpp"
#include "include/semantic/scope.hpp"

namespace dcc {
namespace semantic {
// main analayzer
class semantic_analayzer {
public:
  static dcc::semantic::scope::symbol_table analyze(dcc::ast::module &mod);
private:
};
}
}

#endif
