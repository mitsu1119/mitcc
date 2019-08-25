#pragma once
#include <stdlib.h>

// AST type.
typedef enum {
	AST_ADD, AST_SUB, AST_MUL, AST_DIV, AST_LESS, AST_LESSEQ, AST_EQ, AST_NUM
} ASTType;

// AST sets.
typedef struct AST AST;
struct AST {
	ASTType type;
	AST *lhs, *rhs;
	int val;
};

AST *newAST(ASTType type, AST *lhs, AST *rhs);
AST *newNumAST(int val);

