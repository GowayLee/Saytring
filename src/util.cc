#include "util.h"
#include "parser.tab.h"

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
