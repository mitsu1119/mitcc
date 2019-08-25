#include <stdio.h>
#include "lexer.h"
#include "util.h"

// extern Token *nowToken;
int main(int argc, char *argv[]) {
	if(argc != 2) {
		error(0, "引数の数が正しくありません。");
		return 1;
	}

	userInput = argv[1];
	nowToken = lexer(userInput);

	// Output assembly base.
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	printf("	mov rax, %d\n", expectNumber());

	while(!isEOF()) {
		if(consume('+')) {
			printf("	add rax, %d\n", expectNumber());
			continue;
		}
		
		expect('-');
		printf("	sub rax, %d\n", expectNumber());
	}

	printf("	ret\n");

	return 0;
}
