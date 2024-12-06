#ifndef _AST_H_
#define _AST_H_

#include "symtab.h"
#include <iostream>
#include <vector>

struct YYLTYPE {
  int line;
  int column;
  YYLTYPE() : line(0), column(0) {}
};

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
};

/////////////// Expression //////////////////
class Expression : public AST_Node {
public:
  Expression(YYLTYPE loc) : AST_Node(loc) {}
};

class Nil_Expr : public Expression {
public:
  Nil_Expr(YYLTYPE loc) : Expression(loc) {}
};

/////////////// Identifier //////////////////
class Identifier : public Expression {
public:
  Symbol *name;
  Identifier(Symbol *id, YYLTYPE loc) : Expression(loc) { this->name = id; }
  Identifier(YYLTYPE loc) : Expression(loc) {}
  bool has_owner() { return false; }
};

class Owner_Identifier : public Identifier {
public:
  Symbol *owner_id;
  Symbol *id;
  Owner_Identifier(Symbol *owner_id, Symbol *id, YYLTYPE loc)
      : Identifier(id, loc) {
    this->owner_id = owner_id;
  }
  bool has_owner() { return true; }
};

class Nil_Identifier : public Identifier {
public:
  Nil_Identifier(YYLTYPE loc) : Identifier(loc) {}
  bool has_owner() { return false; }
};

/////////////// Declaration //////////////////
class Decl_Expr : public Expression {
public:
  Decl_Expr(YYLTYPE loc) : Expression(loc) {}
};

class Var_Decl_Expr : public Decl_Expr {
public:
  Identifier *identifier;
  Expression *init;
  Var_Decl_Expr(Identifier *id, Expression *init, YYLTYPE loc)
      : Decl_Expr(loc) {
    this->identifier = id;
    this->init = init;
  }
};

class Property_Decl_Expr : public Decl_Expr {
public:
  Identifier *owner_id;
  Identifier *property_id;
  Property_Decl_Expr(Identifier *owner_id, Identifier *property_id, YYLTYPE loc)
      : Decl_Expr(loc) {
    this->owner_id = owner_id;
    this->property_id = property_id;
  }
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
};

/////////////// Function Call //////////////////
class Call_Expr : public Expression {
public:
  Identifier *id;
  Identifier *func_name;
  std::vector<Expression *> *arg_list;
  Identifier *return_id;
  Call_Expr(Identifier *id, Identifier *func_name,
            std::vector<Expression *> *arg_list, Identifier *return_id,
            YYLTYPE loc)
      : Expression(loc) {
    this->id = id;
    this->func_name = func_name;
    this->arg_list = arg_list;
    this->return_id = return_id;
  }

  Call_Expr(Identifier *func_name, std::vector<Expression *> *arg_list,
            Identifier *return_id, YYLTYPE loc)
      : Expression(loc) {
    this->func_name = func_name;
    this->arg_list = arg_list;
    this->return_id = return_id;
  }
  void set_id(Identifier *id);
  Symbol *get_id();
  Symbol *get_return_id();
  void set_return_id(Identifier *return_id);
  // Infer default return_id
  bool adjust_return_id();
};

// class Cond_Call_Expr : public Call_Expr {
// public:
//   Expression *predictor;
//   Direct_Call_Expr *call_expr;
//   Cond_Call_Expr(Expression *predictor, Direct_Call_Expr *call_expr,
//                  YYLTYPE loc)
//       : Call_Expr(loc) {
//     this->predictor = predictor;
//     this->call_expr = call_expr;
//   }
// };

/////////////// Conditional //////////////////
class Cond_Expr : public Expression {
public:
  Expression *predictor;
  Expression *then;
  Cond_Expr(Expression *predictor, Expression *then, YYLTYPE loc)
      : Expression(loc) {
    this->predictor = predictor;
    this->then = then;
  }
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
};

/////////////// Constant //////////////////
class String_Const_Expr : public Expression {
public:
  Symbol *token;
  String_Const_Expr(Symbol *token, YYLTYPE loc) : Expression(loc) {
    this->token = token;
  }
};

class Int_Const_Expr : public Expression {
public:
  Symbol *token;
  Int_Const_Expr(Symbol *token, YYLTYPE loc) : Expression(loc) {
    this->token = token;
  }
};

class Bool_Const_Expr : public Expression {
public:
  bool value;
  Bool_Const_Expr(bool value, YYLTYPE loc) : Expression(loc) {
    this->value = value;
  }
};

#endif
