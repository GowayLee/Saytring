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
#include <iostream>
#include <vector>
#include "symtab.h"
#include "AST.h"
#include "util.h"

extern char *input_filename;
extern int syntax_error_count;
extern int syntax_warn_count;

// YYLTYPE is defined in AST.h
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1

void yywarn(const char *s, YYLTYPE loc);
void yyerror(const char *s, YYLTYPE loc);
void yyerror(const char *s);

void parse_funcs(Identifier *caller);

extern int yylex();

Program *ast_root;            /* the result of the parse  */

// Temp variables
// TODO: Can use stack to manage `has_pushed_back`
bool has_pushed_back = false;

std::vector<Expression *> *global_expr_list = new std::vector<Expression *>;
std::vector<Expression *> *temp_expr_list;
std::vector<Symbol *> *temp_identifier_list = new std::vector<Symbol *>;
std::vector<Call_Expr *> *temp_call_list = new std::vector<Call_Expr *>;
Identifier *temp_return_id;

%}

%locations

/* a union of all the types that can be the result of parsing actions. */
%union {
  bool bool_val;
  class Symbol *symbol;
  class Program *program;
  class Expression *expression;
  class Direct_Call_Expr *direct_call_expr;
  class Identifier *identifier;
  std::vector<Expression *> *expressions;
  char *error_msg;
}

/* Declaration of terminals */
%token CHAIN BELONG
%token GT LT GE LE EQ NE
%token DEFINE HAS SET AS IF THEN DO USING ON ELSE ENDIF CONVERT TO
%token ASK SAY
%token <symbol> STR_CONST INT_CONST TYPE_CONST
%token <bool_val> BOOL_CONST
%token <symbol> ID
%token ERROR

/* Declaration of non-terminals */
%type <program> program
%type <expression> expression
%type <identifier> identifier
%type <symbol> arith_op comp_op
%type <expressions> expr_list parameter_list then_expr_list else_expr_list

%type <expression> decl_expr property_decl_expr assi_expr io_expr cond_expr
%type <expression> call_expr cast_expr const_expr predictor_expr
%type <direct_call_expr> func_expr

/* Precedence declaration */
%nonassoc GT LT GE LE EQ NE
%right '+' '-'

%%

program : expr_list
        {
          ast_root = new Program(global_expr_list, YYLTYPE());
        }
        ;

expr_list : expression
          {
            // If current expressions has been pushed back before, don't push it again
            if (!has_pushed_back)
              global_expr_list->push_back($1);
            has_pushed_back = false;
          }
          | expr_list expression
          {
            if (!has_pushed_back)
              global_expr_list->push_back($2);
            has_pushed_back = false;
          }
          ;

expression : assi_expr        { $$ = $1; }
           | io_expr          { $$ = $1; }
           | cond_expr        { $$ = $1; }
           | identifier       { $$ = $1; }
           | cast_expr        { $$ = $1; }
           | const_expr       { $$ = $1; }
           | decl_expr          ;
           | property_decl_expr ;
           | call_expr          ;
           | expression comp_op expression ';'
           {
             if (!has_pushed_back)
               $$ = new Comp_Expr($1, $2, $3, @1);
             else {
               $$ = new Comp_Expr($1, $2, temp_return_id, @1);
               has_pushed_back = false;
             }
           }
           | expression arith_op expression ';'
           {
             if (!has_pushed_back)
               $$ = new Arith_Expr($1, $2, $3, @1);
             else {
               $$ = new Arith_Expr($1, $2, temp_return_id, @1);
               has_pushed_back = false;
             }
           }
           | error '\n'
           {
             yyerror("Error in expression", yylloc);
             yyerrok;
           }
           ;

identifier : ID
           {
             $$ = new Single_Identifier($1, @1);
           }
           | ID BELONG ID
           {
             $$ = new Owner_Identifier($1, $3, @3);
           }
           ;

const_expr : INT_CONST
           {
             $$ = new Int_Const_Expr($1, @1);
           }
           | STR_CONST
           {
             $$ = new String_Const_Expr($1, @1);
           }
           | BOOL_CONST
           {
             $$ = new Bool_Const_Expr($1, @1);
           }
           ;

decl_expr : DEFINE ID AS '(' expression ')'
          {
            global_expr_list->push_back(new Var_Decl_Expr($2, $5, @2));

            // Automatically declare var's last_result, as well
            global_expr_list->push_back(new Property_Decl_Expr(new Single_Identifier($2, @2), LAST_RESULT, @2));
            has_pushed_back = true;
          }
          | DEFINE error ')'
          {
            yyerror("Error in variable declaration", yylloc);
            yyerrok;
          }
          ;

property_decl_expr : identifier HAS '[' dummy_identifier_list ']'
                   {
                     if ($1->has_owner())
                       yywarn("Cannot declare property of a property!", @1);
                     else {
                       for (Symbol *property_name : *temp_identifier_list) {
                         global_expr_list->push_back(new Property_Decl_Expr($1, property_name, yylloc));
                       }
                       has_pushed_back = true;
                     }
                   }
                   | error ']'
                   {
                     yyerror("Error in property declaration", yylloc);
                     yyerrok;
                   }
                   ;

