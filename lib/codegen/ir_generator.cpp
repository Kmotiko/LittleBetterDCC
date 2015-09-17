#include "include/codegen/ir_generator.hpp"

namespace dcc {
namespace codegen {


void ir_generator::generate(dcc::ast::translation_unit_ptr &tunit) {
  // nothing to do ?
  for (auto func_decl : tunit->func_decls) {
    generate(func_decl);
  }

  for (auto func_def : tunit->functions) {
    generate(func_def);
  }

  return;
}

void ir_generator::generate(dcc::ast::function_declaration_ptr &fdecl) {
  // create function
  llvm::Function *func = create_function(fdecl->signature, fdecl->scope.lock());

  // add new function to function table
  func_map[fdecl->signature->func_name->value] = func;

  return;
}

void ir_generator::generate(dcc::ast::function_definition_ptr &fdef) {
  llvm::Function *func = nullptr;
  // search from function table
  auto func_it = func_map.find(fdef->signature->func_name->value);
  if (func_it == func_map.end()) {
    func = create_function(fdef->signature, fdef->scope.lock());
    func_map[fdef->signature->func_name->value] = func;
  } else {
    func = func_it->second;
  }

  // generate function statement
  generate(fdef->statements, fdef->signature->param_list, func);

  return;
}

// create alloca for function parameter and store value to it.
llvm::Value *ir_generator::generate(dcc::ast::parameter_ptr &param) {
  llvm::Type *type = create_type(param->symbol.lock()->type);
  llvm::Value *val =
      builder.CreateAlloca(type,                            /* type */
                           nullptr,                         /* ArraySize */
                           "_" + param->symbol.lock()->name /* name */
                           );

  // TODO: check whether param is registerd or not
  auto it = value_table.find(param->symbol.lock()->name);
  llvm::Value *param_val = nullptr;
  if (it != value_table.end())
    param_val = it->second;
  else
    throw codegen_exception();
  builder.CreateStore(param_val, /* value */
                      val       /* value */
                      );

  // TODO: add val to value table
  value_table[param->symbol.lock()->name] = val;
  return val;
}

void ir_generator::generate(dcc::ast::function_statement_ptr &func_stmt,
              std::vector<dcc::ast::parameter_ptr> &param_list,
              llvm::Function *function) {
  // create basic block
  llvm::BasicBlock *bb = llvm::BasicBlock::Create(context, "entry", function);

  // set inseret point
  builder.SetInsertPoint(bb);

  // parameter
  for (auto param : param_list) {
    generate(param);
  }

  // variable decl
  for (auto var_decl : func_stmt->var_decls) {
    generate(var_decl);
  }

  // function statement
  for (auto stmt : func_stmt->statements) {
    //
    generate(stmt);
  }

  return;
}


void ir_generator::generate(dcc::ast::variable_declaration_ptr &var_decl) {
  // create type
  llvm::Type *type = create_type(var_decl->symbol.lock()->type);

  // create alloca inst
  llvm::AllocaInst *alloca_inst =
      builder.CreateAlloca(type,                         /* type */
                           nullptr,                      /* ArraySize */
                           var_decl->symbol.lock()->name /* name */
                           );
  value_table[var_decl->symbol.lock()->name] = alloca_inst;
  return;
}

llvm::Value *ir_generator::generate(dcc::ast::binary_expression_ptr &bin_expr) {
  // create rhs

  if (bin_expr->operand == "=") {
    // create lhs
    auto lhs = dcc::helper::get<dcc::ast::identifier_ptr>(bin_expr->lhs);
    if (!lhs)
      throw codegen_exception();
    llvm::Value *lhs_value = value_table.find((*lhs)->value)->second;
    llvm::Value *rhs_value = generate(bin_expr->rhs);
    builder.CreateStore(rhs_value, lhs_value);
    return lhs_value;
  }

  // create rhs
  llvm::Value *lhs_value = generate(bin_expr->lhs);
  llvm::Value *rhs_value = generate(bin_expr->rhs);
  llvm::Value *expr_value = nullptr;
  // if operator is +
  if (bin_expr->operand == "+") {
    expr_value = builder.CreateAdd(lhs_value, rhs_value, "add_val");
    // if operator is -
  } else if (bin_expr->operand == "-") {
    expr_value = builder.CreateSub(lhs_value, rhs_value, "sub_val");
    // if operator is *
  } else if (bin_expr->operand == "*") {
    expr_value = builder.CreateMul(lhs_value, rhs_value, "mul_val");
    // if operator is /
  } else if (bin_expr->operand == "/") {
    expr_value = builder.CreateSDiv(lhs_value, rhs_value, "div_val");
  }
  return expr_value;
}

llvm::Value *ir_generator::generate(dcc::ast::call_expression_ptr &call_expr) {
  std::vector<llvm::Value *> arg_vec;
  for (auto arg : call_expr->argument_list) {
    arg_vec.push_back(generate(arg));
  }

  llvm::Function *func = nullptr;
  auto it = func_map.find(call_expr->symbol.lock()->name);
  if (it != func_map.end())
    func = it->second;

  // create call inst
  return builder.CreateCall(func,        /* function */
                            arg_vec,     /* arg */
                            "called_val" /* name */
                            );
}

void ir_generator::generate(dcc::ast::jump_statement_ptr &stmt) {
  auto f = [this](auto node) { return this->generate(node); };
  auto wrapped_f = dcc::helper::make_wrapped_function<llvm::Value *>(f);
  llvm::Value *val = boost::apply_visitor(wrapped_f, stmt->ret_expr);
  builder.CreateRet(val);
  return;
}

llvm::Value *ir_generator::generate(dcc::ast::identifier_ptr &var) {
  // create load inst for identifier
  llvm::Value *val = nullptr;
  auto it = value_table.find(var->symbol.lock()->name);
  if (it != value_table.end())
    val = it->second;
  llvm::Value *load_val = builder.CreateLoad(val,       /* value */
                                             var->value /* name */
                                             );
  return load_val;
}

llvm::Value *ir_generator::generate(dcc::ast::number_literal_ptr &num) {
  // create constant int
  return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), num->value);
}

