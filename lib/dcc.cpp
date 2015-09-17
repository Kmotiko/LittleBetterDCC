#include "include/ast/ast.hpp"
#include "include/ast/visitor.hpp"
#include "include/parser/parser.hpp"
#include "include/semantic/semantic_analayzer.hpp"
#include "include/codegen/ir_generator.hpp"

#include <fstream>
#include <iostream>
#include <regex>

#include <boost/program_options.hpp>

#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/PassManager.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/FileSystem.h>


int main(int argc, char const* argv[])
{

  // option description
  boost::program_options::options_description opt("dcc options");
  opt.add_options()
    ("help,h","print help")
    ("output_file,o", boost::program_options::value<std::string>(), "<output file>")
    ("input_file", boost::program_options::value<std::string>(), "<input files>");

  // positional option description
  boost::program_options::positional_options_description pos_opt;
  pos_opt.add("input_file", -1);
  
  // parse input stream
  boost::program_options::variables_map vm;
  boost::program_options::store(
      boost::program_options::command_line_parser(argc, argv)
        .options(opt)
        .positional(pos_opt)
        .run()
        ,vm);
  boost::program_options::notify(vm);


  // TODO: export as driver 
  if (vm.count("input_file"))
  {
    // now, accept only one file.
    std::string name = vm["input_file"].as<std::string>();
    std::ifstream ifs(name);
    std::istreambuf_iterator<char> it(ifs);
    std::istreambuf_iterator<char> last;
    std::string code(it, last);

    // do parse
    dcc::ast::module mod = dcc::parser::parser::parse(code, name);

    // do semantic analysis
    dcc::semantic::scope::symbol_table scope = dcc::semantic::semantic_analayzer::analyze(mod);

    // generate code
    llvm::Module *module = dcc::codegen::ir_generator().generate(mod, scope);

    // output file
    std::string out_file;
    if(vm.count("output_file")){
      out_file = vm["output_file"].as<std::string>();
    }else{
      std::string format = "$1.ll";
      std::regex re("(\\w+)\\.dc");
      std::smatch result;
      if(std::regex_match(name, result, re))
        out_file = std::regex_replace(name, re, format);
      else
        out_file = name + ".ll";
    }

    // output ir
    std::error_code error;
    llvm::raw_fd_ostream raw_stream(out_file.c_str(), error,
                                    llvm::sys::fs::OpenFlags::F_RW);
    llvm::PassManager pm;
    pm.add(llvm::createPrintModulePass(raw_stream));
    pm.run(*module);
    raw_stream.close();
  }

  return 0;
}
