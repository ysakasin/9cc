#include "9cc.h"
#include <stdlib.h>

Type *new_type(TypeKind ty) {
  Type *type = calloc(1, sizeof(Type));
  type->ty = ty;
  return type;
}

Type *ty_int() {
  return new_type(TY_INT);
}

Type *ptr_to(Type *base) {
  Type *type = new_type(TY_PTR);
  type->ptr_to = base;
  return type;
} 
