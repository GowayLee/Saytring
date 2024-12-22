/*
  Saytring Compiler. A compiler translating Saytring to Python.
  Copyright (C) 2024 Haoyuan Li

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "semant.h"
#include "AST.h"
#include "core_func.h"
#include "parser.tab.h"
#include "symtab.h"
#include <cstddef>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

extern char *input_filename; // from main.cc
extern int semant_error_count;
extern int semant_warn_count;

// from core_func.cc
extern std::map<std::pair<Symbol *, Symbol *>, std::string> *type_cast_map;

std::ostream &semant_error(AST_Node *node) {
  std::cerr << input_filename << ":" << node->location.first_line << ":"
            << node->location.first_column << ": Error: ";
  semant_error_count++;
  return std::cerr;
}
std::ostream &semant_warn(AST_Node *node) {
  std::cerr << input_filename << ":" << node->location.first_line << ":"
            << node->location.first_column << ": Warning: ";
  semant_warn_count++;
  return std::cerr;
}

// Static variable of Environment
std::map<Symbol *, Symbol *> *Env::id_map = new std::map<Symbol *, Symbol *>;
std::map<std::pair<Symbol *, Symbol *>, Symbol *> *Env::property_map =
    new std::map<std::pair<Symbol *, Symbol *>,
                 Symbol *>; // <Owner_name, ID_name, Type>
std::map<Symbol *, std::vector<Symbol *> *> *Env::func_map =
    new std::map<Symbol *, std::vector<Symbol *> *>;

Symbol *Env::get_id_type(Single_Identifier *id) {
  auto it = id_map->find(id->name);
  return it == id_map->end() ? nullptr : it->second;
}

Symbol *Env::get_id_type(Owner_Identifier *id) {
  auto it = property_map->find(std::make_pair(id->owner_name, id->name));
  return it == property_map->end() ? nullptr : it->second;
}

void Env::update_id_type_info(Identifier *id, Symbol *new_type) {
  if (id->has_owner())
    update_id_type_info(static_cast<Owner_Identifier *>(id), new_type);
  else
    update_id_type_info(static_cast<Single_Identifier *>(id), new_type);
}

void Env::update_id_type_info(Single_Identifier *id, Symbol *new_type) {
  auto it = Env::id_map->find(id->name);
  if (it == Env::id_map->end())
    Env::id_map->insert(std::make_pair(id->name, new_type));
  else
    it->second = new_type;
}

void Env::update_id_type_info(Owner_Identifier *id, Symbol *new_type) {
  auto new_pair = std::make_pair(id->owner_name, id->name);
  auto it = Env::property_map->find(new_pair);
  if (it == Env::property_map->end())
    Env::property_map->insert(std::make_pair(new_pair, new_type));
  else
    it->second = new_type;
}

/*-------------------------------.
|  type_check() implementation   |
`-------------------------------*/

Symbol *Nil_Expr::type_check() { return NULL_Type; }

Symbol *Nil_Identifier::type_check() { return NULL_Type; }

Symbol *Single_Identifier::type_check() {
  Symbol *temp_type;
  if ((temp_type = Env::get_id_type(this)) == nullptr) {
    semant_error(this) << "Undefined identifier \"" << this->name->get_string()
                       << "\"" << std::endl;
    return ERR_Type;
  }
  return temp_type;
}

Symbol *Owner_Identifier::type_check() {
  Symbol *temp_type;
  if ((temp_type = Env::get_id_type(this)) == nullptr) {
    semant_error(this) << "Undefined identifier \"" << owner_name->get_string()
                       << "\'s " << name->get_string() << "\"" << std::endl;
    return ERR_Type;
  }
  return temp_type;
}

Symbol *Var_Decl_Expr::type_check() {
  if (Env::id_map->find(identifier) != Env::id_map->end())
    semant_warn(this) << "Duplicate declaration of variable \""
                      << identifier->get_string() << "\"" << std::endl;
  init->type = init->type_check();
  if (init->type == ERR_Type)
    return ERR_Type;
  if (init->type == NULL_Type) {
    semant_warn(this)
        << "Should not initialize a variable with NULL_Type value!"
        << std::endl;
  }
  Env::id_map->insert(std::make_pair(identifier, init->type));
  return NULL_Type;
}

