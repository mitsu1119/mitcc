#pragma once
#include "AST.h"
#include "lexer.h"
#include "type.h"

typedef struct AST AST;
typedef struct Type Type;
typedef struct Token Token;

// Label counter.
int labelCnt;

typedef struct Var Var;
typedef struct Func Func;
struct Func {
	Func *next;		// Next function.
	char *name;		// Function name.
	int len;		// Name length.
	AST *body;		// Main part.
	AST *arg;		// Arguments list.
	Var *lvars;		// Local variables.
};
Func *funcs;

struct Var {
	Var *next;		// Next variable.
	Type *type;		// Lvar type.
	char *name;		// Lvar name.
	int len;		// Name length.
	int offset;		// Offset from rbp (for local variable).
};
Var *lvars;	// local variables
Var *gvars;	// global variables

void program();
void declare();
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
Type *typep();

// Search local variable.
Var *searchLVar(Token *token);

// Make local variable and add lvars.
Var *makeLVar(Token *token, Type *type);

// Change lvar to array type.
void toArray(Var *var);

// Search global variable.
Var *searchGVar(Token *token);

// Search function.
Func *searchFunc(Token *token);

