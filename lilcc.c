#include <stdio.h>
#include <stdlib.h>

/**
 * トークンの種類定義
 * TK_SYMBOL = 記号
 * TK_NUM = 数字
 * TK_EOF = 入力の終わり
 */
typedef enum {
    TK_SYMBOL,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;

/**
 * トークンの型定義
 * kind = トークンの種類
 * *next = 次の入力
 * val = kind == TK_NUMの時、数字
 * *str = 文字
 */
struct Token {
    TokenKind Kind;
    Token *next;
    int val;
    char *str;
};

/** 現在のtoken */
Token *token;

/**
 * エラー関数
 * @param fmt
 * @param ...
 */
void err (char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

/**
 * トークンが記号の時、trueを返す
 * trueの場合はトークンを一つ進める
 * トークンが記号以外の場合はfalse
 * @param op
 * @return
 */
bool skip(char op) {
    if (token->kind != TK_SYMBOL || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

int main(int argc, char **argv) {

  /**
   * 引数の数が適切でない場合、エラーを返す
   */
  if (argc != 2) {
    fprintf(stderr, "Incorrect number of arguments \n");
    return 1;
  }

  char *p = argv[1];

  /**
   * アセンブリで出力
   */
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  printf("  mov rax, %ld\n", strtol(p, &p, 10));
  while (*p) {

    /**
     * @brief Construct a new if object
     * 記号(+)の場合は足し算(add rax)したいので
     * 記号の次の文字を参照
     */
    if (*p == '+') {
      p++;
      printf("  add rax, %ld\n", strtol(p, &p, 10));
      continue;
    }

    /**
     * @brief Construct a new if object
     * 記号(-)の場合は引き算(sub rax)したいので
     * 次の文字を引く
     */
    if (*p == '-') {
      p++;
      printf("  sub rax, %ld\n", strtol(p, &p, 10));
      continue;
    }

    /** それ以外の場合は予期しない記号としてエラーを出す */
    fprintf(stderr, "unexpected: '%c'\n", *p);
    return 1;
  }
  printf("  ret\n");
  return 0;
}
