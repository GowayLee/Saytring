#ifndef _SEMANT_H
#define _SEMANT_H

#include "AST.h"
#include "symtab.h"
#include <map>
#include <utility>
#include <vector>

// Print error information during semant check
std::ostream &semant_error(AST_Node *node);

std::ostream &semant_warn(AST_Node *node);

class Env {
public:
  static std::map<Symbol *, Symbol *> *id_map; // <Single_ID_name, Type>
  static std::map<std::pair<Symbol *, Symbol *>, Symbol *>
      *property_map; // <Owner_name, ID_name, Type>
  static std::map<Symbol *, std::vector<Symbol *> *>
      *func_map; // <Func_Name, vector<argIdentifier_types>>

  static Symbol *get_id_type(Single_Identifier *id);
  static Symbol *get_id_type(Owner_Identifier *id);
  static void update_id_type_info(Identifier *id, Symbol *new_type);
  static void update_id_type_info(Single_Identifier *id, Symbol *new_type);
  static void update_id_type_info(Owner_Identifier *id, Symbol *new_type);
};

#endif
