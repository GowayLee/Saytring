#ifndef _CGEN_H_
#define _CGEN_H_

#include "AST.h"
#include "template.h"
#include <string>
#include <unordered_map>

class Code_Generator {
private:
  std::unordered_map<std::string, std::string> templates;

public:
  // Initialize code templates
  // #define TEMPLATE_COMMENT "# {content}\n"
  // #define TEMPLATE_STRING_CONST "\"{value}\""
  // #define TEMPLATE_INTNBOOL_CONST "{value}"
  // #define TEMPLATE_VAR "# {id}"
  // #define TEMPLATE_PROPERTY "# {owner}_{prop}"
  // #define TEMPLATE_TYPE "DataType.{type}"
  // #define TEMPLATE_FUNC_CALL "{name}({params})\n"
  // #define TEMPLATE_INLINE_FUNC_CALL "{name}({params})"
  // #define TEMPLATE_VAR_DECL "{name} = Saytring({init}, {type})\n"
  // #define TEMPLATE_PROP_DECL "{owner}_{name} = Saytring()\n"
  // #define TEMPLATE_ASSIGN "{id} = {expr}\n"
  // #define TEMPLATE_IF_STATEMENT "if {condition}:\n    {_then}\n"
  // #define TEMPLATE_IF_ELSE_STATEMENT \
  //   "if {condition}:\n    {_then}\nelse:\n    {_else}\n"
  Code_Generator() {
    templates["comment"] = TEMPLATE_COMMENT;
    templates["string"] = TEMPLATE_STRING_CONST;
    templates["intnbool"] = TEMPLATE_INTNBOOL_CONST;
    templates["var"] = TEMPLATE_VAR;
    templates["property"] = TEMPLATE_PROPERTY;
    templates["type"] = TEMPLATE_TYPE;
    templates["func_call"] = TEMPLATE_FUNC_CALL;
    templates["inline_func_call"] = TEMPLATE_INLINE_FUNC_CALL;
    templates["var_decl"] = TEMPLATE_VAR_DECL;
    templates["prop_decl"] = TEMPLATE_PROP_DECL;
    templates["assign"] = TEMPLATE_ASSIGN;
    templates["if_statement"] = TEMPLATE_IF_STATEMENT;
    templates["if_else_statement"] = TEMPLATE_IF_ELSE_STATEMENT;
  }

  // Generate code according to template
  std::string
  generate(const std::string &template_name,
           const std::unordered_map<std::string, std::string> &params) {
    std::string code = templates[template_name];
    for (const auto &[key, value] : params) {
      size_t pos = code.find("{" + key + "}");
      while (pos != std::string::npos) {
        code.replace(pos, key.length() + 2, value);
        pos = code.find("{" + key + "}", pos + value.length());
      }
    }
    return code;
  }
};

#endif
