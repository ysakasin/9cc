#include <stdio.h>
#include "9cc.h"

static const char *call_reg[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

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
  case ND_WHILE:
    printf(".Lbegin%p:\n", node);
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%p\n", node);
    gen(node->then);
    printf("  jmp .Lbegin%p\n", node);
    printf(".Lend%p:\n", node);
    return;
  case ND_FOR:
    gen(node->init);
    printf("  pop rax\n");
    printf(".Lbegin%p:\n", node);
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%p\n", node);
    gen(node->then);
    gen(node->post);
    printf("  pop rax\n");
    printf("  jmp .Lbegin%p\n", node);
    printf(".Lend%p:\n", node);
    return;
  case ND_BLOCK:
    for (Node *cur = node->body; cur; cur = cur->next) {
      gen(cur);
    }
    return;
  case ND_EXPR_STMT:
    gen(node->lhs);
    printf("  pop rax\n");
    return;
  case ND_CALL: {
    int len = 0;
    for (Node *cur = node->args; cur; cur = cur->next) {
      gen(cur);
      len++;
    }

    for (int i = len - 1; i >= 0; i--)
      printf("  pop %s\n", call_reg[i]);

    // rspを16の倍数に揃えてから呼び出す
    printf("  mov rax, rsp\n");
    printf("  and rax, 15\n");
    printf("  jnz .L.call%p\n", node);
    printf("  mov rax, 0\n");
    printf("  call %s\n", node->name);
    printf("  jmp .L.end%p\n", node);
    printf(".L.call%p:\n", node);
    printf("  sub rsp, 8\n");
    printf("  mov rax, 0\n");
    printf("  call %s\n", node->name);
    printf("  add rsp, 8\n");
    printf(".L.end%p:\n", node);
    printf("  push rax\n");
    return;
  }
  case ND_FUNC:
    printf("%s:\n", node->name);

    // プロローグ
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", node->locals->offset);
    
    for (Node *cur = node->body; cur; cur = cur->next) {
      gen(cur);
    }
    gen_epilogue();
    return;
  case ND_PROGRAM:
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    for (Node *cur = node->code; cur; cur = cur->next) {
      gen(cur);
    }
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
