#include "AST.h"
#include "symtab.h"

void Call_Expr::set_id(Identifier *id) { this->id = id; }

Symbol *Call_Expr::get_id() { return id->name; }

Symbol *Call_Expr::get_return_id() { return return_id->name; }

void Call_Expr::set_return_id(Identifier *return_id) {
  this->return_id = return_id;
}

/*
 *  Adjust return_id to property with owner.
 *  If detect owner is different with identifier who calls funcition, return
 * false. This function will make sure the return_id is a Owner_Identifier.
 */
bool Call_Expr::adjust_return_id() {
  if (return_id->has_owner())
    // Check whether is the same owner
    return *(return_id->name) == *(id->name);
  Symbol *owner;
  if (id->has_owner())
    // The owner of return_id is same as the owner of id
    owner = static_cast<Owner_Identifier *>(id)->owner_id;
  else
    // The owner of return_id is id
    owner = id->name;
  return_id = new Owner_Identifier(owner, return_id->name, location);
  return true;
}
