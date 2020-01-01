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
  ND_EQ,  // ==
  ND_NEQ, // !=
  ND_LT,  // <
  ND_LE,  // <=
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // 整数
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val; // ND_NUM
};

Node *new_node(NodeKind kind);
Node *new_node_binop(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

//
// Code generator
//

void gen(Node *node);
