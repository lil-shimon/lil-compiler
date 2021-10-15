#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

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
void err(char *fmt, ...) {
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
    if (token->Kind != TK_SYMBOL || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

/**
 * 次の記号が予期している記号の時は、トークンを一つ進める
 * それ以外の記号の場合はエラーを出す
 * @param op
 */
void expect(char op) {
    if (token->Kind != TK_SYMBOL || token->str[0] != op)
        err("not '%c'", op);
    token = token->next;
}

/**
 * 次のトークンがintegerの場合、その数値を返す。
 * トークンは一つ進む
 * @return
 */
int expected_number() {
    if (token->Kind != TK_NUM)
        err("this is not integer");
    int val = token->val;
    token = token->next;
    return val;
}

/**
 * 最終かを判定
 * @return
 */
bool is_eof() {
    return token->Kind == TK_EOF;
}

/**
 * 新しいトークンを返す関数
 * @param kind
 * @param cur
 * @param str
 * @return
 */
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tk = calloc(1, sizeof(Token));
    tk->Kind = kind;
    tk->str = str;
    cur->next = tk;
    return tk;
}

/**
 * 入力された値(*p)をトークナイズする
 * @param p
 * @return
 */
Token *tokenize(char *p) {

    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {


        // スペースの場合は飛ばす
        if (isspace(*p)) {
            p++;
            continue;
        }

        // + or -の場合はシンボルトークンとして追加
        if (*p == '+' || *p == '-') {
            cur = new_token(TK_SYMBOL, cur, p++);
            continue;
        }

        // 数字の場合は数字トークンとして追加
        // 数字として登録
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        // それ以外はエラー
        err("failed to tokenize");
    }

    // 文字の最後EOFトークンとして追加
    new_token(TK_EOF, cur, p);

    return head.next;
}

int main(int argc, char **argv) {

    /**
     * 引数の数が適切でない場合、エラーを返す
     */
    if (argc != 2) {
        fprintf(stderr, "Incorrect number of arguments \n");
        return 1;
    }

    /**
     * トークナイズ
     */
    token = tokenize(argv[1]);

    /**
     * アセンブリで出力
     */
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    /**
     * 数値かを判定
     * 計算の場合は最初が数字でないといけないので
     * アセンブリでも最初のmovに数字が入る
     */
    printf("  mov rax, %d\n", expected_number());

    while (!is_eof()) {

        /**
         * 記号(+)の場合は足し算(add rax)したいので
         * 記号の次の文字を参照
         */
        if (skip('+')) {
            printf("  add rax, %d\n", expected_number());
            continue;
        }

        /**
         * - の場合はsub rax
         */
        expect('-');
        printf("  sub rax, %d\n", expected_number());
    }

    printf("  ret\n");
    return 0;
}
