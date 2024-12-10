#ifndef _AST_H_
#define _AST_H_

#include <iostream>
#include <vector>
#include "parser.tab.h"
#include "symtab.h"

class AST_Node {
public:
  YYLTYPE location;
  AST_Node(YYLTYPE loc) { this->location = loc; }
};

/////////////////////////////////////////////
///////////////// AST Nodes /////////////////
/////////////////////////////////////////////

class Program : public AST_Node {
public:
  std::vector<class Expression *> *expr_list;
  Program(std::vector<Expression *> *expr, YYLTYPE loc) : AST_Node(loc) {
    this->expr_list = expr;
  }

  void semant_check();
};

/////////////// Expression //////////////////
class Expression : public AST_Node {
public:
  Symbol *type;
  Expression(YYLTYPE loc) : AST_Node(loc) {}
  virtual void type_check() = 0;
};

class Nil_Expr : public Expression {
public:
  Nil_Expr(YYLTYPE loc) : Expression(loc) {}
  void type_check();
};

/////////////// Identifier //////////////////
class Identifier : public Expression {
public:
  Identifier(YYLTYPE loc) : Expression(loc) {}
  virtual bool has_owner() = 0;
  virtual void type_check() = 0;
};

class Single_Identifier : public Identifier {
public:
  Symbol *name;
  Single_Identifier(Symbol *name, YYLTYPE loc) : Identifier(loc) {
    this->name = name;
  }

  Single_Identifier(YYLTYPE loc) : Identifier(loc) {}
  bool has_owner() { return false; }
  void type_check();
};

class Owner_Identifier : public Identifier {
public:
  Symbol *owner_name;
  Symbol *name;
  Owner_Identifier(Symbol *owner_name, Symbol *name, YYLTYPE loc)
      : Identifier(loc) {
    this->owner_name = owner_name;
    this->name = name;
  }
  bool has_owner() { return true; }
  Identifier *to_Identifier();
  void type_check();
};

class Nil_Identifier : public Identifier {
public:
  Nil_Identifier(YYLTYPE loc) : Identifier(loc) {}
  bool has_owner() { return false; }
  void type_check();
};

/////////////// Declaration //////////////////
class Decl_Expr : public Expression {
public:
  Decl_Expr(YYLTYPE loc) : Expression(loc) {}
  virtual void type_check() = 0;
};

class Var_Decl_Expr : public Decl_Expr {
public:
  Symbol *identifier;
  Expression *init;
  Var_Decl_Expr(Symbol *id, Expression *init, YYLTYPE loc) : Decl_Expr(loc) {
    this->identifier = id;
    this->init = init;
  }
  void type_check();
};

class Property_Decl_Expr : public Decl_Expr {
public:
  Identifier *owner_id;
  Symbol *property_name;
  Property_Decl_Expr(Identifier *owner_id, Symbol *property_id, YYLTYPE loc)
      : Decl_Expr(loc) {
    this->owner_id = owner_id;
    this->property_name = property_id;
  }
  void type_check();
};

/////////////// Assignment //////////////////
class Assi_Expr : public Expression {
public:
  Identifier *id;
  Expression *expr;
  Assi_Expr(Identifier *id, Expression *expr, YYLTYPE loc) : Expression(loc) {
    this->id = id;
    this->expr = expr;
  }
  void type_check();
};

/////////////// Function Call //////////////////
class Call_Expr : public Expression {
public:
  Call_Expr(YYLTYPE loc) : Expression(loc) {}
  virtual bool is_cond_call() = 0;
  virtual void type_check() = 0;
};

class Direct_Call_Expr : public Call_Expr {
public:
  Identifier *id;
  Symbol *func_name;
  std::vector<Expression *> *arg_list;
  Identifier *return_id;
  Direct_Call_Expr(Identifier *id, Symbol *func_name,
                   std::vector<Expression *> *arg_list, Identifier *return_id,
                   YYLTYPE loc)
      : Call_Expr(loc) {
    this->id = id;
    this->func_name = func_name;
    this->arg_list = arg_list;
    this->return_id = return_id;
  }

