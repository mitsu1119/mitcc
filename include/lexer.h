#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include "type.h"
#include "util.h"

// Token data
typedef enum {
	TK_RESERVED,	// Reserved work.
	TK_IDENT, 		// Identifier token.
	TK_BLOCK,		// Block token.
	TK_IF,			// If token.
	TK_ELSE,		// If-else token.
	TK_WHILE,		// While token.
	TK_RETURN,		// Return token.
	TK_NUM,			// Number token.
	TK_EOF			// File end.
} TokenKind;

// Token type
typedef struct Token Token;
struct Token {
	TokenKind kind;		// Token type.
	Token *next;		// Next input token.
	char *str;			// Token string.
	int len;			// Token length.
	int val;			// TokenKind: The number (TK_NUM).
};

Token *nowToken;

char checkSingleletterReserved(char p);
char *checkMultiletterReserved(char *p);

// If next token is expected reserved word, read token and return true. Otherwise return false.
bool consume(char *op);

// If next token's kind is expected kind, read token and return true. Otherwise return false.
bool consumeKind(TokenKind kind);

// If next token is expected reserved word, read token. Otherwise output error.
void expect(char *op);

// If next token is number token, read token and return the value. Otherwise output error.
int expectNumber();

// If next token is identifier token, read token and return the token pointer. Otherwise return NULL.
Token *consumeIdentifier();

// If next token is identifier token ,read token and return the token pointer. Otherwise output error.
Token *expectIdentifier();

// Check EOF.
bool isEOF();

// Make new token and connect the token to current.
Token *newToken(TokenKind kind, Token *current, char *str, int len);

// Show token lists.
void printTokens();

// Lexer.
Token *lexer(char *p);

