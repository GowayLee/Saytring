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
