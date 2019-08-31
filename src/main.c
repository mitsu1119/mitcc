#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "gen.h"

// extern Token *nowToken;
int main(int argc, char *argv[]) {
	/*if(argc != 2) {
		error(0, "引数の数が正しくありません。");
		return 1;
	}

	userInput = argv[1];*/

	scanf("%100[^\n]%*[^\n]", userInput);
	nowToken = lexer(userInput);
	program();

	// Output assembly base.
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");

	for(Func *func = funcs; func; func = func->next) {
		printf("%.*s:\n", func->len, func->name);

		// Prologue
		printf("	push rbp\n");
		printf("	mov rbp, rsp\n");
		printf("	sub rsp, 208\n");

		genStack(func->body);

		// Pop expression evaluation result.
		printf("	pop rax\n");


		// Epilogue
		printf("	mov rsp, rbp\n");
		printf("	pop rbp\n");
		printf("	ret\n");
	}

	return 0;
}
