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
