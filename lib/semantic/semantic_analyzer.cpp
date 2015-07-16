#include "include/semantic/semantic_analayzer.hpp"

#include <boost/variant/apply_visitor.hpp>

namespace dcc {
namespace semantic {

bool semantic_analayzer::analyze(dcc::ast::module &mod){
  semantic_visitor visitor;
  dcc::ast::helper::accepter<semantic_visitor> accepter(visitor);
  accepter.accept(mod.tunit);
  return true;
}

}
}
