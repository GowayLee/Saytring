#ifndef _UTIL_H_
#define _UTIL_H_

#include <iostream>
#include <ctype.h>
#include <iomanip>
#include "symtab.h"
#include "AST.h"

using namespace std;

extern char *token_to_string(int tok);
extern void print_token(int tok);
extern void print_escaped_string(std::ostream& str, const char *s);

#endif
