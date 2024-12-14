#include "util.h"
#include "AST.h"
#include "parser.tab.h"
#include "symtab.h"
#include <ctype.h>
#include <iomanip>
#include <iostream>

char *token_to_string(int tok) {
  switch (tok) {
  case 0:
    return ("EOF");
    break;
  case (DEFINE):
    return ("DEFINE");
    break;
  case (USING):
    return ("USING");
    break;
  case (ON):
    return ("ON");
    break;
  case (ENDIF):
    return ("ENDIF");
    break;
  case (ASK):
    return ("ASK");
    break;
  case (SAY):
    return ("SAY");
    break;
  case (DO):
    return ("DO");
    break;
  case (ID):
    return ("ID");
    break;
  case (AS):
    return ("AS");
    break;
  case (SET):
    return ("SET");
    break;
  case (HAS):
    return ("HAS");
    break;
  case (IF):
    return ("IF");
    break;
  case (THEN):
    return ("THEN");
    break;
  case (ELSE):
    return ("ELSE");
    break;
  case (STR_CONST):
    return ("STR_CONST");
    break;
  case (INT_CONST):
    return ("INT_CONST");
    break;
  case (BOOL_CONST):
    return ("BOOL_CONST");
    break;
  case (BELONG):
    return ("\'s");
    break;
  case (CHAIN):
    return ("->");
    break;
  case (GT):
    return ("gt");
    break;
  case (LT):
    return ("lt");
    break;
  case (GE):
    return ("ge");
    break;
  case (LE):
    return ("le");
    break;
  case (EQ):
    return ("eq");
    break;
  case (NE):
    return ("ne");
    break;
  case (ERROR):
    return ("ERROR");
    break;
  case '+':
    return ("'+'");
    break;
  case '-':
    return ("'-'");
    break;
  case ',':
    return ("','");
    break;
  case ';':
    return ("';'");
    break;
  case '(':
    return ("'('");
    break;
  case ')':
    return ("')'");
    break;
  case '[':
    return ("'['");
    break;
  case ']':
    return ("']'");
    break;
  default:
    return ("<Invalid Token>");
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
  // id2 has owner, compare id2's owner with id1
  if (id2->has_owner())
    return *(static_cast<Owner_Identifier *>(id2)->owner_name) ==
           *(static_cast<Single_Identifier *>(id1)->name);

  // id1 has owner or both tow id does not have owner, return true
  return true;
}

Owner_Identifier *adjust_return_id(Identifier *id1, Identifier *id2) {
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
