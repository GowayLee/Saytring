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
#ifndef _UTIL_H_
#define _UTIL_H_

#include "AST.h"
#include "symtab.h"

using namespace std;

extern const char *token_to_string(int tok);
extern void print_token(int tok);
extern void print_escaped_string(std::ostream &str, const char *s);

bool has_same_owner(Identifier *id1, Identifier *id2);
Owner_Identifier *adjust_return_id(Identifier *id1, Identifier *id2);
Owner_Identifier *adjust_return_id(Identifier *id);

#endif
