#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -------------------- //
// Container
// -------------------- //

typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

Vector *new_vector();
void vec_push(Vector *vec, void *elem);
void runtest();

// -------------------- //
// Token
// -------------------- //

enum {
  TK_NUM = 256, // Number
  TK_EQ,        // ==
  TK_NE,        // !=
  TK_LE,        // <=
  TK_GE,        // >=
  TK_EOF,       // End of file
};

typedef struct {
  int ty;
  int val;
  char *input;
} Token;

Token *new_token(int ty, char *input);
Token *new_number_token(int val, char *input);

extern Vector *tokens;
extern int pos;
extern char *user_input;

// -------------------- //
// Lexer
// -------------------- //

void tokenize();

// -------------------- //
// Node
// -------------------- //

enum {
  ND_NUM = 256,
  ND_EQ, // ==
  ND_NE, // !=
  ND_LE, // <=
};

typedef struct Node {
  int ty;
  struct Node *lhs;
  struct Node *rhs;
  int val;
} Node;

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);

// -------------------- //
// Parser
// -------------------- //

int consume(int ty);

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();

// -------------------- //
// Code generator
// -------------------- //

void gen(Node *node);

// -------------------- //
// Error utils
// -------------------- //

void error(char *fmt, ...);
void error_at(char *loc, char *msg);