Symbol *Property_Decl_Expr::type_check() {
  if (this->owner_id->has_owner()) {
    semant_error(this) << "Cannot declare properties for a property!"
                       << std::endl;
    return ERR_Type;
  }
  // Do type-checking for owner
  Single_Identifier *single_owner_id =
      static_cast<Single_Identifier *>(this->owner_id);

  if (Env::property_map->find(
          std::make_pair(single_owner_id->name, this->property_name)) !=
      Env::property_map->end())
    semant_warn(this) << "Duplicate declaration of property \""
                      << single_owner_id->name->get_string() << "\'s "
                      << this->property_name->get_string() << "\"" << std::endl;

  single_owner_id->type = single_owner_id->type_check();
  if (single_owner_id->type == ERR_Type)
    return ERR_Type;
  if (single_owner_id->type == NULL_Type)
    semant_warn(this) << "Should not declare properties for NULL_Type variable!"
                      << std::endl;
  Env::property_map->insert(std::make_pair(
      std::make_pair(single_owner_id->name, property_name), NULL_Type));
  return NULL_Type;
}

Symbol *Assi_Expr::type_check() {
  id->type = id->type_check();
  if (id->type == ERR_Type)
    return ERR_Type;

  expr->type = expr->type_check();
  if (expr->type == ERR_Type)
    return ERR_Type;
  if (expr->type == NULL_Type)
    semant_warn(this) << "Should not assign a variable with NULL_Type value!"
                      << std::endl;

  // Pass type-checking, then update id's type information in Env
  // Assert id exists in Env
  if (id->has_owner()) {
    Owner_Identifier *owner_id = static_cast<Owner_Identifier *>(id);
    std::pair prop_pair = std::make_pair(owner_id->owner_name, owner_id->name);
    // Delete previous type information
    Env::property_map->erase(Env::property_map->find(prop_pair));
    // Insert new type information
    Env::property_map->insert(std::make_pair(prop_pair, expr->type));
  } else {
    Single_Identifier *single_id = static_cast<Single_Identifier *>(id);
    Env::id_map->erase(Env::id_map->find(single_id->name));
    Env::id_map->insert(std::make_pair(single_id->name, expr->type));
  }
  return NULL_Type;
}

Symbol *Cast_Expr::type_check() {
  id->type = id->type_check();
  if (id->type == ERR_Type)
    return ERR_Type;
  return_id->type_check();
  if (id->type == ERR_Type)
    return ERR_Type;

  if (id->type == to_type) {
    semant_warn(this) << "Try to perform type casting between two same types."
                      << std::endl;
    return NULL_Type;
  }

  // Check type pair in type_cast_map
  auto it = type_cast_map->find(std::make_pair(id->type, to_type));
  if (it == type_cast_map->end()) {
    semant_error(this) << "There is no cast for \"" << id->type->get_string()
                       << "\" -> \"" << to_type->get_string() << "\""
                       << std::endl;
    return ERR_Type;
  }

  Env::update_id_type_info(id, to_type);
  Env::update_id_type_info(return_id, _bool);
  return NULL_Type;
}

