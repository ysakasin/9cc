#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "9cc.h"

Token *new_token(int ty, char *input) {
  Token *token = malloc(sizeof(Token));
  token->ty = ty;
  token->val = 0;
  token->input = input;
  return token;
}

Token *new_number_token(int val, char *input) {
  Token *token = malloc(sizeof(Token));
  token->ty = TK_NUM;
  token->val = val;
  token->input = input;
  return token;
}

Vector *tokens;
int pos;
char *user_input;

void tokenize() {
  char *p = user_input;
  Token *token;

  tokens = new_vector();
  while (*p) {
    // skip blank character
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (strncmp(p, "==", 2) == 0) {
      token = new_token(TK_EQ, p);
      vec_push(tokens, token);
      p += 2;
      continue;
    }

    if (strncmp(p, "!=", 2) == 0) {
      token = new_token(TK_NE, p);
      vec_push(tokens, token);
      p += 2;
      continue;
    }

    if (strncmp(p, "<=", 2) == 0) {
      token = new_token(TK_LE, p);
      vec_push(tokens, token);
      p += 2;
      continue;
    }

    if (strncmp(p, ">=", 2) == 0) {
      token = new_token(TK_GE, p);
      vec_push(tokens, token);
      p += 2;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '<' || *p == '>') {
      token = new_token(*p, p);
      vec_push(tokens, token);
      p++;
      continue;
    }

    if (isdigit(*p)) {
      int val = strtol(p, &p, 10);
      token = new_number_token(val, p);
      vec_push(tokens, token);
      continue;
    }

    error_at(p, "Can not tokenize");
  }

  token = new_token(TK_EOF, p);
  vec_push(tokens, token);
}
