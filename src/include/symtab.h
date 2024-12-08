#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <iostream>
#include <string.h>

class Symbol {
protected:
  char *str; // the string
public:
  Symbol(char *s);

  int check;

  // std::ostream &print(std::ostream &s) const;

  bool operator==(Symbol other);

  // Return the str and len components of the Entry.
  char *get_string() const { return str; }
};

#endif
