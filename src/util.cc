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
#include "util.h"
#include "AST.h"
#include "parser.tab.h"
#include "symtab.h"
#include <ctype.h>
#include <iomanip>
#include <iostream>
#include <map>

std::map<int, const char *> token_map = {{0, "EOF"},
                                         {DEFINE, "DEFINE"},
                                         {USING, "USING"},
                                         {ON, "ON"},
                                         {ENDIF, "ENDIF"},
                                         {ASK, "ASK"},
                                         {SAY, "SAY"},
                                         {DO, "DO"},
                                         {TO, "TO"},
                                         {CONVERT, "CONVERT"},
                                         {ID, "ID"},
                                         {AS, "AS"},
                                         {SET, "SET"},
                                         {HAS, "HAS"},
                                         {IF, "IF"},
                                         {THEN, "THEN"},
                                         {ELSE, "ELSE"},
                                         {STR_CONST, "STR_CONST"},
                                         {INT_CONST, "INT_CONST"},
                                         {BOOL_CONST, "BOOL_CONST"},
                                         {BELONG, "\'s"},
                                         {CHAIN, "->"},
                                         {GT, "gt"},
                                         {LT, "lt"},
                                         {GE, "ge"},
                                         {LE, "le"},
                                         {EQ, "eq"},
                                         {NE, "ne"},
                                         {ERROR, "ERROR"},
                                         {'+', "'+'"},
                                         {'-', "'-'"},
                                         {',', "','"},
                                         {';', "';'"},
                                         {'(', "'('"},
                                         {')', "')'"},
                                         {'[', "'['"},
                                         {']', "']'"}};

const char *token_to_string(int tok) {
  auto it = token_map.find(tok);
  if (it != token_map.end()) {
    return it->second;
  } else {
    return "<Invalid Token>";
  }
}
void print_escaped_string(ostream &str, const char *s) {
  while (*s) {
    switch (*s) {
    case '\\':
      str << "\\\\";
      break;
    case '\"':
      str << "\\\"";
      break;
    case '\n':
      str << "\\n";
      break;
    case '\t':
      str << "\\t";
      break;
    case '\b':
      str << "\\b";
      break;
    case '\f':
      str << "\\f";
      break;

    default:
      if (isprint(*s))
        str << *s;
      else
        str << '\\' << oct << std::setfill('0') << std::setw(3)
            << (int)((unsigned char)(*s)) << dec << std::setfill(' ');
      break;
    }
    s++;
  }
}
void print_token(int tok) {

  cerr << token_to_string(tok);

  switch (tok) {
  case (STR_CONST):
    cerr << " = ";
    cerr << " \"";
    print_escaped_string(cerr, yylval.symbol->get_string());
    cerr << "\"";
    break;
  case (INT_CONST):
    cerr << " = " << yylval.symbol->get_string();
    break;
  case (BOOL_CONST):
    cerr << (yylval.bool_val ? " = true" : " = false");
    break;
  case (ID):
    cerr << " = " << yylval.symbol->get_string();
    break;
  case (ERROR):
    cerr << " = ";
    print_escaped_string(cerr, yylval.error_msg);
    break;
  }
}

bool has_same_owner(Identifier *id1, Identifier *id2) {
  // Both two id has owner
  if (id1->has_owner() && id2->has_owner())
    return *(static_cast<Owner_Identifier *>(id1)->owner_name) ==
           *(static_cast<Owner_Identifier *>(id2)->owner_name);
  // // id2 has owner, compare id2's owner with id1
  // if (id2->has_owner())
  //   return *(static_cast<Owner_Identifier *>(id2)->owner_name) ==
  //          *(static_cast<Single_Identifier *>(id1)->name);

  // id1 has owner or both tow id does not have owner, return true
  return true;
}

Owner_Identifier *adjust_return_id(Identifier *id1, Identifier *id2) {
  // Assert id2's owner is same as id1's owner
  if (id2->has_owner())
    return static_cast<Owner_Identifier *>(id2);
  if (id1->has_owner())
    return new Owner_Identifier(
        static_cast<Owner_Identifier *>(id1)->owner_name,
        static_cast<Single_Identifier *>(id2)->name, id2->location);
  else
    return new Owner_Identifier(static_cast<Single_Identifier *>(id1)->name,
                                static_cast<Single_Identifier *>(id2)->name,
                                id2->location);
}

Owner_Identifier *adjust_return_id(Identifier *id) {
  // Input var -> output var's last_result
  // Input var's prop1 -> output var's last_result
  if (id->has_owner()) {
    auto *owner_id = static_cast<Owner_Identifier *>(id);
    return new Owner_Identifier(owner_id->owner_name, LAST_RESULT,
                                owner_id->location);
  } else {
    auto *sing_id = static_cast<Single_Identifier *>(id);
    return new Owner_Identifier(sing_id->name, LAST_RESULT, sing_id->location);
  }
}
