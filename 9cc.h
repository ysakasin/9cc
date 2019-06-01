#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
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
void test_vector();

typedef struct {
  Vector *keys;
  Vector *vals;
} Map;

Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);
void test_map();

void runtest();

// -------------------- //
// Token
// -------------------- //

enum {
  TK_NUM = 256, // Number
  TK_IDENT,     // Identifier
  TK_RETURN,    // return
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
  ND_NUM = 256, // Number
  ND_IDENT,     // Identifier
  ND_RETURN,    // return
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LE,        // <=
};

typedef struct Node {
  int ty;
  struct Node *lhs;
  struct Node *rhs;
  int val;
  char name;
} Node;

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_ident(char name);

extern Vector *code;

// -------------------- //
// Parser
// -------------------- //

int consume(int ty);

void program();
Node *stmt();
Node *expr();
Node *assign();
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
