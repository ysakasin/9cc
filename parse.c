#include "9cc.h"

Vector *code;
Map *idents;

Node *new_node(int ty, Node *lhs, Node *rhs) {
  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

Node *new_node_ident(int offset) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_IDENT;
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

int is_eof() {
  Token *t = tokens->data[pos];
  return t->ty == TK_EOF;
}

void program() {
  code = new_vector();
  idents = new_map();

  while (!is_eof()) {
    vec_push(code, stmt());
  }
}

Node *stmt() {
  Node *node;

  if (consume(TK_IF)) {
    node = new_node(ND_IF, NULL, NULL);
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
    node = new_node(ND_BLOCK, NULL, NULL);
    node->stmts = new_vector();

    while (!consume('}')) {
      vec_push(node->stmts, stmt());
    }
    return node;
  }

  if (consume(TK_RETURN)) {
    node = new_node(ND_RETURN, expr(), NULL);
  } else {
    node = expr();
  }

  if (!consume(';')) {
    Token *t = tokens->data[pos];
    error_at(t->input, "Expected ';'");
  }
  return node;
}

Node *expr() { return assign(); }

Node *assign() {
  Node *node = equality();
  if (consume('=')) {
    node = new_node('=', node, assign());
  }
  return node;
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume(TK_EQ)) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume(TK_NE)) {
      node = new_node(ND_NE, node, relational());
    } else {
      return node;
    }
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume('<')) {
      node = new_node('<', node, add());
    } else if (consume(TK_LE)) {
      node = new_node(ND_LE, node, add());
    } else if (consume('>')) {
      node = new_node('<', add(), node);
    } else if (consume(TK_GE)) {
      node = new_node(ND_LE, add(), node);
    } else {
      return node;
    }
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume('+')) {
      node = new_node('+', node, mul());
    } else if (consume('-')) {
      node = new_node('-', node, mul());
    } else {
      return node;
    }
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume('*')) {
      node = new_node('*', node, unary());
    } else if (consume('/')) {
      node = new_node('/', node, unary());
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
    return new_node('-', new_node_num(0), term());
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
  if (t->ty == TK_NUM) {
    pos++;
    return new_node_num(t->val);
  }

  if (t->ty == TK_IDENT) {
    int offset = (int)map_get(idents, t->name);
    if (offset == 0) {
      offset = (idents->keys->len + 1) * 8;
      map_put(idents, t->name, (void *)offset);
    }

    pos++;
    return new_node_ident(offset);
  }

  error_at(t->input, "Expect '(' or number");
  return NULL; // Can not reach here
}
