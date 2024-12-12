#include "semant.h"
#include "AST.h"
#include "parser.tab.h"
#include "symtab.h"
#include <cmath>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

extern char *input_filename;
extern int semant_error_count;
extern int semant_warn_count;

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

extern Symbol *_string, *_int, *_list, *_bool, *NULL_Type, *ERR_Type,
    *LAST_RESULT;

void install_buildin_func() {
  std::vector<Symbol *> *arg_list;

  // Type-cast Functions

  // cast_null_to_str(NULL_Type) : string
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(NULL_Type);
  arg_list->push_back(_string); // return_type
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("cast_null_to_str"), arg_list));

  // cast_null_to_int(NULL_Type) : int
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(NULL_Type);
  arg_list->push_back(_int); // return_type
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("cast_null_to_int"), arg_list));

  // cast_null_to_bool(NULL_Type) : bool
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(NULL_Type);
  arg_list->push_back(_bool); // return_type
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("cast_null_to_bool"), arg_list));

  // cast_str_to_bool(string) : bool
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_string);
  arg_list->push_back(_bool); // return_type
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("cast_str_to_bool"), arg_list));

  // cast_str_to_int(string) : int
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_string);
  arg_list->push_back(_int); // return_type
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("cast_str_to_int"), arg_list));

  // cast_int_to_bool(int) : bool
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_int);
  arg_list->push_back(_bool); // return_type
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("cast_int_to_bool"), arg_list));

  // cast_bool_to_int(bool) : int
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_bool);
  arg_list->push_back(_int); // return_type
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("cast_bool_to_int"), arg_list));

  // concat(string, string) : string
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_string);
  arg_list->push_back(_string);
  arg_list->push_back(_string); // return_type
  Env::func_map->insert(std::make_pair(id_tab->add_string("concat"), arg_list));

  // substring(string, int, int) : string
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_string);
  arg_list->push_back(_int);
  arg_list->push_back(_int);
  arg_list->push_back(_string); // return_type
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("substring"), arg_list));

  // substring_from_start(string, int) : string  override
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_string);
  arg_list->push_back(_int);
  arg_list->push_back(_string); // return_type
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("substring_from_start"), arg_list));

  // get_length(string) : int
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_string);
  arg_list->push_back(_int);
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("get_length"), arg_list));

  // reverse(string) : string
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_string);
  arg_list->push_back(_string); // return_type
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("reverse"), arg_list));

  // is_palindrome(string) : bool
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_string);
  arg_list->push_back(_bool); // return_type
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("is_palindrome"), arg_list));

  // say(NULL_Type, string) : NULL_Type
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(NULL_Type);
  arg_list->push_back(_string);
  arg_list->push_back(NULL_Type);
  Env::func_map->insert(std::make_pair(id_tab->add_string("say"), arg_list));

  // ask(NULL_Type) : NULL_Type
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(NULL_Type);
  arg_list->push_back(NULL_Type);
  Env::func_map->insert(std::make_pair(id_tab->add_string("ask"), arg_list));

  // ask_with_prompt(NULL_Type, string) : NULL_Type
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(NULL_Type);
  arg_list->push_back(_string);
  arg_list->push_back(NULL_Type);
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("ask_with_prompt"), arg_list));
}

/*-------------------------------.
|  type_check() implementation   |
`-------------------------------*/

void Nil_Expr::type_check() {
  this->type = NULL_Type;
  return;
}

void Nil_Identifier::type_check() {
  this->type = NULL_Type;
  return;
}

void Single_Identifier::type_check() {
  if ((this->type = Env::get_id_type(this)) == nullptr) {
    semant_error(this) << "Undefined identifier \"" << this->name->get_string()
                       << "\"" << std::endl;
    this->type = ERR_Type;
  }
  return;
}

void Owner_Identifier::type_check() {
  if ((this->type = Env::get_id_type(this)) == nullptr) {
    semant_error(this) << "Undefined identifier \""
                       << this->owner_name->get_string() << "\'s "
                       << this->name->get_string() << "\"" << std::endl;
    this->type = ERR_Type;
  }
  return;
}

void Var_Decl_Expr::type_check() {
  if (Env::id_map->find(this->identifier) != Env::id_map->end())
    semant_warn(this) << "Duplicate declaration of variable \""
                      << this->identifier->get_string() << "\"" << std::endl;
  this->init->type_check();
  if (this->init->type == ERR_Type)
    return;
  if (this->init->type == NULL_Type) {
    semant_warn(this)
        << "Should not initialize a variable with NULL_Type value!"
        << std::endl;
    return;
  }
  Env::id_map->insert(std::make_pair(this->identifier, this->init->type));
  return;
}

