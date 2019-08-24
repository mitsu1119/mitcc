#include "lexer.h"

// If next token is expected reserved word, read token and return true. Otherwise return false.
bool consume(char op) {
	if(nowToken->kind != TK_RESERVED || nowToken->str[0] != op) return false;
	nowToken = nowToken->next;
	return true;
}

// If next token is expected reserved word, read token. Otherwise output error.
void expect(char op) {
	if(nowToken->kind != TK_RESERVED || nowToken->str[0] != op) error("'%c'ではありません。", op);
	nowToken = nowToken->next;
}

// If next token is number token, read token and return the value. Otherwise output error.
int expectNumber() {
	if(nowToken->kind != TK_NUM) error("数ではありません。");
	int val = nowToken->val;
	nowToken = nowToken->next;
	return val;
}

// Check EOF.
bool isEOF() {
	return nowToken->kind == TK_EOF;
}

// Make new token and connect the token to current.
Token *newToken(TokenKind kind, Token *current, char *str) {
	Token *token = calloc(1, sizeof(Token));
	token->kind = kind;
	token->str = str;
	current->next = token;
	return token;
}

// Lexer.
Token *lexer(char *p) {
	Token head;
	head.next = NULL;
	Token *current = &head;

	while(*p) {
		// Skip space.
		if(isspace(*p)) {
			p++;	
			continue;
		}

		if(*p == '+' || *p == '-') {
			current = newToken(TK_RESERVED, current, p++);
			continue;
		}

		if(isdigit(*p)) {
			current = newToken(TK_NUM, current, p);
			current->val = strtol(p, &p, 10);
			continue;
		}

		error("'%c': 不明な入力です。", *p);
	}
	newToken(TK_EOF, current, p);
	return head.next;
}

