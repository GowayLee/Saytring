%{

#include <vector>
#include "parser.tab.h"
#include "symtab.h"

// #define YY_NO_UNPUT   /* keep g++ happy */

extern YYSTYPE yylval;
extern YYLTYPE yylloc;

/* Max size of string constants */
#define MAX_STR_CONST 1025
char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

// String relavted
#define MAX_STR_BUF_LOC string_buf + MAX_STR_CONST - 1
bool is_escape = false;

/* Reset string buffer once detect \"
 *  and ready to record string as stream.
 */
void reset_string_buf() {
	string_buf[0] = '\0';
	string_buf_ptr = string_buf;
	is_escape = false;
}

// YYLTYPE is defined in AST.h
// Record column number
extern int yylineno;
extern int yycolumn;

int yycolumn = 1;

// debug

%}

%option noyywrap

%{
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yycolumn += yyleng; \
    yylloc.last_column = yycolumn - 1;
%}

%x INLINE_COMMENT NORMAL_STRING_CONST ERROR_STRING_CONST

DELIME_EXNL  	                                        [ \t\f\v\r]
NUMBER                                                -?[0-9]+
ID                                                    [a-z][a-zA-Z0-9_]*
SINGLE_CHAR_OPTER                                     [-+,;()\[\]]
BELONG                                                "\'s"

CHAIN                                                 ->
GT                                                    gt
LT                                                    lt
GE                                                    ge
LE                                                    le
EQ                                                    eq
NE                                                    ne

%%

 /*
  * Inline Comment
  */
"#"                                                   BEGIN(INLINE_COMMENT);
<INLINE_COMMENT>"\n"                                  {
  yylineno++;
  yycolumn = 1;
  BEGIN(INITIAL);
}
<INLINE_COMMENT>.                                     ;

 /*
  *  White spaces
  */
"\n"                                                  yylineno++; yycolumn = 1;
{DELIME_EXNL}+                                        ;

 /*
  * Multi-character operator
  */
{CHAIN}                                               return CHAIN;
{BELONG}                                              return BELONG;
{GT}                                                  return GT;
{LT}                                                  return LT;
{GE}                                                  return GE;
{LE}                                                  return LE;
{EQ}                                                  return EQ;
{NE}                                                  return NE;

 /*
  *  Single-character operator
  */
{SINGLE_CHAR_OPTER}                                   return yytext[0];

 /*
  *  Boolean
  */
true                                                  {
  yylval.bool_val = true;
  return BOOL_CONST;
}
false                                                 {
  yylval.bool_val = false;
  return BOOL_CONST;
}

 /*
  *  Keyword
  */
(?i:d)efine                                           return DEFINE;
(?i:i)f                                               return IF;
(?i:a)sk                                              return ASK;
(?i:s)et                                              return SET;
(?i:s)ay                                              return SAY;
"as"                                                  return AS;
"has"                                                 return HAS;
"do"                                                  return DO;
"using"                                               return USING;
"on"                                                  return ON;
"then"                                                return THEN;
"else"                                                return ELSE;
"endif"                                               return ENDIF;

 /*
  *  Number
  */
{NUMBER}                                              {
	yylval.symbol = new Symbol(yytext);
  return INT_CONST;
}

 /*
  *  Identifier
  */
{ID}                                                  {
  yylval.symbol = new Symbol(yytext);
  return ID;
}

 /*
  *  String constants (C syntax)
  */
"\""                                                  {
	BEGIN(NORMAL_STRING_CONST);
	reset_string_buf();
}
<NORMAL_STRING_CONST>"\\"                             is_escape = true;
<NORMAL_STRING_CONST>"\n"                             {
  yylineno++;
  yycolumn = 1;
  if (is_escape) {
    is_escape = false;
  } else {
    // Assume the programmer simply forget the close-quote.
    BEGIN(ERROR_STRING_CONST);
    yylval.error_msg = "Unterminated string constant";
    return ERROR;
  }
}
<NORMAL_STRING_CONST>"\""                             {
  *string_buf_ptr = '\0';
  BEGIN(INITIAL);
	yylval.symbol = new Symbol(string_buf);
	return STR_CONST;
}
<NORMAL_STRING_CONST>[^\0]*\0                         {
  BEGIN(ERROR_STRING_CONST);
  yylval.error_msg = "String contains null character";
  return ERROR;
}
<NORMAL_STRING_CONST><<EOF>>                          {
  BEGIN(INITIAL);
  yylval.error_msg = "EOF in string constant";
  return ERROR;
}
<NORMAL_STRING_CONST>.                                {
	*string_buf_ptr = yytext[0];

  // Check if the string is too long
  if (++string_buf_ptr >= MAX_STR_BUF_LOC) {
    BEGIN(ERROR_STRING_CONST);
    yylval.error_msg = "String constant too long";
    return ERROR;
  }
}

<ERROR_STRING_CONST>"\""                              BEGIN(INITIAL);
<ERROR_STRING_CONST>"\n"                              yylineno++; BEGIN(INITIAL);
<ERROR_STRING_CONST>.                                 ;

.                                                     {
  yylval.error_msg = yytext;
  return ERROR;
}

%%
