#include "9cc.h"

void gen(Node *node) {
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  if (node->ty == '+') {
    printf("  add rax, rdi\n");
  } else if (node->ty == '-') {
    printf("  sub rax, rdi\n");
  } else if (node->ty == '*') {
    printf("  imul rdi\n");
  } else if (node->ty == '/') {
    printf("  cqo\n");
    printf("  idiv rdi\n");
  } else if (node->ty == ND_EQ) {
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
  } else if (node->ty == ND_NE) {
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
  } else if (node->ty == '<') {
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
  } else if (node->ty == ND_LE) {
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
  }

  printf("  push rax\n");
}
