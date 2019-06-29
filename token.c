#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "9cc.h"

Token *new_token(int ty, char *input) {
  Token *token = calloc(1, sizeof(Token));
  token->ty = ty;
  token->input = input;
  return token;
}

Token *new_token_number(int val, char *input) {
  Token *token = new_token(TK_NUM, input);
  token->val = val;
  return token;
}

Token *new_token_ident(char *name, char *input) {
  Token *token = new_token(TK_IDENT, input);
  token->name = name;
  return token;
}

Vector *tokens;
int pos;
char *user_input;

int is_alnum(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') || (c == '_');
}

char *strtoident(char *c, char **endptr) {
  int length = 0;
  while (is_alnum(*(c + length))) {
    length++;
  }

  *endptr = (c + length);
  char *res = calloc(length + 1, sizeof(char));
  return strncpy(res, c, length);
}

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

    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      token = new_token(TK_RETURN, p);
      vec_push(tokens, token);
      p += 6;
      continue;
    }

    if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
      token = new_token(TK_IF, p);
      vec_push(tokens, token);
      p += 2;
      continue;
    }

    if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
      token = new_token(TK_ELSE, p);
      vec_push(tokens, token);
      p += 4;
      continue;
    }

    if (strncmp(p, "sizeof", 6) == 0 && !is_alnum(p[6])) {
      token = new_token(TK_SIZEOF, p);
      vec_push(tokens, token);
      p += 6;
      continue;
    }

    if (strncmp(p, "int", 3) == 0 && !is_alnum(p[3])) {
      token = new_token(TK_INT, p);
      vec_push(tokens, token);
      p += 3;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '<' || *p == '>' || *p == '=' || *p == ';' ||
        *p == ',' || *p == '{' || *p == '}' || *p == '[' || *p == ']' ||
        *p == '&') {
      token = new_token(*p, p);
      vec_push(tokens, token);
      p++;
      continue;
    }

    if (isdigit(*p)) {
      int val = strtol(p, &p, 10);
      token = new_token_number(val, p);
      vec_push(tokens, token);
      continue;
    }

    if (is_alnum(*p)) {
      char *name = strtoident(p, &p);
      token = new_token_ident(name, p);
      vec_push(tokens, token);
      continue;
    }

    error_at(p, "Can not tokenize");
  }

  token = new_token(TK_EOF, p);
  vec_push(tokens, token);
}
