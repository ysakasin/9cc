#include "9cc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Wrong argument number");
    return 1;
  }

  if (strcmp(argv[1], "-test") == 0) {
    runtest();
    return 0;
  }

  user_input = argv[1];
  tokenize();

  program();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // Prologue
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  // allocate variables on stack
  printf("  sub rsp, %d\n", idents->keys->len * 8);

  for (int i = 0; i < code->len; i++) {
    gen(code->data[i]);

    printf("  pop rax\n");
  }

  // Epilogue
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}
