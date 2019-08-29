#pragma once
#include "AST.h"
#include "lexer.h"
#include "gen.h"

AST *code[100];

typedef struct LVar LVar;
struct LVar {
	LVar *next;		// Next local variable.
	char *name;		// Lvar name.
	int len;		// Name length.
	int offset;		// Offset from rbp.
};

LVar *lvars;

void program();
AST *statement();
AST *expr();
AST *assign();
AST *equality();
AST *inequality();
AST *polynomial();
AST *term();
AST *sign();
AST *factor();
AST *args();
AST *argsp();

// Search local variable.
LVar *searchLVar(Token *token);

