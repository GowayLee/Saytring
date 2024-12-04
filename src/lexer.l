%{

#include <stdlib.h> /* provide atoi */
#include "parser.tab.h"
#include "symtab.h"
#include "AST.h"

#define YY_NO_UNPUT   /* keep g++ happy */

extern YYSTYPE yylval;

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
#define YY_USER_ACTION update_position();

int prev_lineno = 1,
    prev_yyleng = yyleng;

void update_position() {
  if (yytext[0] == '\0') return;
  if (prev_lineno != yylloc.line) {
    yylloc.column = 0;
    prev_lineno = yylloc.column;
  }
  yylloc.column += prev_yyleng;
  prev_yyleng = yyleng;
}

%}

%option noyywrap

%x INLINE_COMMENT NORMAL_STRING_CONST ERROR_STRING_CONST

DELIME_EXNL  	                                        [ \t\f\v\r]
NUMBER		                                            -?[0-9]+
ID		                                                [a-z][a-zA-Z0-9_]*
SINGLE_CHAR_OPTER		                                  [-+,]
BELONG                                                "\'s"

CHAIN                                                 ->
GT                                                    gt
LT                                                    lt
GE                                                    ge
LE                                                    le
EQ                                                    eq

%%

 /*
  * Inline Comment
  */
"#"                                                   BEGIN(INLINE_COMMENT);
<INLINE_COMMENT>"\n"                                  {
  yylloc.line++;
  BEGIN(INITIAL);
}
<INLINE_COMMENT>.                                     ;

 /*
  *  White spaces
  */
"\n"                                                  yylloc.line++;
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

 /*
  *  Single-character operator
  */
{SINGLE_CHAR_OPTER}                                   return yytext[0];

 /*
  *  Boolean
  */
true                                                  {

  return TRUE;
}
false                                                 {

  return FALSE;
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

 /*
  *  Number
  */
{NUMBER}                                              {

  return NUM;
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
  yylloc.line++;
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
	// yylval.symbol = stringtable.add_string(string_buf);
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
<ERROR_STRING_CONST>"\n"                              yylloc.line++; BEGIN(INITIAL);
<ERROR_STRING_CONST>.                                 ;

.                                                     {
  yylval.error_msg = yytext;
  return ERROR;
}

%%
