#include "9cc.h"

char reg[6][4] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen_lval(Node *node) {
  if (node->ty == '*') {
    gen(node->then);
    return;
  }

  if (node->ty != ND_IDENT) {
    error("lhs is not identifier");
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen_function(Node *node) {
  assert(node->ty == ND_FUNCTION);

  printf("%s:\n", node->name);

  // Prologue
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");

  for (int i = 0; i < node->params_len; i++) {
    printf("  push %s\n", reg[i]);
  }

  // allocate variables on stack
  if (node->stack_len - node->params_len > 0) {
    printf("  sub rsp, %d\n", (node->stack_len - node->params_len) * 8);
  }

  gen(node->then);

  // Epilogue
  printf("  pop rax\n");
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
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

  if (node->ty == '*') {
    gen(node->then);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }

  if (node->ty == '&') {
    gen_lval(node->then);
    return;
  }

  if (node->ty == ND_FUNCTION) {
    gen_function(node);
    return;
  }

  if (node->ty == ND_RETURN) {
    gen(node->lhs);
    printf("  pop rax\n"); // Returned value
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  }

  if (node->ty == ND_IF) {
    int label = if_id++;
    gen(node->cond);
    printf("  pop rax\n"); // condition
    printf("  cmp rax, 0\n");

    if (node->els == NULL) {
      printf("  je .Lend%d\n", label);
      gen(node->then);
      printf(".Lend%d:\n", label);
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
    }
    return;
  }

  if (node->ty == ND_VARIABLE) {
    return;
  }

  if (node->ty == ND_EXPR) {
    gen(node->body);
    printf("  pop rax\n");
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
    for (int i = 0; i < node->args->len; i++) {
      gen(node->args->data[i]);
    }

    for (int i = node->args->len - 1; i >= 0; i--) {
      printf("  pop %s\n", reg[i]);
    }
    // align (rsp % 16 == 0)
    printf("  mov rax, rsp\n");
    printf("  mov rbx, 16\n");
    printf("  cqo\n");
    printf("  div rbx\n");
    printf("  cmp rdx, 0\n");
    printf("  setne al\n");
    printf("  movzb eax, al\n");
    printf("  sub rsp, rax\n");

    printf("  call %s\n", node->name);
    printf("  push rax\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n"); // rhs
  printf("  pop rax\n"); // lhs

  if (node->ty == '+') {
    if (is_ptr(node->lhs->type)) { // ptr
      printf("  push rax\n");
      printf("  mov rax, rdi\n");
      if (is_int(node->lhs->type->ptr_to)) {
        printf("  mov rdi, 4\n");
      } else { // ptr
        assert(is_ptr(node->lhs->type->ptr_to));
        printf("  mov rdi, 8\n");
      }
      printf("  imul rdi\n");
      printf("  mov rdi, rax\n");
      printf("  pop rax\n");
    }
    printf("  add rax, rdi\n");
  } else if (node->ty == '-') {
    if (is_ptr(node->lhs->type)) { // ptr
      printf("  push rax\n");
      printf("  mov rax, rdi\n");
      if (is_int(node->lhs->type->ptr_to)) {
        printf("  mov rdi, 4\n");
      } else { // ptr
        assert(is_ptr(node->lhs->type->ptr_to));
        printf("  mov rdi, 8\n");
      }
      printf("  imul rdi\n");
      printf("  mov rdi, rax\n");
      printf("  pop rax\n");
    }
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
