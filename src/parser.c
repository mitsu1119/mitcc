#include "parser.h"

void program() {
	labelCnt = 0;

	int i = 0;
	while(!isEOF()) {
		code[i++] = statement();
	}
	code[i] = NULL;
}

AST *statement() {
	AST *ast;
	if(consumeKind(TK_RETURN)) {
		ast = newAST(AST_RETURN, expr(), NULL);
		expect(";");
		return ast;
	} else if(consumeKind(TK_IF)) {
		expect("(");
		ast = newAST(AST_IF, NULL, NULL);
		ast->cond = expr();
		expect(")");
		ast->lhs = statement();
		return ast;
	} else {
		ast = expr();
		expect(";");
	}

	return ast;
}

AST *expr() {
	return assign();
}

AST *assign() {
	AST *ast = equality();
	if(consume("=")) ast = newAST(AST_ASSIGN, ast, assign());
	return ast;
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

	Token *token = consumeIdentifier();
	if(token) {
		ast = calloc(1, sizeof(AST));
		ast->type = AST_LVAR;
		LVar *lvar = searchLVar(token);
		if(lvar) {
			ast->offset = lvar->offset;
		} else {
			lvar = calloc(1, sizeof(LVar));
			lvar->next = lvars;
			lvar->name = token->str;
			lvar->len = token->len;	
			if(lvars) lvar->offset = lvars->offset + 8;
			else lvar->offset = 8;
			ast->offset = lvar->offset;
			lvars = lvar;
		}
		return ast;
	}

	ast = newNumAST(expectNumber());
	return ast;
}

// Search local variable.
LVar *searchLVar(Token *token) {
	for(LVar *var = lvars; var; var = var->next) {
		if(var->len == token->len && !strncmp(token->str, var->name, token->len)) return var;
	}
	return NULL;
}

// Evaluate the lvalue. If ast type is AST_LVAR, calculate the local variable address and push. Otherwise output error.
void genLval(AST *ast) {
	if(ast->type != AST_LVAR) error(0, "代入の左辺値が不正です。");
	printf("	mov rax, rbp\n");
	printf("	sub rax, %d\n", ast->offset);
	printf("	push rax\n");
}

void genStack(AST *ast) {
	int labelBuf;

	switch(ast->type) {
	case AST_NUM:
		printf("	push %d\n", ast->val);
		return;
	case AST_ASSIGN:
		genLval(ast->lhs);
		genStack(ast->rhs);
		printf("	pop rdi\n");
		printf("	pop rax\n");
		printf("	mov [rax], rdi\n");
		printf("	push rdi\n");
		return;
	case AST_LVAR:
		genLval(ast);
		printf("	pop rax\n");
		printf("	mov rax, [rax]\n");
		printf("	push rax\n");
		return;
	case AST_IF:
		genStack(ast->cond);
		printf("	pop rax\n");
		printf("	cmp rax, 0\n");
		printf("	je .Lifend%d\n", labelCnt);
		labelBuf = labelCnt++;
		genStack(ast->lhs);
		printf(".Lifend%d:\n", labelBuf);
		return;
	case AST_RETURN:
		genStack(ast->lhs);
		printf("	pop rax\n");
		printf("	mov rsp, rbp\n");
		printf("	pop rbp\n");
		printf("	ret\n");
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
