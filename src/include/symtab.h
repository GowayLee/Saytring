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
#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <iostream>
#include <string.h>
#include <vector>

class Symbol {
protected:
  char *str; // the string
public:
  Symbol(char *s);
  bool operator==(Symbol other);

  // Return the str and len components of the Entry.
  char *get_string() const { return str; }
};

// Store identifiers, make sure identifiers with same name is unique
class String_Tab {
private:
  std::vector<Symbol *> *symtab;

public:
  String_Tab() { this->symtab = new std::vector<Symbol *>; }
  Symbol *add_string(char *s);
};

extern String_Tab *id_tab;
extern String_Tab *str_tab;
extern String_Tab *int_tab;

// Predefined symbols & basic types in Saytring
extern Symbol *_string, *_int, *_list, *_bool, *NULL_Type, *ERR_Type,
    *LAST_RESULT;

extern Symbol *_ADD, *_SUB, *_GT, *_LT, *_GE, *_LE, *_EQ, *_NE;

extern Symbol *_anonymous;

#endif
