#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <cstring>
#include <iostream>
#include <string.h>

class Symbol {
protected:
  char *str; // the string
public:
  Symbol(char *s);

  std::ostream &print(std::ostream &s) const;

  bool operator==(Symbol other) {
    return std::strcmp(str, other.str) == 0;
  }

  // Return the str and len components of the Entry.
  char *get_string() const { return str; }
};

#endif
