%{

#include <iostream>
#include <vector>
#include "symtab.h"
#include "AST.h"

extern char *curr_filename;

// YYLTYPE is defined in AST.h
extern YYLTYPE yylloc;
void yyerror(char *s, YYLTYPE loc);
#define YYLTYPE_IS_DECLARED 1

extern int yylex();

Program *ast_root;            /* the result of the parse  */
int omerrs = 0;               /* number of errors in lexing and parsing */

// Temp variables
std::vector<expression *> global_expr_list;
Identifier *temp_return_id;
Identifier *temp_owner;

%}

/* a union of all the types that can be the result of parsing actions. */
%union {
  bool bool_val;
  Symbol *symbol;
  Program *program;
  Expression *expression;
  Identifier *identifier;
  std::vector<Expression *> expressions;
  char *error_msg;
}

/* Declaration of terminals */
%token CHAIN BELONG
%token GT LT GE LE EQ NE
%token DEFINE HAS SET AS IF THEN DO USING ON
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
%type <expression> func_expr call_expr chain_call_expr

/* Precedence declaration */
%left '+' '-'
%left GT LT GE LE EQ NE
%left BELONG
%left CHAIN

%%

program : expr_list
        {
          ast_root = new Program($1, YYLTYPE());
        }
        ;

expr_list : expression
          {
            $$ = new std::vector<Expression *>;
            global_expr_list = $$;
            $$->push_back($1);
          }
          | expr_list expression
          {
            $$ = $1;
            $$->push_back($2);
          }
          ;

expression : decl_expr        { $$ = $1; }
           | assi_expr        { $$ = $1; }
           | io_expr          { $$ = $1; }
           | cond_expr        { $$ = $1; }
           | call_expr        ;
           | expression comp_op expression
           {
             $$ = new Comp_Expr($1, $2, $3, @1);
           }
           | expression arith_op expression
           {
             $$ = new Arith_Expr($1, $2, $3, @1);
           }
           | INT_CONST
           {
             $$ = new Int_Const_Expr($1, @1);
           }
           | STR_CONST
           {
             $$ = new Str_Const_Expr($1, @1);
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
             $$ = new Identifier($1, @1);
           }
           | ID BELONG ID
           {
             $$ = new Owner_Identifier($1, $3, @3);
           }
           ;

decl_expr : DEFINE identifier
          {
            $$ = new Var_Decl_Expr($2, new Nil_Expr(@2), @2);
          }
          | DEFINE identifier AS expression
          {
            $$ = new Var_Decl_Expr($2, $4, @2);
          }
          | property_decl_expr
          ;

property_decl_expr : identifier HAS identifier
                   {
                     temp_owner = $1;
                     if ($1->has_owner())
                       yyerror("Warning: cannot declare property of a property!", yylloc);
                     else {
                       $$ = new Property_Decl_Expr($1, $3, @3);
                     }
                   }
                   | property_decl_expr ',' identifier
                   {
                     if (temp_owner->has_owner())
                       yyerror("Warning: cannot declare property of a property!", yylloc);
                     else {
                       $$ = new Property_Decl_Expr($1, $3, @3);
                     }
                   }
                   | error ','
                   {
                     yyerror("Error in property declaration", yylloc);
                     yyerrok;
                   }
                   ;

assi_expr : SET identifier AS expression
          {
            $$ = new Assi_Expr($2, $4, @2);
          }
          ;

parameter_list : expression
               {
                 $$ = new std::vector<expression *>;
                 $$->push_back($1);
               }
               | parameter_list ',' expression
               {
                 $$ = $1;
                 $$->push_back($3);
               }
               | error ','
               {
                 yyerror("Error in parameter list", yylloc);
                 yyerrok;
               }
               ;

 /*
  * Call_expr, as well as parse Chain call
  */
call_expr : identifier func_expr
          {
            $2->set_id($1);
            if (!$2->adjust_return_id())
              yyerror("Warning: should not store result value in a property of another variable!", yylloc);
            temp_return_id = $2->get_return_id();
            global_expr_list->push_back($2);
          }
          | call_expr chain_call_expr     ;
          | error chain_call_expr
          {
            yyerror("Error in chain call", yylloc);
            yyerrok;
          }
          ;

chain_call_expr : CHAIN func_expr
                {
                  /* Pass down return_id */
                  $2->set_id(temp_return_id);
                  if (!$2->adjust_return_id())
                    yyerror("Warning: should not store result value in a property of another variable!", yylloc);
                  temp_return_id = $2->get_return_id();
                  global_expr_list->push_back($2);
                }
                | CHAIN IF expression THEN func_expr
                {
                  $5->set_id(temp_return_id);
                  if (!$5->adjust_return_id())
                    yyerror("Warning: should not store result value in a property of another variable!", yylloc);
                  temp_return_id = $5->get_return_id();
                  global_expr_list->push_back(new Cond_Expr($3, $5, @3));
                }
                ;

func_expr : DO identifier
          {
            $$ = new Call_Expr($2, new std::vector<expression *>, new Identifier(new Symbol("last_result"), @2), @2);
          }
          | DO identifier USING parameter_list
          {
            $$ = new Call_Expr($2, $4, new Identifier(new Symbol("last_result"), @2), @2);
          }
          | DO identifier ON identifier
          {
            $$ = new Call_Expr($2, new std::vector<expression *>, $4, @2);
          }
          | DO identifier ON identifier USING parameter_list
          {
            $$ = new Call_Expr($2, $6, $4, @2);
          }
          ;

cond_expr : IF expression THEN expression
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
          std::vector<expression *> *args = new std::vector<expression *>;
          args->push_back($2);
          Call_Expr *call_expr = new Call_Expr($4, new Identifier(new Symbol("ask"), @2), args, new Nil_Identifier(@2), @4);
          call_expr->adjust_return_id(); // last_result is not a property of another variable
          $$ = call_expr;
        }
        | ASK AS identifier
        {
          std::vector<expression *> *args = new std::vector<expression *>;
          Call_Expr *call_expr = new Call_Expr($3, new Identifier(new Symbol("ask"), @3), args, new Nil_Identifier(@2), @3);
          call_expr->adjust_return_id(); // last_result is not a property of another variable
          $$ = call_expr;
        }
        | SAY expression
        {
          std::vector<expression *> *args = new std::vector<expression *>;
          args->push_back($2);
          Call_Expr *call_expr = new Call_Expr(new Nil_Identifier(@2), new Identifier(new Symbol("say"), @2), args, new Nil_Identifier(@2), @2);`
          $$ = call_expr;
        }
        ;

%%

void yyerror(char *s, YYLTYPE loc) {
  std::cerr << "\"" << curr_filename << "\" :" << loc.line << ":" << loc.column
            << ": " << s << " at or near ";
  std::cerr << endl;
  omerrs++;

  if (omerrs > 50) {
    fprintf(stdout, "More than 50 errors\n");
    exit(1);
  }
}
