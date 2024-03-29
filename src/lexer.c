#include "lexer.h"

char *stringLiterals[stringLiteralsSize] = {NULL};
int nowStringLiteralsNum = 0;

char checkSingleletterReserved(char p) {
	// Punctutor.
	static char spuncts[] = {'+', '-', '*', '/', '>', '<', ';', '=', '(', ')', '[', ']', '{', '}', ',', '&', '"'};
	for(int i = 0; i < sizeof(spuncts) / sizeof(char); i++) {
		if(p == spuncts[i]) return spuncts[i];
	}
	return 0;
}

char *checkMultiletterReserved(char *p) {
	// Keyword.
	static char *mkeywords[] = {"return", "if", "else", "while", "int", "char", "sizeof"};
	for(int i = 0; i < sizeof(mkeywords) / sizeof(*mkeywords); i++) {
		if(!strncmp(p, mkeywords[i], strlen(mkeywords[i])) && !isalnum(p[strlen(mkeywords[i])]) && p[strlen(mkeywords[i])] != '_') return mkeywords[i];
	}

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

// If next token's kind is expected kind. Otherwise return false.
Token *consumeKind(TokenKind kind) {
	if(nowToken->kind != kind) return NULL;
	Token *oldToken = nowToken;
	nowToken = nowToken->next;
	return oldToken;
}

// If next token is expected reserved word, read token. Otherwise output error.
void expect(char *op) {
	if(nowToken->kind != TK_RESERVED || strlen(op) != nowToken->len || strncmp(nowToken->str, op, nowToken->len)) {
		if(nowToken->kind == TK_NUM) printf("%d\n", nowToken->val);
		else printf("%.*s\n", nowToken->len, nowToken->str);
		error(nowToken->str, "'%s'ではありません。", op);
	}
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

// If next token is identifier token ,read token and return the token pointer. Otherwise output error.
Token *expectIdentifier() {
	if(nowToken->kind != TK_IDENT) error(nowToken->str, "識別子ではありません。");
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

// Make new string token and connect the token to current.
// Arg "str" has to null end.
Token *newStringToken(Token *current, char *str) {
	Token *token = calloc(1, sizeof(Token));
	token->kind = TK_STR;
	token->str = str;
	token->len = strlen(str);
	current->next = token;
	token->val = nowStringLiteralsNum;
	return token;
}

// Show token lists.
void printTokens() {
	Token *token = nowToken;
	printf("----------- Tokens ---------------\n");
	while(token) {
		if(token->kind == TK_NUM) printf("%d\n", token->val);
		else if(token->kind == TK_EOF) printf("EOF\n");
		else printf("%.*s\n", token->len, token->str);
		token = token->next;
	}
	printf("----------------------------------\n");
}

// Show string literals.
void printStringLiterals() {
	printf("----------- String ---------------\n");
	for(int i = 0; i < nowStringLiteralsNum; i++) printf("%s len: %ld\n", stringLiterals[i], strlen(stringLiterals[i]));
	printf("----------------------------------\n");
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

		// Skip line comment.
		if(!strncmp(p, "//", 2)) {
			p += 2;
			while(*p != '\n') p++;
			continue;
		}

		// Skipt block comment.
		if(!strncmp(p, "/*", 2)) {
			char *q = strstr(p + 2, "*/");
			if(!q) error(p, "コメントが閉じられていません。");
			p = q + 2;
			continue;
		}

		// Multi letter reserved. ex) "<=", ">="
		char *ch = checkMultiletterReserved(p);
		if(ch) {
			int len = strlen(ch);
			if(!strncmp(ch, "return", 6)) {
				current = newToken(TK_RETURN, current, p, len);
			} else if(!strncmp(ch, "if", 2)) {
				current = newToken(TK_IF, current, p, len);
			} else if(!strncmp(ch, "else", 4)) {
				current = newToken(TK_ELSE, current, p, len);
			} else if(!strncmp(ch, "while", 5)) {
				current = newToken(TK_WHILE, current, p, len);
			} else if(!strncmp(ch, "sizeof", 6)) {
				current = newToken(TK_SIZEOF, current, p, len);
			} else {
				current = newToken(TK_RESERVED, current, p, len);
			}
			p += len;
			continue;
		}
		
		char c = checkSingleletterReserved(*p);
		
		// string literal
		if(c == '"') {
			p++;
			char *s = p;
			do {
				c = *p++;
			} while(c != '"');
			int len = p - s - 1;
			stringLiterals[nowStringLiteralsNum] = (char *)malloc(sizeof(char) * len + 1);
			memcpy(stringLiterals[nowStringLiteralsNum], s, len);
			current = newStringToken(current, stringLiterals[nowStringLiteralsNum]);
			nowStringLiteralsNum++;
			continue;
		}

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

