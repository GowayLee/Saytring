%{

/*
  Saytring Compiler. A compiler translating Saytring to Python.
  Copyright (C) 2024 Haoyuan Li

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
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

/* Reset string buffer once detect \"
 *  and ready to record string as stream.
 */
void reset_string_buf() {
	string_buf[0] = '\0';
	string_buf_ptr = string_buf;
}

bool append_char() {
	*string_buf_ptr = yytext[0];

  // Check if the string is too long
  if (++string_buf_ptr >= MAX_STR_BUF_LOC) {
    yylval.error_msg = "String constant too long";
    return false;
  }
  return true;
}

// YYLTYPE is defined in AST.h
// Record column number
extern int yylineno;
extern int yycolumn;

int yycolumn = 1;

%}

%option noyywrap

%{
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yycolumn += yyleng; \
    yylloc.last_column = yycolumn - 1;
%}

%x INLINE_COMMENT NORMAL_STRING_CONST ERROR_STRING_CONST ESCAPE_CHAR

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
  *  Type constants
  */
"string"                                              {
  yylval.symbol = _string;
  return TYPE_CONST;
}

"int"                                                 {
  yylval.symbol = _int;
  return TYPE_CONST;
}

"list"                                                {
  yylval.symbol = _list;
  return TYPE_CONST;
}

"bool"                                                {
  yylval.symbol = _bool;
  return TYPE_CONST;
} 

 /*
  *  Keyword
  */
(?i:d)efine                                           return DEFINE;
(?i:i)f                                               return IF;
(?i:a)sk                                              return ASK;
(?i:s)et                                              return SET;
(?i:s)ay                                              return SAY;
(?i:c)onvert                                          return CONVERT;
"as"                                                  return AS;
"has"                                                 return HAS;
"do"                                                  return DO;
"using"                                               return USING;
"on"                                                  return ON;
"then"                                                return THEN;
"else"                                                return ELSE;
"endif"                                               return ENDIF;
"to"                                                  return TO;

 /*
  *  Number
  */
{NUMBER}                                              {
	yylval.symbol = int_tab->add_string(yytext);
  return INT_CONST;
}

 /*
  *  Identifier
  */
{ID}                                                  {
  yylval.symbol = id_tab->add_string(yytext);
  return ID;
}

 /*
  *  String constants (C syntax)
  */
"\""                                                  {
	BEGIN(NORMAL_STRING_CONST);
	reset_string_buf();
}

 /*
  *  Append "\\" to str_buf, only filter "\n" after escaping
  */
<NORMAL_STRING_CONST>"\\"                             {
  if (append_char())
    BEGIN(NORMAL_STRING_CONST);
  else {
    BEGIN(ERROR_STRING_CONST);
    return ERROR;
  }
  BEGIN(ESCAPE_CHAR);
}
<NORMAL_STRING_CONST>"\n"                             {
  yylineno++;
  yycolumn = 1;
  // Assume the programmer simply forget the close-quote.
  BEGIN(ERROR_STRING_CONST);
  yylval.error_msg = "Unterminated string constant";
  return ERROR;
}
<NORMAL_STRING_CONST>"\""                             {
  *string_buf_ptr = '\0';
  BEGIN(INITIAL);
  yylval.symbol = str_tab->add_string(string_buf);
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
  if (append_char())
    BEGIN(NORMAL_STRING_CONST);
  else {
    BEGIN(ERROR_STRING_CONST);
    return ERROR;
  }
}

<ESCAPE_CHAR>"\n"                                     {
  yylineno++;
  yycolumn = 1;
  BEGIN(NORMAL_STRING_CONST);
}
<ESCAPE_CHAR>.                                        {
  if (append_char())
    BEGIN(NORMAL_STRING_CONST);
  else {
    BEGIN(ERROR_STRING_CONST);
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
