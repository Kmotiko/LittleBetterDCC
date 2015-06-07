#ifndef DCC_AST_FWD_HPP
#define DCC_AST_FWD_HPP

#include <memory>
#include <boost/variant/variant.hpp>

namespace dcc {
namespace ast {

struct translation_unit;
struct function_declaration;
struct function_definition;
struct prototype;
struct variable_declaration;
struct function_statement;
struct parameter;
struct jump_statement;
struct expression_statement;
struct function_statement;
struct binary_expression;
struct call_expression;
struct number_literal;
struct identifier;


using translation_unit_ptr      = std::shared_ptr<translation_unit>;
using function_declaration_ptr  = std::shared_ptr<function_declaration>;
using function_definition_ptr   = std::shared_ptr<function_definition>;
using prototype_ptr             = std::shared_ptr<prototype>;
using variable_declaration_ptr  = std::shared_ptr<variable_declaration>;
using function_statement_ptr    = std::shared_ptr<function_statement>;
using parameter_ptr             = std::shared_ptr<parameter>;
using jump_statement_ptr        = std::shared_ptr<jump_statement>;
using expression_statement_ptr  = std::shared_ptr<expression_statement>;
using function_statement_ptr    = std::shared_ptr<function_statement>;
using binary_expression_ptr     = std::shared_ptr<binary_expression>;
using call_expression_ptr       = std::shared_ptr<call_expression>;
using number_literal_ptr        = std::shared_ptr<number_literal>;
using identifier_ptr            = std::shared_ptr<identifier>;

using one_of_expr = boost::variant<
        identifier_ptr,
        number_literal_ptr,
        parameter_ptr,
        binary_expression_ptr,
        call_expression_ptr
        >;

using one_of_statement =  boost::variant<
        variable_declaration_ptr,
        jump_statement_ptr,
        one_of_expr
        >; 



} // end of namespace ast
} // end of namespace dcc

#endif
