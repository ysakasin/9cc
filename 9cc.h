#include <stdbool.h>

//
// Error handling
//

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);

//
// Token
//

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  int len;        // トークンの長さ
};

// 現在着目しているトークン
extern Token *token;

bool consume(char *op);
Token *consume_ident();

void expect(char *op);
int expect_number();
Token *expect_ident();

bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str);
int reserved(char *p);
Token *tokenize(char *p);

//
// Node
//

typedef enum {
  ND_PROGRAM,
  ND_FUNC,   // 関数定義
  ND_RETURN,
  ND_IF,
  ND_WHILE,
  ND_FOR,
  ND_BLOCK,
  ND_EXPR_STMT,
  ND_ASSIGN, // =
  ND_EQ,     // ==
  ND_NEQ,    // !=
  ND_LT,     // <
  ND_LE,     // <=
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_ADDR,   // &x
  ND_DEREF,  // *x
  ND_LVAR,   // ローカル変数
  ND_NUM,    // 整数
  ND_CALL,   // 関数呼び出し
  ND_NOP,    // No Operation
} NodeKind;

typedef struct Node Node;
typedef struct LVar LVar;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;    // ND_NUM
  int offset; // ND_LVAR
  Node *cond; // ND_IF, ND_WHILE, ND_FOR
  Node *then; // ND_IF, ND_WHILE, ND_FOR
  Node *els;  // ND_IF
  Node *init; // ND_FOR
  Node *post; // ND_FOR
  Node *body; // ND_BLOCK, ND_FUNC
  Node *next; // ND_BLOCK, ND_CALL
  char *name; // ND_CALL, ND_FUNC
  Node *args; // ND_CALL
  LVar *locals; // ND_FUNC
  LVar *params; // ND_FUNC
  int nparams;  // ND_FUNC

  Node *code; // ND_PROGRAM
};

Node *new_node(NodeKind kind);
Node *new_node_binop(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

Node *program();
Node *func_decl();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
Node *arguments();

LVar *func_params();

//
// Variable
//

typedef struct LVar LVar;

struct LVar {
  LVar *next; // 次の変数
  char *name; // 変数の名前
  int len;    // 変数名の長さ
  int offset; // RBPからのオフセット
};

extern LVar *locals;

//
// Code generator
//

void gen_epilogue();
void gen(Node *node);
