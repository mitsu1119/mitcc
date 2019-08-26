#include "lexer.h"

char checkSingleletterReserved(char p) {
	// Punctutor.
	static char spuncts[] = {'+', '-', '*', '/', '>', '<', ';', '='};
	for(int i = 0; i < sizeof(spuncts) / sizeof(char); i++) {
		if(p == spuncts[i]) return spuncts[i];
	}
	return 0;
}

char *checkMultiletterReserved(char *p) {
	// Punctutor.
	static char *mpuncts[] = {"==", "<=", ">="};
	for(int i = 0; i < sizeof(mpuncts) / sizeof(*mpuncts); i++) {
		if(!strncmp(p, mpuncts[i], strlen(mpuncts[i]))) return mpuncts[i];
	}

	return NULL;
}

// If next token is expected reserved word, read token and return true. Otherwise return false.
bool consume(char *op) {
	if(nowToken->kind != TK_RESERVED || strlen(op) != nowToken->len || strncmp(nowToken->str, op, nowToken->len)) return false;
	nowToken = nowToken->next;
	return true;
}

// If next token is expected reserved word, read token. Otherwise output error.
void expect(char *op) {
	if(nowToken->kind != TK_RESERVED || strlen(op) != nowToken->len || strncmp(nowToken->str, op, nowToken->len)) error(nowToken->str, "'%s'ではありません。", op);
	nowToken = nowToken->next;
}

// If next token is number token, read token and return the value. Otherwise output error.
int expectNumber() {
	if(nowToken->kind != TK_NUM) error(nowToken->str, "数ではありません。");
	int val = nowToken->val;
	nowToken = nowToken->next;
	return val;
}

// If next token is identifier token, read token and return the token pointer. Otherwise return NULL.
Token *consumeIdentifier() {
	if(nowToken->kind != TK_IDENT) return NULL;
	Token *oldToken = nowToken;
	nowToken = nowToken->next;
	return oldToken;
}

// Check EOF.
bool isEOF() {
	return nowToken->kind == TK_EOF;
}

// Make new token and connect the token to current.
Token *newToken(TokenKind kind, Token *current, char *str, int len) {
	Token *token = calloc(1, sizeof(Token));
	token->kind = kind;
	token->str = str;
	token->len = len;
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

		// Multi letter reserved. ex) "<=", ">="
		char *ch = checkMultiletterReserved(p);
		if(ch) {
			int len = strlen(ch);
			current = newToken(TK_RESERVED, current, p, len);
			p += len;
			continue;
		}
		
		char c = checkSingleletterReserved(*p);
		if(c != 0) {
			current = newToken(TK_RESERVED, current, p++, 1);
			continue;
		}

		if(isalpha(*p) || *p == '_') {
			char *q = p++;
			while(isalnum(*p) || *p == '_') p++;
			current = newToken(TK_IDENT, current, q, p - q);
			continue;
		}

		if(isdigit(*p)) {
			current = newToken(TK_NUM, current, p, 0);
			current->val = strtol(p, &p, 10);
			continue;
		}
		error(p, "'%c': 不明な入力です。", *p);		
	}
	newToken(TK_EOF, current, p, 1);
	return head.next;
}

