#include "util.h"

void error(char *loc, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	if(loc) {
		int pos = loc - userInput;

		fprintf(stderr, "%s\n", userInput);
		for(; pos > 0; pos--) fprintf(stderr, " ");
		fprintf(stderr, "^\n");
		vfprintf(stderr, fmt, ap);
		fprintf(stderr, "\n");
	} else {
		vfprintf(stderr, fmt, ap);
		fprintf(stderr, "\n");
	}

	exit(EXIT_FAILURE);
}
