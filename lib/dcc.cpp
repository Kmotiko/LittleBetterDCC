#include "include/ast/ast.hpp"
#include "include/ast/visitor.hpp"
#include "include/parser/parser.hpp"
#include "include/semantic/semantic_analayzer.hpp"

#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>


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


  // do parse
  if (vm.count("input_file"))
  {
    // now, accept only one file.
    std::string name = vm["input_file"].as<std::string>();
    std::ifstream ifs(name);
    std::istreambuf_iterator<char> it(ifs);
    std::istreambuf_iterator<char> last;
    std::string code(it, last);
    dcc::ast::module mod = dcc::parser::parser::parse(code, name);

    // do semantic analysis
    dcc::semantic::scope::symbol_table scope = dcc::semantic::semantic_analayzer::analyze(mod);

    // TODO
    // generate code
  }

  return 0;
}
