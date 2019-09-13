#include <stdio.h>
#include <stdlib.h>

void allocs(int **p, int num1, int num2, int num3, int num4) {
	*p = (int *)malloc(sizeof(int) * 4);
	(*p)[0] = num1;
	(*p)[1] = num2;
	(*p)[2] = num3;
	(*p)[3] = num4;
}

