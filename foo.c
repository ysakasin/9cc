#include <stdio.h>
int foo() {
  printf("OK\n");
  return 0;
}

int bar(int x, int y) {
  printf("%d\n", x + y);
  return 0;
}

int baz(int x) {
  printf("%d\n", x);
  return 0;
}
