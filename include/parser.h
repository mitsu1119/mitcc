#pragma once
#include "AST.h"
#include "lexer.h"
#include "type.h"

typedef struct AST AST;
typedef struct Type Type;
typedef struct Token Token;

// Label counter.
int labelCnt;

typedef struct Func Func;
struct Func {
	Func *next;		// Next function.
	char *name;		// Function name.
	int len;		// Name length.
	AST *body;		// Main part.
	AST *arg;		// Arguments list.
};
Func *funcs;

typedef struct LVar LVar;
struct LVar {
	LVar *next;		// Next local variable.
	Type *type;		// Lvar type.
	char *name;		// Lvar name.
	int len;		// Name length.
	int offset;		// Offset from rbp.
};
LVar *lvars;

void program();
void declare_func();
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
AST *declare_args();
AST *declare_argsp();

// Search local variable.
LVar *searchLVar(Token *token);

// Search function.
Func *searchFunc(Token *token);

