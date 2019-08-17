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

typedef enum {
  TK_RESERVED, // 予約語と記号
  TK_IDENT,    // Identifier
  TK_NUM,      // 数字
  TK_EOF,      // End of file
} TokenKind;

typedef struct Token Token;
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  int len;        // トークン文字列の長さ
};

extern char *user_input;

// -------------------- //
// Lexer
// -------------------- //

Token *tokenize(char *p);

// -------------------- //
// Node
// -------------------- //

typedef enum {
  ND_ADD,      // +
  ND_SUB,      // -
  ND_MUL,      // *
  ND_DIV,      // /
  ND_NUM,      // Number
  ND_IDENT,    // Identifier
  ND_FUNCTION, // Function declare
  ND_RETURN,   // return
  ND_IF,       // if
  ND_BLOCK,    // Block
  ND_VARIABLE, // Declare a local variable
  ND_EXPR,     // Expression statement
  ND_EQ,       // ==
  ND_NE,       // !=
  ND_LT,       // <
  ND_LE,       // <=
  ND_CALL,
} NodeKind;

typedef struct Node Node;
struct Node {
  int ty;
  Type *type;

  // Binary expression
  Node *lhs;
  Node *rhs;

  // Identifier
  int val;
  int offset;

  // if
  Node *cond;
  Node *then;
  Node *els;

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
};

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
