#pragma once
#include "AST.h"
#include "lexer.h"

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

// Search local variable.
LVar *searchLVar(Token *token);

// Evaluate the lvalue. If ast type is AST_LVAR, calculate the local variable address and push. Otherwise output error.
void genLval(AST *ast);

// Generate assembly code from ast.
void genStack(AST *ast);

