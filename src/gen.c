#include "gen.h"

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
		printf("	call %.*s\n", ast->calledFunc->len, ast->calledFunc->str);
		return;
	case AST_LIST:
		if(ast->lhs != NULL) {
			genStack(ast->lhs);
			printf("	pop rax\n");
			genStack(ast->rhs);
		}
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

