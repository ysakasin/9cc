#include <assert.h>
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
int map_exist(Map *map, char *key);
void test_map();

void runtest();

// -------------------- //
// Type
// -------------------- //

typedef struct Type {
  enum { INT, PTR, ARRAY } ty;
  struct Type *ptr_to;
  int array_size;
} Type;

Type *ty_int();
Type *ptr_to(Type *ty);
Type *array_of(Type *ty, int size);
int equals_type(Type *x, Type *y);
int is_int(Type *ty);
int is_ptr(Type *ty);
int is_array(Type *ty);
int get_words(Type *ty);
int get_bytes(Type *ty);

// -------------------- //
// Token
// -------------------- //

enum {
  TK_NUM = 256, // Number
  TK_IDENT,     // Identifier
  TK_RETURN,    // return
  TK_IF,        // if
  TK_ELSE,      // else
  TK_SIZEOF,    // sizeof
  TK_INT,       // int
  TK_EQ,        // ==
  TK_NE,        // !=
  TK_LE,        // <=
  TK_GE,        // >=
  TK_EOF,       // End of file
};

typedef struct {
  int ty;
  int val;
  char *name;
  char *input;
} Token;

Token *new_token(int ty, char *input);
Token *new_token_number(int val, char *input);
Token *new_token_ident(char *name, char *input);

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
  ND_FUNCTION,  // Function declare
  ND_RETURN,    // return
  ND_IF,        // if
  ND_BLOCK,     // Block
  ND_VARIABLE,  // Declare a local variable
  ND_EXPR,      // Expression statement
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LE,        // <=
  ND_CALL,
};

typedef struct Node {
  int ty;
  Type *type;

  // Binary expression
  struct Node *lhs;
  struct Node *rhs;

  // Identifier
  int val;
  int offset;

  // if
  struct Node *cond;
  struct Node *then;
  struct Node *els;

  // Block
  Vector *stmts;

  // Function
  char *name;
  Vector *args;
  Vector *params;
  Vector *param_types;
  int params_len;
  int stack_len;
  Type *ret_ty;

  // Expression statement
  struct Node *body;
} Node;

Node *new_node(int ty);
Node *new_node_binop(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_ident(int offset, Type *type);
Type *binop_type(Node *node);

extern Vector *code;
extern Map *idents;
extern Map *idents_type;
extern Map *functions_type;

// -------------------- //
// Parser
// -------------------- //

int consume(int ty);

void program();
Node *declare_function();
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
