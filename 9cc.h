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
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str);
int reserved(char *p);
Token *tokenize(char *p);

//
// Node
//

typedef enum {
  ND_RETURN,
  ND_ASSIGN, // =
  ND_EQ,     // ==
  ND_NEQ,    // !=
  ND_LT,     // <
  ND_LE,     // <=
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_LVAR,   // ローカル変数
  ND_NUM,    // 整数
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;    // ND_NUM
  int offset; // ND_LVAR
};

Node *new_node(NodeKind kind);
Node *new_node_binop(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

extern Node *code[100];

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

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
