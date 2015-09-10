#include <boost/variant/apply_visitor.hpp>

#include "include/semantic/semantic_analayzer.hpp"

namespace dcc {
namespace semantic {

// analayze semantic information...
// In current implement, analayze phase is separated to 2 steps.
// One is building phase which create scoep tree.
// And the other one is resolving phase.
// DCC syntax not need multi step analayzer, 
// but I implement it because for future improvement, 
// for studying how to implement it.
dcc::semantic::scope::symbol_table semantic_analayzer::analyze(dcc::ast::module &mod){

  // create symbol table
  auto root = std::make_shared<dcc::semantic::scope::global_scope>();
  scope_builder builder(root);
  dcc::ast::helper::traverse(builder, mod);
  if(builder.error_counts() > 0){
    builder.dump_errors();
    // throw exception
    throw semantic_exception();
  }

  // resolve symbol
  symbol_resolver resolver(root);
  dcc::ast::helper::traverse(resolver, mod);
  if(resolver.error_counts() > 0){
    resolver.dump_errors();
    // throw exception
    throw semantic_exception();
  }

  return dcc::semantic::scope::symbol_table(root);
}

}
}
