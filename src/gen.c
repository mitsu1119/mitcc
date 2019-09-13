#include "gen.h"

// Loading codes.
int loadInput(const char *filename, const char *debugMode) {
	int fd = open(filename, O_RDONLY);
	if(fd == -1) exit(1);
	read(fd, userInput, sizeof(userInput) * sizeof(char));
	close(fd);
	nowToken = lexer(userInput);

	if(!strcmp(debugMode, "debug")) printTokens();
	program();
}

// Generate assembly code.
void codeGen() {
	// Output assembly base.
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");

	genFuncCode(funcs);
}

// Generate a function code.
void genFuncCode(Func *function) {
	if(function && function->next) genFuncCode(function->next);
	printf("%.*s:\n", function->len, function->name);

	// Prologue
	printf("	push rbp\n");
	printf("	mov rbp, rsp\n");
	printf("	sub rsp, 208\n");

	// Arguments declaration
	if(function->arg) genStack(function->arg);

	// Main codes
	genStack(function->body);

	// Pop expression evaluation result.
	printf("	pop rax\n");

	// Epilogue
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	printf("	ret\n");
}

// Evaluate the lvalue. If ast type is AST_LVAR, calculate the local variable address and push. Otherwise output error.
void genLval(AST *ast) {
	if(ast->type != AST_LVAR) error(0, "代入の左辺値が不正です。");
	printf("	mov rax, rbp\n");
	printf("	sub rax, %d\n", ast->lvar->offset);
	printf("	push rax\n");
}

void genStack(AST *ast) {
	int labelBuf, cnt = 0;

	switch(ast->type) {
	case AST_ARGDECS:
		while(ast->lhs) {
			genLval(ast->lhs);
			printf("	pop rax\n");
			printf("	mov [rax], %s\n", regNames[cnt]);
			printf("	push %s\n", regNames[cnt++]);
			ast = ast->rhs;
		}
		return;
	case AST_NUM:
		printf("	push %d\n", ast->val);
		return;
	case AST_ADDR:
		genLval(ast->lhs);
		return;
	case AST_DEREF:
		genStack(ast->lhs);
		printf("	pop rax\n");
		printf("	mov rax, [rax]\n");
		printf("	push rax\n");
		return;
	case AST_ASSIGN:
		if(ast->lhs->type != AST_DEREF) genLval(ast->lhs);
		else genStack(ast->lhs->lhs);
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
	case AST_ARGS:
		while(ast->lhs != NULL) {
			genStack(ast->lhs);
			printf("	pop %s\n", regNames[cnt++]);
			ast = ast->rhs;
		}
		return;
	case AST_BLOCK:
		if(ast->lhs != NULL) {
			genStack(ast->lhs);
			printf("	pop rax\n");
			genStack(ast->rhs);
		}
		return;
	case AST_IF:
		genStack(ast->cond);
		printf("	pop rax\n");
		printf("	cmp rax, 0\n");
		if(ast->rhs != NULL) {			// if-else
			printf("	je .Lelse%d\n", labelCnt);
			labelBuf = labelCnt++;
			genStack(ast->lhs);
			printf("	jmp .Lend%d\n", labelCnt);
			printf(".Lelse%d:\n", labelBuf);
			labelBuf = labelCnt++;
			genStack(ast->rhs);
			printf(".Lend%d:\n", labelBuf);
		} else {
			printf("	je .Lend%d\n", labelCnt);
			labelBuf = labelCnt++;
			genStack(ast->lhs);
			printf(".Lend%d:\n", labelBuf);
		}
		return;
	case AST_WHILE:
		printf(".Lbegin%d:\n", labelCnt);
		labelBuf = labelCnt++;
		genStack(ast->cond);
		printf("	pop rax\n");
		printf("	cmp rax, 0\n");
		printf("	je .Lend%d\n", labelCnt);
		genStack(ast->lhs);
		printf("	jmp .Lbegin%d\n", labelBuf);
		printf(".Lend%d:\n", labelCnt++);
		return;
	case AST_RETURN:
		genStack(ast->lhs);
		printf("	pop rax\n");
		printf("	mov rsp, rbp\n");
		printf("	pop rbp\n");
		printf("	ret\n");
		return;	
	case AST_CALL:
		if(ast->lhs != NULL) genStack(ast->lhs);
		printf("	call %.*s\n", ast->calledFunc->len, ast->calledFunc->str);
		printf("	push rax\n");
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
	case AST_PTRADD:
		if(ast->lhs->ty->ptr->kind == TY_INT) printf("	imul rdi, 4\n");
		else printf("	imul rdi, 8\n");
		printf("	add rax, rdi\n");
		break;
	case AST_PTRSUB:
		if(ast->lhs->ty->ptr->kind == TY_INT) printf("	imul rdi, 4\n");
		else printf("	imul rdi, 8\n");
		printf("	sub rax, rdi\n");
		break;
	case AST_PTRDIFF:
		printf("	sub rax, rdi\n");	
		printf("	cqo\n");
		if(ast->lhs->ty->ptr->kind == TY_INT) printf("	mov rdi, 4\n");
		else printf("	mov rdi, 8\n");
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

