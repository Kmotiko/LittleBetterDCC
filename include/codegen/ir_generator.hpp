#ifndef DCC_IRGEN_HPP
#define DCC_IRGEN_HPP

#include <algorithm>
#include <exception>
#include <map>
#include <vector>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>

#include <boost/variant/apply_visitor.hpp>

#include "include/ast/ast_fwd.hpp"
#include "include/ast/visitor.hpp"
#include "include/helper.hpp"
#include "include/semantic/symbol.hpp"

namespace dcc {
namespace codegen {

// TODO: implement detail of exception
class codegen_exception : public std::exception{};

class costum_acceptor : public dcc::ast::helper::ast_accepter{
  void accept(dcc::ast::binary_expression_ptr &expr) {
  }
};


class ir_generator{
public:

  // ctor
  explicit ir_generator() : 
  triple(llvm::sys::getDefaultTargetTriple()),
  context(llvm::getGlobalContext()),
  builder(context)
  {
    llvm::InitializeNativeTarget();
    std::string error;
    target = llvm::TargetRegistry::lookupTarget(triple.getTriple(), error);
    llvm::TargetOptions options;
    target_machine = target->createTargetMachine(triple.getTriple(), llvm::sys::getHostCPUName(), "", options);

    // getDataLayout is deleted in LLVM 3.6 ?
#if(LLVM_MAJOR_VERSION == 3 && LLVM_MINOR_VERSION != 6)
    data_layout = target_machine->getDataLayout();
#endif
  }


  llvm::Module* generate(dcc::ast::module &mod, dcc::semantic::scope::symbol_table &scope) {
    // create module
    module = new llvm::Module(mod.name ,context);
    module->setTargetTriple(triple.getTriple());
    generate(mod.tunit);
    return module;
  }

private:
  void generate(dcc::ast::translation_unit_ptr &tunit);
  void generate(dcc::ast::function_declaration_ptr &fdecl);
  void generate(dcc::ast::function_definition_ptr &fdef);
  llvm::Value *generate(dcc::ast::parameter_ptr &param);
  void generate(dcc::ast::function_statement_ptr &func_stmt,
              std::vector<dcc::ast::parameter_ptr> &param_list,
              llvm::Function *function);
  void generate(dcc::ast::variable_declaration_ptr &var_decl);
  llvm::Value *generate(dcc::ast::binary_expression_ptr &bin_expr);
  llvm::Value *generate(dcc::ast::call_expression_ptr &call_expr);
  void generate(dcc::ast::jump_statement_ptr &stmt);
  llvm::Value *generate(dcc::ast::identifier_ptr &var);
  llvm::Value *generate(dcc::ast::number_literal_ptr &num);
  void generate(dcc::ast::one_of_statement node);
  llvm::Value *generate(dcc::ast::one_of_expr node);
  llvm::Function *create_function(dcc::ast::prototype_ptr signature, dcc::semantic::symbol::func_symbol_ptr symbol);
  llvm::Type *create_type(dcc::semantic::type::one_of_type type);


  llvm::Triple triple;
  llvm::LLVMContext &context;
  llvm::IRBuilder<> builder;
  llvm::Target const *target;
  llvm::TargetMachine *target_machine;
#if(LLVM_MAJOR_VERSION == 3 && LLVM_MINOR_VERSION != 6)
  llvm::DataLayout const *data_layout;
#endif

  llvm::Module *module;
  std::map<std::string, llvm::Function*> func_map;
  std::map<std::string, llvm::Value*> value_table;
};
}
}

#endif