void ir_generator::generate(dcc::ast::one_of_statement node) {
  auto f = [this](auto node) {
    this->generate(node);
    return;
  };
  auto wrapped_f = dcc::helper::make_wrapped_function<void>(f);
  boost::apply_visitor(wrapped_f, node);
}

llvm::Value *ir_generator::generate(dcc::ast::one_of_expr node) {
  auto f = [this](auto node) { return this->generate(node); };
  auto wrapped_f = dcc::helper::make_wrapped_function<llvm::Value *>(f);
  return boost::apply_visitor(wrapped_f, node);
}

llvm::Function *ir_generator::create_function(dcc::ast::prototype_ptr signature,
                                dcc::semantic::symbol::func_symbol_ptr symbol) {
  // create function type
  std::vector<llvm::Type *> arg_types;

  // push argument type
  for (auto param : symbol->params) {
    arg_types.push_back(create_type(param->type));
  }

  // create FunctionType
  llvm::FunctionType *func_type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(context), arg_types, false);

  // create function
  llvm::Function *func = llvm::Function::Create(
      func_type, llvm::Function::ExternalLinkage, symbol->name, module);

  // set parameter name
  auto arg_it = func->arg_begin();
  auto param_it = std::begin(symbol->params);
  auto param_end = std::end(symbol->params);
  for (; param_it != param_end; ++arg_it, ++param_it) {
    // set name
    arg_it->setName((*param_it)->name);
    value_table[(*param_it)->name] = &(*arg_it);
  }
  return func;
}

llvm::Type *ir_generator::create_type(dcc::semantic::type::one_of_type type) {
  auto f = [this](auto type) {
    llvm::Type *ret_type = nullptr;
    if (type->name == "int")
      ret_type = llvm::Type::getInt32Ty(this->context);
    else
      // throw exception
      throw codegen_exception();

    return ret_type;
  };
  auto wrapped_f = dcc::helper::make_wrapped_function<llvm::Type *>(f);
  return boost::apply_visitor(wrapped_f, type);
}


}
}
