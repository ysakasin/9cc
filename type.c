#include "9cc.h"
#include <stdlib.h>

Type *new_type(TypeKind ty) {
  Type *type = calloc(1, sizeof(Type));
  type->ty = ty;
  return type;
}

Type *ty_int() { return new_type(TY_INT); }

Type *ptr_to(Type *base) {
  Type *type = new_type(TY_PTR);
  type->base = base;
  return type;
}

LVar *find_lvar_by_offset(int offset) {
  for (LVar *var = locals; var; var = var->next)
    if (var->offset == offset)
      return var;
  return NULL;
}

int type_sizeof(Type *type) {
  switch (type->ty) {
  case TY_INT:
    return 4;
  case TY_PTR:
    return 8;
  default:
    error("大きさが定義されていない型のサイズが要求された:%d", type->ty);
    return -1;
  }
}

void eval_type(Node *node) {
  switch (node->kind) {
  case ND_PROGRAM:
    for (Node *cur = node->code; cur; cur = cur->next) {
      eval_type(cur);
    }
    break;
  case ND_FUNC:
    locals = node->locals;
    eval_type(node->body);
    break;
  case ND_RETURN:
    eval_type(node->lhs);
    break;
  case ND_IF:
    eval_type(node->cond);
    eval_type(node->then);
    if (node->els) {
      eval_type(node->els);
    }
    break;
  case ND_WHILE:
    eval_type(node->cond);
    eval_type(node->then);
    break;
  case ND_FOR:
    eval_type(node->init);
    eval_type(node->cond);
    eval_type(node->post);
    eval_type(node->then);
    break;
  case ND_BLOCK:
    for (Node *cur = node->body; cur; cur = cur->next) {
      eval_type(cur);
    }
    break;
  case ND_EXPR_STMT:
    eval_type(node->lhs);
    break;
  case ND_ASSIGN:
    eval_type(node->lhs);
    eval_type(node->rhs);
    node->type = node->lhs->type;
    break;
  case ND_EQ:
  case ND_NEQ:
  case ND_LT:
  case ND_LE:
  case ND_ADD:
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
    eval_type(node->lhs);
    eval_type(node->rhs);
    node->type = ty_int();
    break;
  case ND_ADDR:
    eval_type(node->lhs);
    node->type = ptr_to(node->lhs->type);
    break;
  case ND_DEREF:
    eval_type(node->lhs);
    node->type = node->lhs->type->base;
    break;
  case ND_LVAR: {
    LVar *var = find_lvar_by_offset(node->offset);
    node->type = var->type;
    break;
  }
  case ND_NUM:
    node->type = ty_int();
    break;
  case ND_CALL:
    for (Node *cur = node->args; cur; cur = cur->next) {
      eval_type(cur);
    }
    node->type = ty_int();
    break;
  case ND_NOP:
    break;
  default:
    error("型解決がサポートされていないNodeKindが指定された");
  }
}
