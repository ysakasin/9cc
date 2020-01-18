#include <stdio.h>
#include <stdlib.h>

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

int alloc4(int **p, int a, int b, int c, int d) {
  int *q = (int *)malloc(sizeof(int) * 4);
  q[0] = a;
  q[1] = b;
  q[2] = c;
  q[3] = d;
  *p = q;
  return 0;
}

int print4(int *p) {
  printf("%d, %d, %d, %d\n", p[0], p[1], p[2], p[3]);
  return 0;
}
