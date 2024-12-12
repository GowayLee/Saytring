#include "cgen.h"
#include "AST.h"
#include "symtab.h"
#include "template.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

extern char *output_filename;
extern Symbol *_string, *_int, *_list, *_bool, *NULL_Type, *ERR_Type,
    *LAST_RESULT;

std::ostringstream generated_code;
Code_Generator *cg = new Code_Generator();

void Program::code_generation() {
  for (Expression *expr : *expr_list) {
    expr->code_generate(generated_code);
    generated_code << "\n";
  }

  // Write into output_file
  std::ofstream out_file(output_filename);
  if (out_file.is_open()) {
    out_file.write(generated_code.str().c_str(), generated_code.str().size());
    out_file.close();
    std::cout << "Code successfully written to " << output_filename
              << std::endl;
  } else {
    std::cerr << "Unable to open file: " << output_filename << std::endl;
  }
}

void Expression::code_generate(std::ostringstream &generated_code) {
  generated_code << this->code_generate();
}

/*----------------------------------.
|  code_generate() implementation   |
`----------------------------------*/

std::string Nil_Expr::code_generate() {
  // Dump comment
  std::unordered_map<std::string, std::string> params;
  params = {{"content", "Nil_Expr"}};
  return cg->generate("comment", params);
}

std::string Single_Identifier::code_generate() {
  // Construct Single_Identifer expression template params
  std::unordered_map<std::string, std::string> params;
  params = {{"identifier", this->name->get_string()}};
  return cg->generate("var", params);
}

std::string Owner_Identifier::code_generate() {
  std::unordered_map<std::string, std::string> params;
  params["owner"] = this->owner_name->get_string();
  params["prop"] = this->name->get_string();
  return cg->generate("var", params);
}

std::string Nil_Identifier::code_generate() { return ""; }

std::string Var_Decl_Expr::code_generate() {
  std::ostringstream buf;
  std::unordered_map<std::string, std::string> params;
  // Dump comment
  params = {{"content",
             std::string("Var_Decl_Expr: ") + this->identifier->get_string()}};
  buf << cg->generate("comment", params);

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

  // Dump comment
  buf << "Property_Decl_Expr: " << si->name->get_string() << "\'s "
      << this->property_name->get_string();
  params = {{"content", buf.str()}};
  buf.clear();
  buf << cg->generate("comment", params);

  params.clear();
  params["owner"] = si->name->get_string();
  params["name"] = this->property_name->get_string();
  buf << cg->generate("prop_decl", params);
  return buf.str();
}

std::string Assi_Expr::code_generate() {
  std::ostringstream buf;
  std::unordered_map<std::string, std::string> params;

  // Dump comment
  if (this->id->has_owner())
    buf << "Assign_Expr: "
        << static_cast<Owner_Identifier *>(id)->owner_name->get_string()
        << "\'s " << static_cast<Owner_Identifier *>(id)->name->get_string();
  else
    buf << "Assign_Expr: "
        << static_cast<Single_Identifier *>(id)->name->get_string();
  params = {{"content", buf.str()}};
  buf.clear();
  buf << cg->generate("comment", params);

  buf.clear();
  params["id"] = this->id->code_generate();
  params["expr"] = this->expr->code_generate();
  buf << cg->generate("assign", params);
  return buf.str();
}

std::string Direct_Call_Expr::code_generate() {
  std::unordered_map<std::string, std::string> params;
  std::ostringstream buf;

  // Dump comment
  if (this->id->has_owner()) {
    buf << "Direct_Call_Expr: "
        << static_cast<Owner_Identifier *>(id)->owner_name->get_string()
        << "\'s " << static_cast<Owner_Identifier *>(id)->name->get_string()
        << " do " << this->func_name->get_string();
  } else
    buf << "Direct_Call_Expr: "
        << static_cast<Single_Identifier *>(id)->name->get_string() << " do "
        << this->func_name->get_string();
  params = {{"content", buf.str()}};
  buf.clear();
  buf << cg->generate("comment", params);

  params.clear();
  buf.clear();

  params["name"] = this->func_name->get_string();

  std::ostringstream arg_buf;
  arg_buf << this->id->code_generate();
  // Adjust ','
  if (arg_list->size() > 0) {
    if (this->id->is_nil())
      arg_buf << arg_list->at(0)->code_generate();
    else
      arg_buf << ", " << arg_list->at(0)->code_generate();
  }
  // Append args
  for (size_t i = 1; i < arg_list->size(); i++)
    arg_buf << ", " << arg_list->at(i)->code_generate();
  // Append return_id
  arg_buf << this->return_id->code_generate();
  params["params"] = arg_buf.str();

  buf << cg->generate("func_call", params);

  return buf.str();
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