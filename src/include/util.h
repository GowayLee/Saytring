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
