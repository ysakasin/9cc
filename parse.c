#include "9cc.h"

Vector *code;
Map *idents;

Node *new_node(int ty) {
  Node *node = calloc(1, sizeof(Node));
  node->ty = ty;
  return node;
}

Node *new_node_binop(int ty, Node *lhs, Node *rhs) {
  Node *node = new_node(ty);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

Node *new_node_ident(int offset) {
  Node *node = new_node(ND_IDENT);
  node->offset = offset;
  return node;
}

int consume(int ty) {
  Token *token = tokens->data[pos];
  if (token->ty != ty) {
    return 0;
  }
  pos++;
  return 1;
}

static void expect(int ty) {
  if (!consume(ty)) {
    Token *t = tokens->data[pos];
    error_at(t->input, "Unexpected");
  }
}

char *ident() {
  Token *token = tokens->data[pos];
  expect(TK_IDENT);
  return token->name;
}

Type *ptr_to(Type *ty) {
  Type *ptr = malloc(sizeof(Type));
  ptr->ty = PTR;
  ptr->ptr_to = ty;
  return ptr;
}

Type *ty_int() {
  Type *ty = malloc(sizeof(Type));
  ty->ty = INT;
  return ty;
}

Type *type() {
  expect(TK_INT);
  Type *ty = ty_int();

  while (consume('*')) {
    ty = ptr_to(ty);
  }
  return ty;
}

int is_type() {
  Token *t = tokens->data[pos];
  return t->ty == TK_INT;
}

int is_eof() {
  Token *t = tokens->data[pos];
  return t->ty == TK_EOF;
}

void program() {
  code = new_vector();
  idents = new_map();

  while (!is_eof()) {
    idents = new_map();
    vec_push(code, declare_function());
  }
}

Node *declare_function() {
  Node *node = new_node(ND_FUNCTION);
  node->params = new_vector();
  node->param_types = new_vector();

  node->ret_ty = type();
  node->name = ident();

  expect('(');
  if (consume(')')) {
    node->then = stmt();
    node->stack_len = idents->keys->len;
    return node;
  }

  int offset = 1;
  Type *param_type = type();
  char *param = ident();
  vec_push(node->params, param);
  vec_push(node->param_types, param_type);
  map_put(idents, param, (void *)(offset * 8));
  while (!consume(')')) {
    offset++;
    expect(',');

    param_type = type();
    param = ident();
    vec_push(node->params, param);
    vec_push(node->param_types, param_type);
    map_put(idents, param, (void *)(offset * 8));
  }

  node->params_len = idents->keys->len;
  if (idents->keys->len > 6) {
    Token *before = tokens->data[pos - 1];
    error_at(before->input, "No more than 6 parameters");
  }

  node->then = stmt();
  node->stack_len = idents->keys->len;
  return node;
}

Node *stmt() {
  Node *node;

  if (consume(TK_IF)) {
    node = new_node(ND_IF);
    expect('(');
    node->cond = expr();
    expect(')');

    node->then = stmt();

    if (consume(TK_ELSE)) {
      node->els = stmt();
    } else {
      node->els = NULL;
    }
    return node;
  }

  if (consume('{')) {
    node = new_node(ND_BLOCK);
    node->stmts = new_vector();

    while (!consume('}')) {
      vec_push(node->stmts, stmt());
    }
    return node;
  }

  if (consume(TK_RETURN)) {
    node = new_node(ND_RETURN);
    node->lhs = expr();
    expect(';');
    return node;
  }

  if (is_type()) {
    node = new_node(ND_VARIABLE);
    node->var_ty = type();

    Token *token = tokens->data[pos];
    node->name = token->name;
    expect(TK_IDENT);

    int offset = (int)map_get(idents, node->name);
    if (offset != 0) {
      error_at(token->input, "It is already defined.");
    }

    offset = (idents->keys->len + 1) * 8;
    map_put(idents, token->name, (void *)offset);
    expect(';');
    return node;
  }

  node = expr();
  expect(';');
  return node;
}

Node *expr() { return assign(); }

Node *assign() {
  Node *node = equality();
  if (consume('=')) {
    node = new_node_binop('=', node, assign());
  }
  return node;
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume(TK_EQ)) {
      node = new_node_binop(ND_EQ, node, relational());
    } else if (consume(TK_NE)) {
      node = new_node_binop(ND_NE, node, relational());
    } else {
      return node;
    }
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume('<')) {
      node = new_node_binop('<', node, add());
    } else if (consume(TK_LE)) {
      node = new_node_binop(ND_LE, node, add());
    } else if (consume('>')) {
      node = new_node_binop('<', add(), node);
    } else if (consume(TK_GE)) {
      node = new_node_binop(ND_LE, add(), node);
    } else {
      return node;
    }
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume('+')) {
      node = new_node_binop('+', node, mul());
    } else if (consume('-')) {
      node = new_node_binop('-', node, mul());
    } else {
      return node;
    }
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume('*')) {
      node = new_node_binop('*', node, unary());
    } else if (consume('/')) {
      node = new_node_binop('/', node, unary());
    } else {
      return node;
    }
  }
}

Node *unary() {
  if (consume('+')) {
    return term();
  }
  if (consume('-')) {
    return new_node_binop('-', new_node_num(0), term());
  }
  if (consume('*')) {
    Node *node = new_node('*');
    node->then = unary();
    return node;
  }
  if (consume('&')) {
    Node *node = new_node('&');
    node->then = unary();
    return node;
  }
  return term();
}

Node *term() {
  Token *t;

  if (consume('(')) {
    Node *node = expr();
    if (!consume(')')) {
      t = tokens->data[pos];
      error_at(t->input, "Expect ')'");
    }
    return node;
  }

  t = tokens->data[pos];
  Token *peek = tokens->data[pos + 1];
  if (t->ty == TK_NUM) {
    pos++;
    return new_node_num(t->val);
  }

  if (t->ty == TK_IDENT && peek->ty == '(') {
    Node *node = new_node(ND_CALL);
    node->name = t->name;
    node->args = new_vector();
    pos += 2;

    if (consume(')')) {
      return node;
    }

    vec_push(node->args, expr());
    while (!consume(')')) {
      expect(',');
      vec_push(node->args, expr());
    }

    if (node->args->len > 6) {
      error_at(peek->input, "No more than 6 arguments");
    }

    return node;
  }

  if (t->ty == TK_IDENT) {
    int offset = (int)map_get(idents, t->name);
    if (offset == 0) {
      error_at(t->input, "undefined variable");
    }

    pos++;
    return new_node_ident(offset);
  }

  error_at(t->input, "Expect '(' or number");
  return NULL; // Can not reach here
}
