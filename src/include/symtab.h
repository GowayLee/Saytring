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

// Predefined symbols, basic types in Saytring
extern Symbol *_string, *_int, *_list, *_bool, *NULL_Type, *ERR_Type,
    *LAST_RESULT;

#endif
