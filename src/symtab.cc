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
#include "symtab.h"
#include <cstring>

Symbol::Symbol(char *s) {
  int len =
      (int)strlen(s); // store length of string without '\0', help to add '\0'
  str = new char[len + 1];
  strncpy(str, s, len);
  str[len] = '\0';
}

bool Symbol::operator==(Symbol other) {
  return std::strcmp(str, other.str) == 0;
}

Symbol *String_Tab::add_string(char *s) {
  Symbol *new_sym = new Symbol(s);
  for (Symbol *sym : *symtab) {
    if (*new_sym == *sym) { // If find same symbol, return it
      delete new_sym;
      return sym;
    }
  }
  this->symtab->push_back(new_sym);
  return new_sym;
}

String_Tab *id_tab = new String_Tab();
String_Tab *str_tab = new String_Tab();
String_Tab *int_tab = new String_Tab();

Symbol *_string = id_tab->add_string("_string");
Symbol *_int = id_tab->add_string("_int");
Symbol *_list = id_tab->add_string("_list");
Symbol *_bool = id_tab->add_string("_bool");
Symbol *NULL_Type = id_tab->add_string("NULL_Type");
Symbol *ERR_Type = id_tab->add_string("ERR_Type");
Symbol *LAST_RESULT = id_tab->add_string("last_result");

Symbol *_ADD = new Symbol("ADD");
Symbol *_SUB = new Symbol("SUB");
Symbol *_GT = new Symbol("GT");
Symbol *_LT = new Symbol("LT");
Symbol *_GE = new Symbol("GE");
Symbol *_LE = new Symbol("LE");
Symbol *_EQ = new Symbol("EQ");
Symbol *_NE = new Symbol("NE");

Symbol *_anonymous = new Symbol("_anonymous");