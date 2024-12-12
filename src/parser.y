%{

#include <iostream>
#include <vector>
#include "symtab.h"
#include "AST.h"
#include "util.h"

extern char *input_filename;

// YYLTYPE is defined in AST.h
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1

void warning(const char *s, YYLTYPE loc);
void yyerror(const char *s, YYLTYPE loc);
void yyerror(const char *s);

extern int yylex();

Program *ast_root;            /* the result of the parse  */

// Temp variables
bool has_pushed_back = false;
std::vector<Expression *> *global_expr_list = new std::vector<Expression *>;
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
%token DEFINE HAS SET AS IF THEN DO USING ON ELSE ENDIF
%token ASK SAY
%token <symbol> STR_CONST INT_CONST
%token <bool_val> BOOL_CONST
%token <symbol> ID
%token ERROR

/* Declaration of non-terminals */
%type <program> program
%type <expression> expression
%type <identifier> identifier
%type <symbol> arith_op comp_op
%type <expressions> expr_list parameter_list

%type <expression> decl_expr property_decl_expr assi_expr io_expr cond_expr
%type <expression> call_expr
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
           | decl_expr          ;
           | property_decl_expr ;
           | call_expr          ;
           | expression comp_op expression ';'
           {
             $$ = new Comp_Expr($1, $2, $3, @1);
           }
           | expression arith_op expression ';'
           {
             $$ = new Arith_Expr($1, $2, $3, @1);
           }
           | INT_CONST
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

decl_expr : DEFINE ID AS '(' expression ')'
          {  
            global_expr_list->push_back(new Var_Decl_Expr($2, $5, @2));

            // Automatically declare var's last_result, as well
            global_expr_list->push_back(new Property_Decl_Expr(new Single_Identifier($2, @2), id_tab->add_string("last_result"), @2));
            has_pushed_back = true;
          }
          ;

property_decl_expr : identifier HAS '[' dummy_identifier_list ']'
                   {
                     if ($1->has_owner())
                       warning("Cannot declare property of a property!", yylloc);
                     else {
                       for (Symbol *property_name : *temp_identifier_list) {
                         global_expr_list->push_back(new Property_Decl_Expr($1, property_name, yylloc));
                       }
                       has_pushed_back = true;
                     }
                   }
                   ;
              
dummy_identifier_list :ID 
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
          ;

// Will collect parameters in inverse order
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
            Identifier *caller = $1;

            // dummy_chain_call_list collects call_expr in inverse order.
            // size_t i is unsigned integer so there is no negative
            for (size_t i = temp_call_list->size(); i > 0; i--) {
              Call_Expr *expr = temp_call_list->at(i - 1);
              if (expr->is_cond_call()) { // This expr is an function call with condition
                Cond_Call_Expr *cond_call_expr = static_cast<Cond_Call_Expr *>(expr);
                Direct_Call_Expr *direct_expr = cond_call_expr->call_expr;

                // Pass prev call's return_id to current call's id
                direct_expr->set_id(i == temp_call_list->size() ? caller : temp_return_id);

                // Check owner and property relationship
                if (!direct_expr->adjust_return_id())
                  warning("Should not store result value in a property of another variable!", yylloc);

                // Continue pass down return_id
                temp_return_id = direct_expr->return_id;

                // Convert Cond_Call_Expr to Cond_Expr
                global_expr_list->push_back(new Cond_Expr(cond_call_expr->predictor, direct_expr, cond_call_expr->location));
              } else {
                Direct_Call_Expr *direct_expr = static_cast<Direct_Call_Expr *>(expr);

                // Pass prev call's return_id to current call's id
                direct_expr->set_id(i == temp_call_list->size() ? caller : temp_return_id);

                // Check owner and property relationship
                if (!direct_expr->adjust_return_id())
                  warning("Should not store result value in a property of another variable!", yylloc);
                temp_return_id = direct_expr->return_id;
                global_expr_list->push_back(direct_expr);
              }
            }
            has_pushed_back = true;
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
                        temp_call_list->push_back(new Cond_Call_Expr($3, $5, @3));
                      }
                      | '(' IF expression THEN func_expr ')' CHAIN dummy_chain_call_list
                      {
                        temp_call_list->push_back(new Cond_Call_Expr($3, $5, @3));
                      }
                      | CHAIN error 
                      {
                        yyerror("Error in chain call", yylloc);
                        yyerrok;
                      }
                      ;

func_expr : DO ID
          {
            $$ = new Direct_Call_Expr($2, new std::vector<Expression *>, new Single_Identifier(id_tab->add_string("last_result"), @2), @2);
          }
          | DO ID USING '[' parameter_list ']'
          {
            $$ = new Direct_Call_Expr($2, $5, new Single_Identifier(id_tab->add_string("last_result"), @2), @2);
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

cond_expr : IF expression THEN expression ELSE expression ENDIF
          {
            $$ = new Cond_Expr($2, $4, $6, @2);
          }
          | IF expression THEN expression ENDIF
          {
            $$ = new Cond_Expr($2, $4, @2);
          }
          ;

comp_op : GT
        {
          $$ = new Symbol("GT");
        }
        | LT
        {
          $$ = new Symbol("LT");
        }
        | GE
        {
          $$ = new Symbol("GE");
        }
        | LE
        {
          $$ = new Symbol("LE");
        }
        | EQ
        {
          $$ = new Symbol("EQ");
        }
        | NE
        {
          $$ = new Symbol("NE");
        }
        ;

arith_op : '+'
         {
           $$ = new Symbol("ADD");
         }
         | '-'
         {
           $$ = new Symbol("SUB");
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

void warning(const char *s, YYLTYPE loc) {
  std::cerr << input_filename << ":" << loc.first_line << ":" << loc.first_column
            << ": Warning: " << s;
  std::cerr << std::endl;
}

void yyerror(const char *s) {
  yyerror(s, yylloc);
}

void yyerror(const char *s, YYLTYPE loc) {
  std::cerr << input_filename << ":" << loc.first_line << ":" << loc.first_column
            << ": " << s << " at or near ";
  print_token(yychar);
  std::cerr << std::endl;
}