// TODO: fix order of collecting
dummy_identifier_list : ID
                      {
                        temp_identifier_list->clear();
                        temp_identifier_list->push_back($1);
                      }
                      | ID ',' dummy_identifier_list
                      {
                        temp_identifier_list->push_back($1);
                      }
                      | error ','
                      {
                        yyerror("Error in property declaration", yylloc);
                        yyerrok;
                      }
                      ;

assi_expr : SET identifier AS '(' expression ')'
          {
            if (!has_pushed_back)
              $$ = new Assi_Expr($2, $5, @2);
            else {
              $$ = new Assi_Expr($2, temp_return_id, @2);
              has_pushed_back = false;
            }
          }
          | SET error ')'
          {
            yyerror("Error in assignment", yylloc);
            yyerrok;
          }
          ;

cast_expr : CONVERT identifier TO TYPE_CONST ON identifier
          {
            $$ = new Cast_Expr($2, $4, $6, @2);
          }
          | CONVERT identifier TO TYPE_CONST
          {
            $$ = new Cast_Expr($2, $4, adjust_return_id($2), @2);
          }
          ;

// Will collect parameters in inverse order
// TODO: fix order of collecting
parameter_list : expression
               {
                 $$ = new std::vector<Expression *>;
                 if (!has_pushed_back)
                   $$->push_back($1);
                 else {
                   $$->push_back(temp_return_id);
                   has_pushed_back = false;
                 }
               }
               | expression ',' parameter_list
               {
                 $$ = $3;
                 if (!has_pushed_back)
                   $$->push_back($1);
                 else {
                   $$->push_back(temp_return_id);
                   has_pushed_back = false;
                 }
               }
               | error ','
               {
                 yyerror("Error in parameter list", yylloc);
                 yyerrok;
               }
               ;

 /*
  * Call_expr, as well as parse Chain call
  * Convert chain_call into plain call expressions
  */
call_expr : identifier dummy_chain_call_list
          {
            parse_funcs($1);
          }
          | const_expr dummy_chain_call_list
          {
            Identifier *anony_caller = new Single_Identifier(_anonymous, @1);
            global_expr_list->push_back(new Assi_Expr(anony_caller, $1, @1));
            parse_funcs(anony_caller);
          }
          ;

dummy_chain_call_list : func_expr
                      {
                        temp_call_list->clear();
                        temp_call_list->push_back($1);
                      }
                      | func_expr CHAIN dummy_chain_call_list
                      {
                        temp_call_list->push_back($1);
                      }
                      | '(' IF expression THEN func_expr ')'
                      {
                        temp_call_list->clear();
                        if (!has_pushed_back)
                          temp_call_list->push_back(new Cond_Call_Expr($3, $5, @3));
                        else {
                          yywarn("Nested function call should not appear in chain call", @3);
                          temp_call_list->push_back(new Cond_Call_Expr(temp_return_id, $5, @3));
                          has_pushed_back = false;
                        }
                      }
                      | '(' IF expression THEN func_expr ')' CHAIN dummy_chain_call_list
                      {
                        if (!has_pushed_back)
                          temp_call_list->push_back(new Cond_Call_Expr($3, $5, @3));
                        else {
                          yywarn("Nested function call should not appear in chain call", @3);
                          temp_call_list->push_back(new Cond_Call_Expr(temp_return_id, $5, @3));
                          has_pushed_back = false;
                        }
                      }
                      | CHAIN error
                      {
                        yyerror("Error in chain call", yylloc);
                        yyerrok;
                      }
                      ;

func_expr : DO ID
          {
            $$ = new Direct_Call_Expr($2, new std::vector<Expression *>, new Single_Identifier(LAST_RESULT , @2), @2);
          }
          | DO ID USING '[' parameter_list ']'
          {
            $$ = new Direct_Call_Expr($2, $5, new Single_Identifier(LAST_RESULT, @2), @2);
          }
          | DO ID ON identifier
          {
            $$ = new Direct_Call_Expr($2, new std::vector<Expression *>, $4, @2);
          }
          | DO ID USING '[' parameter_list ']' ON identifier
          {
            $$ = new Direct_Call_Expr($2, $5, $8, @2);
          }
          ;

predictor_expr : IF expression
               {
                 if (!has_pushed_back)
                   $$ = $2;
                 else {
                   $$ = temp_return_id;
                   has_pushed_back = false;
                 }
               }
               ;

then_expr_list : THEN expression
               {
                 temp_expr_list = new std::vector<Expression *>;
                 $$ = temp_expr_list;
                 if (!has_pushed_back)
                   $$->push_back($2);
                 has_pushed_back = false;
               }
               | then_expr_list expression
               {
                 if (!has_pushed_back)
                   temp_expr_list->push_back($2);
                 has_pushed_back = false;
               }
               ;

