#include <stdio.h>
#include "9cc.h"

void gen_epilogue() {
  // ローカル変数の領域を解放する
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  // 最後の式の結果がRAXに残っているのでそれが返り値になる
  printf("  ret\n");
}

// ローカル変数があるスタックの位置をスタックに格納
void gen_lvar(Node *node) {
  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  switch (node->kind) {
  case ND_NUM: 
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lvar(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lvar(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n"); // スタックのraxの位置に値rdiを格納
    printf("  push rdi\n");
    return;
  case ND_RETURN:
    gen(node->lhs);
    printf("  pop rax\n");
    gen_epilogue();
    return;
  case ND_IF:
    if (node->els) {
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lelse%p\n", node);
      gen(node->then);
      printf("  jmp .Lend%p\n", node);
      printf(".Lelse%p:\n", node);
      gen(node->els);
      printf(".Lend%p:\n", node);
      return;
    } else {
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%p\n", node);
      gen(node->then);
      printf(".Lend%p:\n", node);
      return;
    }
  case ND_EXPR_STMT:
    gen(node->lhs);
    printf("  pop rax\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_EQ:
    printf("  cmp rax, rdi\n");  // 比較してフラグレジスタにセット
    printf("  sete al\n");       // フラグレジスタを al にセット。alはraxの下位8bit
    printf("  movzb rax, al\n"); // raxの上位56bitを0埋め
    break;
  case ND_NEQ:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  default:
    error("コード生成できません");
  }

  printf("  push rax\n");
}
