#include "AST.h"

AST *newAST(ASTType type, AST *lhs, AST *rhs) {
	AST *ast = calloc(1, sizeof(AST));
	ast->type = type;
	ast->lhs = lhs;
	ast->rhs = rhs;
	return ast;
}

AST *newNumAST(int val) {
	AST *ast = calloc(1, sizeof(AST));
	ast->type = AST_NUM;
	ast->lhs = NULL;
	ast->rhs = NULL;
	ast->val = val;
	return ast;
}

