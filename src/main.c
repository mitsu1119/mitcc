#include <stdio.h>
#include "lexer.h"
#include "AST.h"
#include "parser.h"
#include "util.h"

// extern Token *nowToken;
int main(int argc, char *argv[]) {
	if(argc != 2) {
		error(0, "引数の数が正しくありません。");
		return 1;
	}

	userInput = argv[1];
	nowToken = lexer(userInput);
	AST *ast = expr();

	// Output assembly base.
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	genStack(ast);

	printf("	pop rax\n");
	printf("	ret\n");

	return 0;
}
