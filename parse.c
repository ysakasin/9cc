#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "9cc.h"

Node *new_node(NodeKind kind) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_node_binop(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

Node *new_node_lvar(int offset) {
  Node *node = new_node(ND_LVAR);
  node->offset = offset;
  return node;
}

LVar *locals;

LVar *new_lvar(Token *tok) {
  LVar *var = calloc(1, sizeof(LVar));
  var->name = tok->str;
  var->len = tok->len;
  return var;
}

LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && strncmp(var->name, tok->str, var->len) == 0)
      return var;
  return NULL;
}

LVar *append_lvar(Token *tok) {
  LVar *var = new_lvar(tok);
  var->next = locals;
  var->offset = locals->offset + 8;
  locals = var;
  return var;
}

int len_lvar(LVar *lvar) {
  int len = 0;
  while (lvar->next) {
    len++;
    lvar = lvar->next;
  }
  return len;
}

char *strndup(char *str, int size) {
  char *ret = calloc(size, sizeof(char));
  strncpy(ret, str, size);
  return ret;
}

/* Grammar
program    = stmt*
stmt       = expr ";"
           | "return" expr ";"
           | "if" "(" expr ")" stmt ("else" stmt)?
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num
           | ident ("(" ")")?
           | "(" expr ")"
*/

Node *program() {
  Node head = {};
  Node *cur = &head;

  while (!at_eof()) {
    cur->next = func_decl();
    cur = cur->next;
  }

  Node *prog = new_node(ND_PROGRAM);
  prog->code = head.next;
  return prog;
}

Node *func_decl() {
  Node *node = new_node(ND_FUNC);
  locals = calloc(1, sizeof(LVar));

  Token *tok = expect_ident();
  node->name = strndup(tok->str, tok->len); 
  node->params = func_params();
  node->nparams = len_lvar(node->params);
  node->body = stmt();
  node->locals = locals;

  if (node->nparams > 6)
    error_at(tok->str, "引数の数が6以下ではありません");

  return node;
}

LVar *func_params() {
  expect("(");
  if (consume(")"))
    return locals;

  Token *tok = expect_ident();
  append_lvar(tok);

  while (consume(",")) {
    Token *tok = expect_ident();
    append_lvar(tok);
  }
  expect(")");
  return locals;
}

Node *stmt() {
  Node *node;

  if (consume("if")){
    node = new_node(ND_IF);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if (consume("else"))
      node->els = stmt();
    return node;
  }

  if (consume("while")) {
    node = new_node(ND_WHILE);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    return node;
  }

  if (consume("for")) {
    node = new_node(ND_FOR);
    expect("(");
    node->init = expr();
    expect(";");
    node->cond = expr();
    expect(";");
    node->post = expr();
    expect(")");
    node->then = stmt();
    return node;
  }

  if (consume("{")) {
    Node head = {};
    Node *cur = &head;

    while (!consume("}")) {
      cur->next = stmt();
      cur = cur->next;
    }

    node = new_node(ND_BLOCK);
    node->body = head.next;
    return node;
  }

  if (consume("return")) {
    node = new_node(ND_RETURN);
    node->lhs = expr();
  } else {
    node = new_node(ND_EXPR_STMT);
    node->lhs = expr();
  }

  expect(";");
  return node;
}

Node *expr() {
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_node_binop(ND_ASSIGN, node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("=="))
      node = new_node_binop(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_node_binop(ND_NEQ, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();

  for (;;) {
    if (consume("<"))
      node = new_node_binop(ND_LT, node, add());
    else if (consume("<="))
      node = new_node_binop(ND_LE, node, add());
    else if (consume(">"))
      node = new_node_binop(ND_LT, add(), node);
    else if (consume(">="))
      node = new_node_binop(ND_LE, add(), node);
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_node_binop(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node_binop(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node_binop(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node_binop(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume("+"))
    return primary();
  else if (consume("-"))
    return new_node_binop(ND_SUB, new_node_num(0), primary());
  return primary();
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    if (consume("(")) {
      Node *node = new_node(ND_CALL);
      node->name = strndup(tok->str, tok->len);
      node->args = arguments();
      return node;
    }

    LVar *var = find_lvar(tok);
    if (var == NULL) {
      var = append_lvar(tok);
    }
    return new_node_lvar(var->offset);
  }

  return new_node_num(expect_number());
}

Node *arguments() {
  if (consume(")"))
    return NULL;

  Node *head = expr();
  Node *cur = head;
  while (consume(",")) {
    cur->next = expr();
    cur = cur->next;
  }
  expect(")");
  return head;
}