void Property_Decl_Expr::type_check() {
  if (this->owner_id->has_owner()) {
    semant_error(this) << "Cannot declare properties for a property!"
                       << std::endl;
    return;
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

  single_owner_id->type_check();
  if (single_owner_id->type == ERR_Type)
    return;
  if (single_owner_id->type == NULL_Type)
    semant_warn(this) << "Should not declare properties for NULL_Type variable!"
                      << std::endl;
  Env::property_map->insert(std::make_pair(
      std::make_pair(single_owner_id->name, property_name), NULL_Type));
  return;
}

void Assi_Expr::type_check() {
  this->id->type_check();
  if (this->id->type == ERR_Type)
    return;

  this->expr->type_check();
  if (this->expr->type == ERR_Type)
    return;
  if (this->expr->type == NULL_Type)
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
  return;
}

void Direct_Call_Expr::type_check() {
  this->id->type_check();
  if (this->id->type == ERR_Type)
    return;
  this->return_id->type_check();
  if (this->id->type == ERR_Type)
    return;

  // Check function
  auto it = Env::func_map->find(this->func_name);
  if (it == Env::func_map->end()) {
    semant_error(this) << "Undefined function \""
                       << this->func_name->get_string() << "\" is called!"
                       << std::endl;
    return;
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
                       << this->func_name->get_string() << "\", require "
                       << func_arg_list->size() - 2 << " args!" << std::endl;
    return;
  } else if (func_arg_list_size - 2 < actual_arg_list_size) {
    semant_error(this) << "Too more args for function \""
                       << this->func_name->get_string() << "\", require "
                       << func_arg_list->size() - 2 << " args!" << std::endl;
    return;
  }
  // Check type
  // First check function caller, which is at the back of actual_arg_list
  this->id->type_check();
  if (this->id->type == ERR_Type)
    return;
  if (func_arg_list->at(0) != this->id->type)
    semant_warn(this) << "The variable calling function does not match proper "
                         "type! Required: \""
                      << func_arg_list->at(0)->get_string() << "\", Actual \""
                      << this->id->type->get_string() << "\"" << std::endl;
  // Then check rest args
  for (size_t i = 1; i < func_arg_list_size - 1; i++) {
    Symbol *func_arg_type = func_arg_list->at(i);
    // Arguments are collected in inverse order
    arg_list->at(actual_arg_list_size - i)->type_check();
    Symbol *actual_arg_type = arg_list->at(actual_arg_list_size - i)->type;
    if (actual_arg_type == ERR_Type)
      return;
    if (actual_arg_type == NULL_Type)
      semant_warn(this)
          << "Should never pass a NULL_Type variable as parameter."
          << std::endl;
    if (actual_arg_type != func_arg_type) {
      semant_warn(this) << "Calling function \""
                        << this->func_name->get_string() << "\", the " << i
                        << "th arg is different with the required! Required: \""
                        << func_arg_type->get_string() << "\", Actual: \""
                        << actual_arg_type->get_string() << "\"" << std::endl;
      return;
    }
  }
  // Update type information of return_id
  Env::update_id_type_info(return_id, func_arg_list->back());
  this->type = func_arg_list->back();
  return;
}

void Cond_Call_Expr::type_check() {
  semant_error(this)
      << "Here should not appear Type-checking for Cond_Call_Expr!"
      << std::endl;
  return;
}

void Cond_Expr::type_check() {
  // Do type-check for Predictor
  predictor->type_check();
  if (predictor->type == ERR_Type)
    return;
  if (predictor->type != _bool) {
    semant_warn(this) << "In IF-THEN-ELSE expression, predictor should be "
                         "bool type! Actual type: \""
                      << predictor->type->get_string() << "\"" << std::endl;
    return;
  }
  this->type = NULL_Type;
  return;
}

void Comp_Expr::type_check() {
  this->e1->type_check();
  this->e2->type_check();

  if (e1->type == ERR_Type || e2->type == ERR_Type)
    return;
  if (e1->type == e2->type && (e1->type == _string || e1->type == _int))
    this->type = _bool;
  else {
    semant_warn(this)
        << "In Comparison expression, compared expressions should all be "
           "string type or int type! Actual type: \""
        << e1->type->get_string() << "\" compare \"" << e2->type->get_string()
        << "\"" << std::endl;
    this->type = ERR_Type;
  }
  return;
}

void Arith_Expr::type_check() {
  this->e1->type_check();
  this->e2->type_check();

  if (e1->type == ERR_Type || e2->type == ERR_Type)
    return;
  if (e1->type == e2->type && e1->type == _int)
    this->type = _int;
  else {
    semant_warn(this) << "In Arithmetic expression, expressions should all be "
                         "int type! Actual type: \""
                      << e1->type->get_string() << "\" op \""
                      << e2->type->get_string() << std::endl;
  }
  return;
}

void String_Const_Expr::type_check() {
  this->type = _string;
  return;
}

void Int_Const_Expr::type_check() {
  this->type = _int;
  return;
}

void Bool_Const_Expr::type_check() {
  this->type = _bool;
  return;
}

void Program::semant_check() {
  // Set up predefined types
  install_buildin_func();

  // Do type-check
  for (Expression *expr : *this->expr_list)
    expr->type_check();
  return;
}
