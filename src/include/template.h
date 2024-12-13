#ifndef _TEMPLATE_H
#define _TEMPLATE_H

// Definition of Python code templates for code generation
#define TEMPLATE_COMMENT "# {content}"
#define TEMPLATE_STRING_CONST "\"{value}\""
#define TEMPLATE_INTNBOOL_CONST "{value}"
#define TEMPLATE_VAR "{id}"
#define TEMPLATE_PROPERTY "{owner}_{prop}"
#define TEMPLATE_TYPE "DataType.{type}"
#define TEMPLATE_FUNC_CALL "{name}({params})"
#define TEMPLATE_INLINE_FUNC_CALL "{name}({params})"
#define TEMPLATE_VAR_DECL "{name} = SaytringVar({init}, DataType.{type})"
#define TEMPLATE_PROP_DECL "{owner}_{name} = SaytringVar()"
#define TEMPLATE_ASSIGN "{id}.set_value({expr})"
#define TEMPLATE_IF_STATEMENT "if {condition}:\n    {_then}"
#define TEMPLATE_IF_ELSE_STATEMENT                                             \
  "if {condition}:\n    {_then}\nelse:\n    {_else}"

#define COMP_FUNC_NAME "comp"
#define ARITH_FUNC_NAME "arithmetic"

#endif
