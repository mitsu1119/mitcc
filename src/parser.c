#include "parser.h"

AST *expr() {
	return equality();
}

AST *equality() {
	AST *ast = inequality();
	while(true) {
		if(consume("==")) ast = newAST(AST_EQ, ast, inequality());
		else return ast;
	}
}

AST *inequality() {
	AST *ast = polynomial();
	while(true) {
		if(consume("<=")) ast = newAST(AST_LESSEQ, ast, polynomial());
		else if(consume("<")) ast = newAST(AST_LESS, ast, polynomial());
		else if(consume(">=")) ast = newAST(AST_LESSEQ, polynomial(), ast);
		else if(consume(">")) ast = newAST(AST_LESS, polynomial(), ast);
		else return ast;
	}
}

AST *polynomial() {
	AST *ast = term();
	while(true) {
		if(consume("+")) ast = newAST(AST_ADD, ast, term());
		else if(consume("-")) ast = newAST(AST_SUB, ast, term());
		else return ast;
	}
}

AST *term() {
	AST *ast = sign();
	while(true) {
		if(consume("*")) ast = newAST(AST_MUL, ast, sign());
		else if(consume("/")) ast = newAST(AST_DIV, ast, sign());
		else return ast;
	}
}

AST *sign() {
	AST *ast;
	if(consume("-")) {
		ast = newAST(AST_SUB, newNumAST(0), factor());
	} else {
		consume("+");
		ast = factor();
	}
	return ast;
}

AST *factor() {
	AST *ast;
	if(consume("(")) {
		ast = expr();
		expect(")");
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
	switch(ast->type) {
	case AST_ADD:
		printf("	add rax, rdi\n");
		break;
	case AST_SUB:
		printf("	sub rax, rdi\n");
		break;
	case AST_MUL:
		printf("	imul rax, rdi\n");
		break;
	case AST_DIV:
		printf("	cqo\n");
		printf("	idiv rdi\n");
		break;
	case AST_LESS:
		printf("	cmp rax, rdi\n");
		printf("	setl al\n");
		printf("	movzb rax, al\n");
		break;
	case AST_LESSEQ:
		printf("	cmp rax, rdi\n");
		printf("	setle al\n");
		printf("	movzb rax, al\n");
		break;
	case AST_EQ:
		printf("	cmp rax, rdi\n");
		printf("	sete al\n");
		printf("	movzb rax, al\n");
		break;
	}
	printf("	push rax\n");
}
