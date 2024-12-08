%{

#include <iostream>
#include <vector>
#include "symtab.h"
#include "AST.h"
#include "util.h"

extern char *curr_filename;

// YYLTYPE is defined in AST.h
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1

void warning(const char *s, YYLTYPE loc);
void yyerror(const char *s, YYLTYPE loc);
void yyerror(const char *s);

extern int yylex();

Program *ast_root;            /* the result of the parse  */
int omerrs = 0;               /* number of errors in lexing and parsing */

// Temp variables
std::vector<Expression *> *global_expr_list = new std::vector<Expression *>;
std::vector<Identifier *> *temp_identifier_list = new std::vector<Identifier *>;
std::vector<Call_Expr *> *temp_call_list = new std::vector<Call_Expr *>;
Identifier *temp_return_id;
Identifier *temp_owner;

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
          ast_root = new Program($1, YYLTYPE());
        }
        ;

expr_list : expression
          {
            global_expr_list->push_back($1);
          }
          | expr_list expression
          {
            global_expr_list->push_back($2);
          }
          ;

expression : decl_expr        { $$ = $1; }
           | property_decl_expr ;
           | assi_expr        { $$ = $1; }
           | io_expr          { $$ = $1; }
           | cond_expr        { $$ = $1; }
           | identifier       { $$ = $1; }
           | call_expr        ;
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

decl_expr : DEFINE identifier AS '(' expression ')'
          {
            $$ = new Var_Decl_Expr($2, $5, @2);
          }
          ;

property_decl_expr : identifier HAS '[' dummy_identifier_list ']'
                   {
                     if ($1->has_owner())
                       warning("Warning: cannot declare property of a property!", yylloc);
                     else {
                       for (Identifier *property_id : *temp_identifier_list) {
                         if (property_id->has_owner()) {
                           Owner_Identifier *oproperty_id = static_cast<Owner_Identifier *>(property_id);
                           if (!(*(oproperty_id->owner_name) == *(static_cast<Single_Identifier *>($1)->name))) { // Different owner
                             warning("Warning: cannot declare property with different owner!", yylloc);
                           } else { // Same owner
                             global_expr_list->push_back(new Property_Decl_Expr($1, oproperty_id->to_Identifier(), oproperty_id->location));
                           }
                           continue;
                         }
                         global_expr_list->push_back(new Property_Decl_Expr($1, property_id, property_id->location));
                       }
                     }
                   }
                   ;
              
dummy_identifier_list : identifier
                      {
                        temp_identifier_list->clear();
                        temp_identifier_list->push_back($1);
                      }    
                      | identifier ',' dummy_identifier_list 
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
            $$ = new Assi_Expr($2, $5, @2);
          }
          ;

parameter_list : expression
               {
                 $$ = new std::vector<Expression *>;
                 $$->push_back($1);
               }
               | expression ',' parameter_list
               {
                 $$ = $3;
                 $$->push_back($1);
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
            // size_t i is unsigned 03:3803:38integer so there is no negative
            for (size_t i = temp_call_list->size(); i > 0; i--) {
              Call_Expr *expr = temp_call_list->at(i - 1);
              if (expr->is_cond_call()) { // This expr is an function call with condition
                Cond_Call_Expr *cond_call_expr = static_cast<Cond_Call_Expr *>(expr);
                Direct_Call_Expr *direct_expr = cond_call_expr->call_expr;
                direct_expr->set_id(i == temp_call_list->size() ? caller : temp_return_id);
                if (!direct_expr->adjust_return_id())
                  warning("Warning: should not store result value in a property of another variable!", yylloc);
                temp_return_id = direct_expr->return_id;

                // Convert Cond_Call_Expr to Cond_Expr
                global_expr_list->push_back(new Cond_Expr(cond_call_expr->predictor, direct_expr, cond_call_expr->location));
              } else {
                Direct_Call_Expr *direct_expr = static_cast<Direct_Call_Expr *>(expr);
                direct_expr->set_id(i == temp_call_list->size() ? caller : temp_return_id);
                if (!direct_expr->adjust_return_id())
                  warning("Warning: should not store result value in a property of another variable!", yylloc);
                temp_return_id = direct_expr->return_id;
                global_expr_list->push_back(direct_expr);
              }
            }
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

func_expr : DO identifier
          {
            $$ = new Direct_Call_Expr($2, new std::vector<Expression *>, new Single_Identifier(new Symbol("last_result"), @2), @2);
          }
          | DO identifier USING '[' parameter_list ']'
          {
            $$ = new Direct_Call_Expr($2, $5, new Single_Identifier(new Symbol("last_result"), @2), @2);
          }
          | DO identifier ON identifier
          {
            $$ = new Direct_Call_Expr($2, new std::vector<Expression *>, $4, @2);
          }
          | DO identifier USING '[' parameter_list ']' ON identifier
          {
            $$ = new Direct_Call_Expr($2, $5, $8, @2);
          }
          ;

cond_expr : IF expression THEN expression ELSE expression ENDIF
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
          args->push_back($2);
          Direct_Call_Expr *call_expr = new Direct_Call_Expr($4, new Single_Identifier(new Symbol("ask"), @2), args, new Nil_Identifier(@2), @4);
          call_expr->adjust_return_id(); // last_result is not a property of another variable
          $$ = call_expr;
        }
        | ASK AS identifier
        {
          std::vector<Expression *> *args = new std::vector<Expression *>;
          Direct_Call_Expr *call_expr = new Direct_Call_Expr($3, new Single_Identifier(new Symbol("ask"), @3), args, new Nil_Identifier(@2), @3);
          call_expr->adjust_return_id(); // last_result is not a property of another variable
          $$ = call_expr;
        }
        | SAY '(' expression ')'
        {
          std::vector<Expression *> *args = new std::vector<Expression *>;
          args->push_back($3);
          Direct_Call_Expr *call_expr = new Direct_Call_Expr(new Nil_Identifier(@3), new Single_Identifier(new Symbol("say"), @3), args, new Nil_Identifier(@3), @3);
          $$ = call_expr;
        }
        ;

%%

void warning(const char *s, YYLTYPE loc) {
  std::cerr << curr_filename << ":" << loc.first_line << ":" << loc.first_column
            << ": " << s;
  std::cerr << std::endl;
}

void yyerror(const char *s) {
  yyerror(s, yylloc);
}

void yyerror(const char *s, YYLTYPE loc) {
  std::cerr << curr_filename << ":" << loc.first_line << ":" << loc.first_column
            << ": " << s << " at or near ";
  print_token(yychar);
  std::cerr << std::endl;
  omerrs++;

  if (omerrs > 50) {
    fprintf(stdout, "More than 50 errors\n");
    exit(1);
  }
}
