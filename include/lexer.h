#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include "util.h"

// Token data
// TK_RESERVED	: Reserved word.
// TK_NUM		: Number token.
// TK_EOF		: File end.
typedef enum {
	TK_RESERVED, TK_NUM, TK_EOF
} TokenKind;

// Token type
typedef struct Token Token;
struct Token {
	TokenKind kind;		// Token type.
	Token *next;		// Next input token.
	int val;			// TokenKind: The number (TK_NUM).
	char *str;			// Token string.
	int len;			// Token length.
};

Token *nowToken;

char checkSingleletterReserved(char p);
char *checkMultiletterReserved(char *p);

// If next token is expected reserved word, read token and return true. Otherwise return false.
bool consume(char *op);

// If next token is expected reserved word, read token. Otherwise output error.
void expect(char *op);

// If next token is number token, read token and return the value. Otherwise output error.
int expectNumber();

// Check EOF.
bool isEOF();

// Make new token and connect the token to current.
Token *newToken(TokenKind kind, Token *current, char *str, int len);

// Lexer.
Token *lexer(char *p);

