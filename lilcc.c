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

/**
 * ノードの種類定義 
 * ND_ADD = 足し算
 * ND_SUB = 引き算
 * ND_MUL = 掛け算
 * ND_DIV = 割り算
 * ND_NUM = 整数
 */
typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
} NodeKind;

typedef struct Token Token;
typedef struct Node Node;

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

/**
 * 抽象構文木ノードのタイプ定義
 * kind = ノードの型
 * lhs = 左辺
 * rhs = 右辺
 * val = kind == ND_NUMの場合のみ使用
 */
struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

/** 現在のtoken */
Token *token;

/** 入力全て */
char *user_input;

/**
 * エラー関数
 * エラー箇所と原因を報告
 * @param loc
 * @param fmt
 * @param ...
 */
void err(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int blank = loc - user_input;

    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", blank, " ");
    fprintf(stderr, "^ ");
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
        err(token->str, "not '%c'", op);
    token = token->next;
}

/**
 * 次のトークンがintegerの場合、その数値を返す。
 * トークンは一つ進む
 * @return
 */
int expected_number() {
    if (token->Kind != TK_NUM)
        err(token->str, "this is not integer");
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
        err(token->str, "failed to tokenize");
    }

    // 文字の最後EOFトークンとして追加
    new_token(TK_EOF, cur, p);

    return head.next;
}

/**
 * 左辺と右辺を受け取る二項演算子のノードを新しく作成する関数
 * 
 * @param kind 
 * @param lhs 
 * @param rhs 
 * @return Node* 
 */
Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

/**
 * 数値ノードを作成する関数
 * 
 * @param val 
 * @return Node* 
 */
Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *primary () {
    // 次のトークンが(の場合、"(" expr ")" である可能性が高い
    if (skip('(')) {
        Node *node = expr();
        expect(')');
        return node;
    }

    // もし違うなら数値のはず
    return new_node_num(expected_number());
}

/**
 * 左結合の演算子をパースする関数(掛割)
 * 
 * @return Node* 
 */
Node *mul() {
    Node *node = primary();

    for (;;) {
        if (skip('*'))
            node = new_node(ND_MUL, node, primary());
        else if (skip('/'))
            node = new_node(ND_DIV, node, primary());
        else
            return node;
    }
}

/**
 * 左結合の演算子をパーズする関数(足し引き)
 * 
 * @return Node* 
 */
Node *expr() {
    Node *node = mul();

    for (;;) {
        if (skip('+')) 
            node = new_node(ND_ADD, node, mul());
        else if (skip('-'))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
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
     * アセンブリの頭部分出力
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
