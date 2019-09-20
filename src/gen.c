#include "gen.h"

// Registor name list sorted arguments list.
const char *regNames[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

// Loading codes.
int loadInput(const char *filename, const char *debugMode) {
	FILE *fp = fopen(filename, "r");
	if(!fp) error(NULL, "ファイルが開けません。");

	fseek(fp, 0, SEEK_END);
	size_t length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	userInput = (char *)calloc(1, length + 2);
	fread(userInput, length, 1, fp);

	if(length == 0 || userInput[length - 1] != '\n') userInput[length++] = '\n';
	userInput[length] = '\0';
	fclose(fp);

	nowToken = lexer(userInput);

	if(!strcmp(debugMode, "debug")) {	
		printTokens();
		printStringLiterals();
	}

	program();
}

// Generate assembly code.
void codeGen() {
	// Output assembly base.
	printf(".intel_syntax noprefix\n");
	printf(".text\n");

	genGvarCode();

	printf(".section	.rodata\n");
	genStringLiteralCodes();

	printf(".global main\n");
	genFuncCode(funcs);
}

// Generate global variable codes.
void genGvarCode() {
	while(gvars) {
		switch(gvars->type->kind) {
		case TY_CHAR:
			printf(".comm	%.*s, 1, 1\n", gvars->len, gvars->name);
			break;
		case TY_INT:
			printf(".comm	%.*s, 4, 4\n", gvars->len, gvars->name);
			break;
		case TY_PTR:
			printf(".comm	%.*s, 8, 8\n", gvars->len, gvars->name);
			break;
		case TY_ARRAY:
			if(gvars->type->size == 0) printf(".comm	%.*s, %d, 0\n", gvars->len, gvars->name, gvars->type->size);
			else printf(".comm	%.*s, %d, %d\n", gvars->len, gvars->name, gvars->type->size, (int)pow(2, (int)log2(gvars->type->size)));
			break;
		}
		gvars = gvars->next;
	}
}

// Generate string literals.
void genStringLiteralCodes() {
	for(int i = 0; i < nowStringLiteralsNum; i++) {
		printf(".LC%d:\n", i);
		printf("	.string \"%s\"\n", stringLiterals[i]);
	}
}

// Generate a function code.
void genFuncCode(Func *function) {
	if(function && function->next) genFuncCode(function->next);
	printf("%.*s:\n", function->len, function->name);

	// Prologue
	printf("	push rbp\n");
	printf("	mov rbp, rsp\n");

	if(!function->lvars) {
		printf("	sub rsp, 0\n");
	} else {
		int subs = function->lvars->offset;
		if(function->lvars->type->kind == TY_ARRAY && function->lvars->type->arraySize != 0) subs += function->lvars->type->ptr->size * (function->lvars->type->arraySize - 1);

		// 16 byte alignment.
		if((subs & 15) != 0) subs += 16 - subs % 16;
		printf("	sub rsp, %d\n", subs);
	}

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

// Evaluate the lvalue or gvalue. If ast type is AST_LVAR, calculate the local variable address and push. Otherwise output error.
void genVar(AST *ast) {
	if(ast->type != AST_LVAR && ast->type != AST_GVAR) error(nowToken->str, "変数が不正です。");
	if(ast->type == AST_LVAR) {
		printf("	mov rax, rbp\n");
		printf("	sub rax, %d\n", ast->var->offset);
		printf("	push rax\n");
	} else {
		printf("	push offset %.*s\n", ast->var->len, ast->var->name);
	}
}

void genStack(AST *ast) {
	int labelBuf, labelBuf2, cnt = 0;

	switch(ast->type) {
	case AST_ARGDECS:
		while(ast->lhs) {
			genVar(ast->lhs);
			printf("	pop rax\n");
			printf("	mov [rax], %s\n", regNames[cnt]);
			printf("	push %s\n", regNames[cnt++]);
			ast = ast->rhs;
		}
		return;
	case AST_NUM:
		printf("	push %d\n", ast->val);
		return;
	case AST_STR:
		printf("	push offset .LC%d\n", ast->val);
		return;
	case AST_ADDR:
		genVar(ast->lhs);
		return;
	case AST_DEREF:
		genStack(ast->lhs);
		printf("	pop rax\n");
		addType(ast->lhs);
		if(ast->lhs->ty->ptr->kind == TY_CHAR) {	// char type is 4 byte.
			printf("	movsx rax, BYTE PTR [rax]\n");
		} else if(ast->lhs->ty->ptr->kind == TY_INT) {	// int type is 4 byte.
			printf("	movsx rax, DWORD PTR [rax]\n");
		} else {
			printf("	mov rax, QWORD PTR [rax]\n");
		}
		printf("	push rax\n");
		return;
	case AST_ASSIGN:
		if(ast->lhs->type != AST_DEREF) genVar(ast->lhs);
		else genStack(ast->lhs->lhs);
		genStack(ast->rhs);
		addType(ast->rhs);
		printf("	pop rdi\n");
		printf("	pop rax\n");
		if(ast->rhs->ty->size == 1) printf("	mov BYTE PTR [rax], di");
		else if(ast->rhs->ty->size == 4) printf("	mov DWORD PTR [rax], edi\n");
		else printf("	mov QWORD PTR [rax], rdi\n");
		printf("	push rdi\n");
		return;
	case AST_GVAR:
	case AST_LVAR:
		genVar(ast);
		if(ast->var->type->kind != TY_ARRAY) {
			printf("	pop rax\n");
			if(ast->var->type->kind == TY_CHAR) {		// char type is 4 byte.
				printf("	movsx rax, BYTE PTR [rax]\n");
			} else if(ast->var->type->kind == TY_INT) {		// int type is 4 byte.
				printf("	movsx rax, DWORD PTR [rax]\n");			
			} else {
				printf("	mov rax, QWORD PTR [rax]\n");
			}
			printf("	push rax\n");
		}
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
			if(ast->lhs->type == AST_IF || ast->lhs->type == AST_WHILE) ;
			else printf("	pop rax\n");
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
		labelBuf2 = labelCnt++;
		genStack(ast->lhs);
		printf("	jmp .Lbegin%d\n", labelBuf);
		printf(".Lend%d:\n", labelBuf2);
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
		printf("	xor al, al\n");
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
		printf("	imul rdi, %d\n", ast->lhs->ty->ptr->size);
		printf("	add rax, rdi\n");
		break;
	case AST_PTRSUB:
		printf("	imul rdi, %d\n", ast->lhs->ty->ptr->size);
		printf("	sub rax, rdi\n");
		break;
	case AST_PTRDIFF:
		printf("	sub rax, rdi\n");	
		printf("	cqo\n");
		printf("	imul rdi, %d\n", ast->lhs->ty->ptr->size);
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