  Direct_Call_Expr(Symbol *func_name, std::vector<Expression *> *arg_list,
                   Identifier *return_id, YYLTYPE loc)
      : Call_Expr(loc) {
    this->func_name = func_name;
    this->arg_list = arg_list;
    this->return_id = return_id;
  }
  void set_id(Identifier *id);
  // Symbol *get_id();
  // Symbol *get_return_name();
  void set_return_id(Identifier *return_id);
  bool is_cond_call() { return false; }
  // Infer default return_id
  bool adjust_return_id();
  void type_check();
};

class Cond_Call_Expr : public Call_Expr {
public:
  Expression *predictor;
  Direct_Call_Expr *call_expr;

  Cond_Call_Expr(Expression *pre, Identifier *id, Symbol *func_name,
                 std::vector<Expression *> *arg_list, Identifier *return_id,
                 YYLTYPE loc)
      : Call_Expr(loc) {
    this->predictor = pre;
    this->call_expr =
        new Direct_Call_Expr(id, func_name, arg_list, return_id, loc);
  }

  Cond_Call_Expr(Expression *pre, Symbol *func_name,
                 std::vector<Expression *> *arg_list, Identifier *return_id,
                 YYLTYPE loc)
      : Call_Expr(loc) {
    this->predictor = pre;
    this->call_expr = new Direct_Call_Expr(func_name, arg_list, return_id, loc);
  }

  Cond_Call_Expr(Expression *pre, Direct_Call_Expr *call_expr, YYLTYPE loc)
      : Call_Expr(loc) {
    this->predictor = pre;
    this->call_expr = call_expr;
  }

  bool is_cond_call() { return true; }
  void type_check();
};

/////////////// Conditional //////////////////
class Cond_Expr : public Expression {
public:
  Expression *predictor;
  Expression *then;
  Expression *_else;
  Cond_Expr(Expression *predictor, Expression *then, Expression *_else,
            YYLTYPE loc)
      : Expression(loc) {
    this->predictor = predictor;
    this->then = then;
    this->_else = _else;
  }

  Cond_Expr(Expression *predictor, Expression *then, YYLTYPE loc)
      : Expression(loc) {
    this->predictor = predictor;
    this->then = then;
    this->_else = new Nil_Expr(loc);
  }
  void type_check();
};

/////////////// Comparion //////////////////
class Comp_Expr : public Expression {
public:
  Expression *e1;
  Symbol *op;
  Expression *e2;
  Comp_Expr(Expression *e1, Symbol *op, Expression *e2, YYLTYPE loc)
      : Expression(loc) {
    this->e1 = e1;
    this->op = op;
    this->e2 = e2;
  }
  void type_check();
};

/////////////// Arithmetic //////////////////
class Arith_Expr : public Expression {
public:
  Expression *e1;
  Symbol *op;
  Expression *e2;
  Arith_Expr(Expression *e1, Symbol *op, Expression *e2, YYLTYPE loc)
      : Expression(loc) {
    this->e1 = e1;
    this->op = op;
    this->e2 = e2;
  }
  void type_check();
};

/////////////// Constant //////////////////
class String_Const_Expr : public Expression {
public:
  Symbol *token;
  String_Const_Expr(Symbol *token, YYLTYPE loc) : Expression(loc) {
    this->token = token;
  }
  void type_check();
};

class Int_Const_Expr : public Expression {
public:
  Symbol *token;
  Int_Const_Expr(Symbol *token, YYLTYPE loc) : Expression(loc) {
    this->token = token;
  }
  void type_check();
};

class Bool_Const_Expr : public Expression {
public:
  bool value;
  Bool_Const_Expr(bool value, YYLTYPE loc) : Expression(loc) {
    this->value = value;
  }
  void type_check();
};

#endif
