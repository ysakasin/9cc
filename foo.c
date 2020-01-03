#include <stdio.h>
int foo() { printf("OK\n"); }

int bar(int x, int y) {
  printf("%d\n", x + y);
}

int baz(int x) {
  printf("%d\n", x);
}
