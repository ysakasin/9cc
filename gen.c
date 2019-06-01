#include "9cc.h"

void gen_lval(Node *node) {
  if (node->ty != ND_IDENT) {
    error("lhs is not identifier");
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

int if_id;

void gen(Node *node) {
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  if (node->ty == ND_IDENT) {
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }

  if (node->ty == ND_RETURN) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  }

  if (node->ty == ND_IF) {
    int label = if_id++;
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");

    if (node->els == NULL) {
      printf("  je .Lend%d\n", label);
      gen(node->then);
      printf("  pop rax\n");
      printf(".Lend%d:\n", label);
      printf("  push rax\n");
      return;
    } else {
      printf("  je .Lelse%d\n", label);
      gen(node->then);
      printf("  jmp .Lend%d\n", label);

      printf(".Lelse%d:\n", label);
      gen(node->els);

      printf(".Lend%d:\n", label);
      return;
    }
  }

  if (node->ty == ND_BLOCK) {
    for (int i = 0; i < node->stmts->len; i++) {
      gen(node->stmts->data[i]);
      printf("  pop rax\n");
    }
    printf("  push rax\n");
    return;
  }

  if (node->ty == '=') {
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  }

  if (node->ty == ND_CALL) {
    // align (rsp % 16 == 0)
    printf("  mov rax, rsp\n");
    printf("  mov rdi, 16\n");
    printf("  cqo\n");
    printf("  div rdi\n");
    printf("  cmp rdx, 0\n");
    printf("  setne al\n");
    printf("  movzb eax, al\n");
    printf("  add rsp, rax\n");

    printf("  call %s\n", node->name);
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