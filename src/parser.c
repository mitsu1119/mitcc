#include "parser.h"


AST *expr() {
	AST *ast = mul();
	while(true) {
		if(consume('+')) ast = newAST(AST_ADD, ast, mul());
		else if(consume('-')) ast = newAST(AST_SUB, ast, mul());
		else return ast;
	}
}

AST *mul() {
	AST *ast = primary();
	while(true) {
		if(consume('*')) ast = newAST(AST_MUL, ast, primary());
		else if(consume('/')) ast = newAST(AST_DIV, ast, primary());
		else return ast;
	}
}

AST *primary() {
	AST *ast;
	if(consume('(')) {
		ast = expr();
		expect(')');
		return ast;
	}
	ast = newNumAST(expectNumber());
	return ast;
}

void genStack(AST *ast) {
	if(ast->type == AST_NUM) {
		printf("	push %d\n", ast->val);
		return;
	}

	genStack(ast->lhs);
	genStack(ast->rhs);

	printf("	pop rdi\n");
	printf("	pop rax\n");
	if(ast->type == AST_ADD) {
		printf("	add rax, rdi\n");
	} else if(ast->type == AST_SUB) {
		printf("	sub rax, rdi\n");
	} else if(ast->type == AST_MUL) {
		printf("	imul rax, rdi\n");
	} else if(ast->type == AST_DIV) {
		printf("	cqo\n");
		printf("	idiv rdi\n");
	}
	printf("	push rax\n");
}
