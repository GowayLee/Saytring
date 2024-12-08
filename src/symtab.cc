#include "symtab.h"
#include <cstring>

Symbol::Symbol(char* s) {
  int len = (int) strlen(s);  // store length of string without '\0', help to add '\0'
  str = new char [len+1];
  strncpy(str, s, len);
  str[len] = '\0';
}

bool Symbol::operator==(Symbol other) {
  return std::strcmp(str, other.str) == 0;
}
