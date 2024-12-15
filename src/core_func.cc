#include "core_func.h"
#include "semant.h"
#include "symtab.h"
#include <map>
#include <string>
#include <utility>

// Type_cast map entries
#define TYPE_CAST_MAP_ENTRY(t1, t2, func_name)                                 \
  std::make_pair(std::make_pair(t1, t2), func_name))

auto *type_cast_map = new std::map<std::pair<Symbol *, Symbol *>, std::string>;

void install_type_cast_map() {
  type_cast_map->insert(TYPE_CAST_MAP_ENTRY(_int, _string, "cast_int_to_str");
  type_cast_map->insert(TYPE_CAST_MAP_ENTRY(_int, _bool, "cast_int_to_bool");
  type_cast_map->insert(TYPE_CAST_MAP_ENTRY(_string, _int, "cast_str_to_int");
  type_cast_map->insert(TYPE_CAST_MAP_ENTRY(_string, _bool, "cast_str_to_bool");
  type_cast_map->insert(TYPE_CAST_MAP_ENTRY(_bool, _string, "cast_bool_to_str");
  type_cast_map->insert(TYPE_CAST_MAP_ENTRY(_bool, _int, "cast_bool_to_int");
  type_cast_map->insert(TYPE_CAST_MAP_ENTRY(_list, _string, "cast_list_to_str");
  type_cast_map->insert(TYPE_CAST_MAP_ENTRY(NULL_Type, _string, "cast_null_to_str");
  type_cast_map->insert(TYPE_CAST_MAP_ENTRY(NULL_Type, _int, "cast_null_to_int");
  type_cast_map->insert(TYPE_CAST_MAP_ENTRY(NULL_Type, _bool, "cast_null_to_bool");
  type_cast_map->insert(TYPE_CAST_MAP_ENTRY(NULL_Type, _string, "cast_null_to_str");
}

void install_buildin_func() {
  std::vector<Symbol *> *arg_list;

  // Type-cast Functions
  // cast_int_to_str(NULL_Type) : string
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_int);
  arg_list->push_back(_string); // return_type
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("cast_int_to_str"), arg_list));

  // cast_bool_to_str(NULL_Type) : string
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_bool);
  arg_list->push_back(_string); // return_type
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("cast_bool_to_str"), arg_list));

  // cast_list_to_str(NULL_Type) : string
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_list);
  arg_list->push_back(_string); // return_type
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("cast_list_to_str"), arg_list));

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

  // replace(string, string, string) : string
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_string);
  arg_list->push_back(_string);
  arg_list->push_back(_string);
  arg_list->push_back(_string);
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("replace"), arg_list));

  // find(string, string) : int
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_string);
  arg_list->push_back(_string);
  arg_list->push_back(_int);
  Env::func_map->insert(std::make_pair(id_tab->add_string("find"), arg_list));

  // to_lower(string) : string
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_string);
  arg_list->push_back(_string);
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("to_lower"), arg_list));

  // to_upper(string) : string
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_string);
  arg_list->push_back(_string);
  Env::func_map->insert(
      std::make_pair(id_tab->add_string("to_upper"), arg_list));

  // trim(string) : string
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_string);
  arg_list->push_back(_string);
  Env::func_map->insert(std::make_pair(id_tab->add_string("trim"), arg_list));

  // split(string, string) : list
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_string);
  arg_list->push_back(_string);
  arg_list->push_back(_list);
  Env::func_map->insert(std::make_pair(id_tab->add_string("split"), arg_list));

  // get_at(list, int) : string
  arg_list = new std::vector<Symbol *>;
  arg_list->push_back(_list);
  arg_list->push_back(_int);
  arg_list->push_back(_string);
  Env::func_map->insert(std::make_pair(id_tab->add_string("get_at"), arg_list));
}