Symbol *Direct_Call_Expr::type_check() {
  id->type = id->type_check();
  if (id->type == ERR_Type)
    return ERR_Type;
  return_id->type = return_id->type_check();
  if (id->type == ERR_Type)
    return ERR_Type;

  // Check function
  auto it = Env::func_map->find(func_name);
  if (it == Env::func_map->end()) {
    semant_error(this) << "Undefined function \"" << func_name->get_string()
                       << "\" is called!" << std::endl;
    return ERR_Type;
  }
  std::vector<Symbol *> *func_arg_list = it->second;

  // Do type-check for arg_list
  // Check number of args
  size_t func_arg_list_size = func_arg_list->size();
  size_t actual_arg_list_size = arg_list->size();
  // function caller_id is the 1st arg
  // func_arg_list contain return_type. So -2
  if (func_arg_list_size - 2 > actual_arg_list_size) {
    semant_error(this) << "Missing args for function \""
                       << func_name->get_string() << "\", require "
                       << func_arg_list->size() - 2 << " args!" << std::endl;
    return ERR_Type;
  } else if (func_arg_list_size - 2 < actual_arg_list_size) {
    semant_error(this) << "Too more args for function \""
                       << func_name->get_string() << "\", require "
                       << func_arg_list->size() - 2 << " args!" << std::endl;
    return ERR_Type;
  }
  // Check type
  // First check function caller, which is at the back of actual_arg_list
  id->type = id->type_check();
  if (id->type == ERR_Type)
    return ERR_Type;
  if (func_arg_list->at(0) != id->type)
    semant_warn(this) << "The variable calling function does not match proper "
                         "type! Required: \""
                      << func_arg_list->at(0)->get_string() << "\", Actual \""
                      << id->type->get_string() << "\"" << std::endl;
  // Then check rest args
  for (size_t i = 1; i < func_arg_list_size - 1; i++) {
    Symbol *func_arg_type = func_arg_list->at(i);
    // Arguments are collected in inverse order
    Expression *cur_arg = arg_list->at(actual_arg_list_size - i);
    cur_arg->type = cur_arg->type_check();
    Symbol *actual_arg_type = arg_list->at(actual_arg_list_size - i)->type;
    if (actual_arg_type == ERR_Type)
      return ERR_Type;
    if (actual_arg_type == NULL_Type)
      semant_warn(this)
          << "Should never pass a NULL_Type variable as parameter."
          << std::endl;
    if (actual_arg_type != func_arg_type)
      semant_warn(this) << "Calling function \"" << func_name->get_string()
                        << "\", the " << i
                        << "th arg is different with the required! Required: \""
                        << func_arg_type->get_string() << "\", Actual: \""
                        << actual_arg_type->get_string() << "\"" << std::endl;
  }
  // Update type information of return_id
  Env::update_id_type_info(return_id, func_arg_list->back());
  return func_arg_list->back();
}

Symbol *Cond_Call_Expr::type_check() {
  semant_error(this)
      << "Here should not appear Type-checking for Cond_Call_Expr!"
      << std::endl;
  return ERR_Type;
}

Symbol *Cond_Expr::type_check() {
  // Do type-check for Predictor
  predictor->type = predictor->type_check();
  if (predictor->type == ERR_Type)
    return ERR_Type;
  if (predictor->type != _bool)
    semant_warn(this) << "In IF-THEN-ELSE expression, predictor should be "
                         "bool type! Actual type: \""
                      << predictor->type->get_string() << "\"" << std::endl;
  return NULL_Type;
}

Symbol *Comp_Expr::type_check() {
  e1->type = e1->type_check();
  e2->type = e2->type_check();

  if (e1->type == ERR_Type || e2->type == ERR_Type)
    return ERR_Type;
  if (e1->type == e2->type && (e1->type == _string || e1->type == _int))
    return _bool;
  else {
    semant_warn(this)
        << "In Comparison expression, compared expressions should all be "
           "string type or int type! Actual type: \""
        << e1->type->get_string() << "\" compare \"" << e2->type->get_string()
        << "\"" << std::endl;
    return _bool;
  }
}

Symbol *Arith_Expr::type_check() {
  e1->type = e1->type_check();
  e2->type = e2->type_check();

  if (e1->type == ERR_Type || e2->type == ERR_Type)
    return ERR_Type;
  if (e1->type == e2->type && e1->type == _int)
    return _int;
  if (e1->type == e2->type && e1->type == _string)
    return _string;
  else {
    semant_warn(this) << "In Arithmetic expression, expressions should all be "
                         "int type! Actual type: \""
                      << e1->type->get_string() << "\" op \""
                      << e2->type->get_string() << std::endl;
    return _int;
  }
}

Symbol *String_Const_Expr::type_check() { return _string; }

Symbol *Int_Const_Expr::type_check() { return _int; }

Symbol *Bool_Const_Expr::type_check() { return _bool; }

void Program::semant_check() {
  // Set up predefined types
  install_type_cast_map();
  install_buildin_func();
  install_buildin_var();

  // Do type-check
  for (Expression *expr : *this->expr_list)
    expr->type = expr->type_check();
  return;
}
