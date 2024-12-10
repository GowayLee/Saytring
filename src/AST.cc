#include "AST.h"
#include "symtab.h"

void Direct_Call_Expr::set_id(Identifier *id) { this->id = id; }

// Symbol *Direct_Call_Expr::get_id() { return id->name; }

// Symbol *Direct_Call_Expr::get_return_name() { return return_id->name; }

void Direct_Call_Expr::set_return_id(Identifier *return_id) {
  this->return_id = return_id;
}

/*
 *  Adjust return_id to property with owner.
 *  If detect owner is different with identifier who calls funcition, return
 *  false. This function will make sure the return_id is a Owner_Identifier.
 */
bool Direct_Call_Expr::adjust_return_id() {
  // Check whether is the same owner
  // Both two id has owner
  if (return_id->has_owner() && id->has_owner()) {
    return *(static_cast<Owner_Identifier *>(return_id)->owner_name) ==
           *(static_cast<Owner_Identifier *>(id)->owner_name);
  }
  // Return id has owner
  if (return_id->has_owner())
    return *(static_cast<Owner_Identifier *>(return_id)->owner_name) ==
           *(static_cast<Single_Identifier *>(id)->name);
  Symbol *owner;
  if (id->has_owner())
    // The owner of return_id is same as the owner of id
    owner = static_cast<Owner_Identifier *>(id)->owner_name;
  else
    // The owner of return_id is id
    owner = static_cast<Single_Identifier *>(id)->name;
  return_id = new Owner_Identifier(
      owner, static_cast<Single_Identifier *>(return_id)->name, location);
  return true;
}

Identifier *Owner_Identifier::to_Identifier() {
  return new Single_Identifier(name, location);
}