else_expr_list : ELSE expression
               {
                 temp_expr_list = new std::vector<Expression *>;
                 $$ = temp_expr_list;
                 if (!has_pushed_back)
                   $$->push_back($2);
                 has_pushed_back = false;
               }
               | else_expr_list expression
               {
                 if (!has_pushed_back)
                   temp_expr_list->push_back($2);
                 has_pushed_back = false;
               }
               ;

cond_expr : predictor_expr then_expr_list else_expr_list ENDIF
          {
            $$ = new Cond_Expr($1, $2, $3, @1);
          }
          | predictor_expr then_expr_list ENDIF
          {
            $$ = new Cond_Expr($1, $2, @1);
          }
          | error ENDIF
          {
            yyerror("Error in conditional expression", yylloc);
            yyerrok;
          }
          ;

comp_op : GT
        {
          $$ = _GT;
        }
        | LT
        {
          $$ = _LT;
        }
        | GE
        {
          $$ = _GE;
        }
        | LE
        {
          $$ = _LE;
        }
        | EQ
        {
          $$ = _EQ;
        }
        | NE
        {
          $$ = _NE;
        }
        ;

arith_op : '+'
         {
           $$ = _ADD;
         }
         | '-'
         {
           $$ = _SUB;
         }
         ;

 /*
  *   Syntactic Sugars
  */

io_expr : ASK expression AS identifier
        {
          std::vector<Expression *> *args = new std::vector<Expression *>;
          if (!has_pushed_back)
            args->push_back($2);
          else {
            args->push_back(temp_return_id);
            has_pushed_back = false;
          }
          Direct_Call_Expr *call_expr = new Direct_Call_Expr(new Nil_Identifier(@2), id_tab->add_string("ask_with_prompt"), args, $4, @4);
          $$ = call_expr;
        }
        | ASK AS identifier
        {
          std::vector<Expression *> *args = new std::vector<Expression *>;
          Direct_Call_Expr *call_expr = new Direct_Call_Expr(new Nil_Identifier(@2), id_tab->add_string("ask"), args, $3, @3);
          $$ = call_expr;
        }
        | SAY '(' expression ')'
        {
          std::vector<Expression *> *args = new std::vector<Expression *>;
          if (!has_pushed_back)
            args->push_back($3);
          else {
            args->push_back(temp_return_id);
            has_pushed_back = false;
          }
          Direct_Call_Expr *call_expr = new Direct_Call_Expr(new Nil_Identifier(@3), id_tab->add_string("say"), args, new Nil_Identifier(@3), @3);
          $$ = call_expr;
        }
        ;

%%

void yywarn(const char *s, YYLTYPE loc) {
  std::cerr << input_filename << ":" << loc.first_line << ":" << loc.first_column
            << ": Warning: " << s;
  std::cerr << std::endl;
  syntax_warn_count++;
}

void yyerror(const char *s) {
  yyerror(s, yylloc);
}

void yyerror(const char *s, YYLTYPE loc) {
  std::cerr << input_filename << ":" << loc.first_line << ":" << loc.first_column
            << ": " << s << " at or near ";
  print_token(yychar);
  std::cerr << std::endl;
  syntax_error_count++;
}

void parse_funcs(Identifier *caller) {
  // dummy_chain_call_list collects call_expr in inverse order.
  for (size_t i = temp_call_list->size(); i > 0; i--) {
    Call_Expr *expr = temp_call_list->at(i - 1);
    if (expr->is_cond_call()) { // This expr is a function call with condition
      Cond_Call_Expr *cond_call_expr = static_cast<Cond_Call_Expr *>(expr);
      Direct_Call_Expr *direct_expr = cond_call_expr->call_expr;

      // Pass previous call's return_id to current call's id
      direct_expr->id = (i == temp_call_list->size() ? caller : temp_return_id);

      // Check owner and property relationship
      if (!has_same_owner(direct_expr->id, direct_expr->return_id))
        yywarn("Should not store result value of a function called by a property in another property belongs to another variable!", yylloc);

      direct_expr->return_id = adjust_return_id(direct_expr->id, direct_expr->return_id);
      temp_return_id = direct_expr->return_id;

      // Convert Cond_Call_Expr to Cond_Expr
      auto then_list = new std::vector<Expression *>;
      then_list->push_back(direct_expr);
      global_expr_list->push_back(new Cond_Expr(cond_call_expr->predictor, then_list, cond_call_expr->location));
    } else {
      Direct_Call_Expr *direct_expr = static_cast<Direct_Call_Expr *>(expr);

      // Pass previous call's return_id to current call's id
      direct_expr->id = (i == temp_call_list->size() ? caller : temp_return_id);

      // Check owner and property relationship
      if (!has_same_owner(direct_expr->id, direct_expr->return_id))
      yywarn("Should not store result value of a function called by a property in another property belongs to another variable!", yylloc);

      direct_expr->return_id = adjust_return_id(direct_expr->id, direct_expr->return_id);
      temp_return_id = direct_expr->return_id;
      global_expr_list->push_back(direct_expr);
    }
  }
  has_pushed_back = true;
}