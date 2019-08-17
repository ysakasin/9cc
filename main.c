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
  Token *tok = tokenize(user_input);

  program();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  for (int i = 0; i < code->len; i++) {
    gen(code->data[i]);
  }

  return 0;
}
