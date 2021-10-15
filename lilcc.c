#include <stdio.h>
#include <stdlib.h>

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
