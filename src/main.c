#include <stdio.h>
#include "gen.h"

// extern Token *nowToken;
int main(int argc, char *argv[]) {
	if(argc < 3) {
		printf("引数が足りません。");
		return -1;
	}
	loadInput(argv[1], argv[2]);
	codeGen();

	return 0;
}
