#include "cgen.h"
#include "AST.h"
#include "symtab.h"
#include "template.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>

extern char *output_filename;
extern char *runtime_filename;
extern Symbol *_string, *_int, *_list, *_bool, *NULL_Type, *ERR_Type,
    *LAST_RESULT;

// from core_func.cc
extern std::map<std::pair<Symbol *, Symbol *>, std::string> *type_cast_map;

std::ostringstream generated_code;
Code_Generator *cg = new Code_Generator();

void Program::code_generation() {
  // Generate code node by node
  for (Expression *expr : *expr_list) {
    expr->code_generate(generated_code);
    generated_code << "\n";
  }

  // Write into output_file
  std::ofstream out_file(output_filename);
  std::ifstream runtime_file(runtime_filename);
  // Copy runtime into output_file
  out_file << runtime_file.rdbuf();

  if (!runtime_file && !out_file) {
    std::cerr << "Error in copy Runtime file to output file!" << std::endl;
    return;
  }

  // Input geneated code into output_file
  if (out_file.is_open()) {
    out_file.write(generated_code.str().c_str(), generated_code.str().size());
    out_file.close();
    std::cout << "Generate code to " << output_filename
              << std::endl;
  } else {
    std::cerr << "Unable to open file: " << output_filename << std::endl;
  }
}

// Output generated code into ostring stream
void Expression::code_generate(std::ostringstream &generated_code) {
  generated_code << this->code_generate();
}

/*----------------------------------.
|  code_generate() implementation   |
`----------------------------------*/

std::string Nil_Expr::code_generate() { return ""; }

std::string Single_Identifier::code_generate() {
  // Construct Single_Identifer expression template params
  std::unordered_map<std::string, std::string> params;
  params["id"] = this->name->get_string();
  return cg->generate("var", params);
}

std::string Owner_Identifier::code_generate() {
  std::unordered_map<std::string, std::string> params;
  params["owner"] = this->owner_name->get_string();
  params["prop"] = this->name->get_string();
  return cg->generate("property", params);
}

std::string Nil_Identifier::code_generate() { return ""; }

std::string Var_Decl_Expr::code_generate() {
  std::ostringstream buf;
  std::unordered_map<std::string, std::string> params;

  params.clear();
  params["name"] = this->identifier->get_string();
  params["init"] = this->init->code_generate();
  if (this->init->type == _string)
    params["type"] = "STRING";
  else if (this->init->type == _int)
    params["type"] = "INT";
  else if (this->init->type == _bool)
    params["type"] = "BOOL";
  else
    params["type"] = "NULL_TYPE";
  buf << cg->generate("var_decl", params);

  return buf.str(); // to std:string
}

std::string Property_Decl_Expr::code_generate() {
  std::ostringstream buf;
  std::unordered_map<std::string, std::string> params;
  // Assert this->identifier is a Single_Identifier
  Single_Identifier *si = static_cast<Single_Identifier *>(this->owner_id);

  params.clear();
  params["owner"] = si->name->get_string();
  params["name"] = this->property_name->get_string();
  buf << cg->generate("prop_decl", params);
  return buf.str();
}

std::string Assi_Expr::code_generate() {
  std::unordered_map<std::string, std::string> params;

  params["id"] = this->id->code_generate();
  params["expr"] = this->expr->code_generate();
  return cg->generate("assign", params);
}

std::string Cast_Expr::code_generate() {
  // Generation nothing if dest type is NULL_Type
  if (to_type == NULL_Type || to_type == _list)
    return "";
  // Generation nothing if source type = dest type
  if (id->type == to_type)
    return "";

  std::ostringstream buf;
  std::unordered_map<std::string, std::string> params;

  // Generate function name
  auto it = type_cast_map->find(std::make_pair(id->type, to_type));
  if (it == type_cast_map->end())
    return ""; // Should never reach here
  else
    params["name"] = it->second;

  buf << id->code_generate() << ", " << return_id->code_generate();
  params["params"] = buf.str();
  return cg->generate("func_call", params);
}

std::string Direct_Call_Expr::code_generate() {
  std::unordered_map<std::string, std::string> params;

  params["name"] = this->func_name->get_string();

  // Need to reverse the list, since yacc has collected args in inverse order
  std::ostringstream arg_buf;
  arg_buf << this->id->code_generate();

  int arg_size = arg_list->size();
  // Adjust ','
  if (arg_size > 0) {
    if (this->id->is_nil())
      arg_buf << arg_list->at(arg_size - 1)->code_generate();
    else
      arg_buf << ", " << arg_list->at(arg_size - 1)->code_generate();
  }
  // Append args
  for (size_t i = arg_size - 1; i > 0; i--)
    arg_buf << ", " << arg_list->at(i - 1)->code_generate();
  // Append return_id
  if (!return_id->is_nil())
    arg_buf << ", ";
  arg_buf << this->return_id->code_generate();
  params["params"] = arg_buf.str();

  return cg->generate("func_call", params);
}

std::string Cond_Call_Expr::code_generate() {
  std::cerr << "Here should not appear Cond_Call_Expr!" << std::endl;
  return "";
}

std::string Cond_Expr::code_generate() {
  std::unordered_map<std::string, std::string> params;

  params["condition"] = this->predictor->code_generate();
  params["_then"] = this->then->code_generate();

  if (!this->has_else)
    return cg->generate("if_statement", params);

  params["_else"] = this->_else->code_generate();
  return cg->generate("if_else_statement", params);
}

std::string Comp_Expr::code_generate() {
  std::unordered_map<std::string, std::string> params;
  std::ostringstream buf;

  params["name"] = COMP_FUNC_NAME;
  buf << this->e1->code_generate() << ", " << this->e2->code_generate() << ", "
      << "\"" << this->op->get_string() << "\"";
  params["params"] = buf.str();
  return cg->generate("func_call", params);
}

std::string Arith_Expr::code_generate() {
  std::unordered_map<std::string, std::string> params;
  std::ostringstream buf;

  params["name"] = ARITH_FUNC_NAME;
  buf << this->e1->code_generate() << ", " << this->e2->code_generate() << ", "
      << "\"" << this->op->get_string() << "\"";
  params["params"] = buf.str();
  return cg->generate("func_call", params);
}

std::string String_Const_Expr::code_generate() {
  std::unordered_map<std::string, std::string> params;

  params["value"] = this->token->get_string();
  return cg->generate("string", params);
}

std::string Int_Const_Expr::code_generate() {
  std::unordered_map<std::string, std::string> params;

  params["value"] = this->token->get_string();
  return cg->generate("intnbool", params);
}

std::string Bool_Const_Expr::code_generate() {
  std::unordered_map<std::string, std::string> params;

  if (this->value)
    params["value"] = "True";
  else
    params["value"] = "False";
  return cg->generate("intnbool", params);
}
