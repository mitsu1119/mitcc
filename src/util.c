#include "util.h"

void error(char *loc, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	if(loc) {
		char *line = loc;
		while(userInput < line && line[-1] != '\n') line--;

		char *end = loc;
		while(*end != '\n') end++;
		
		int errorLine = 1;
		for(char *s = userInput; s < line; s++) {
			if(*s == '\n') errorLine++;
		}

		int indent = fprintf(stderr, "%d: ", errorLine);
		fprintf(stderr, "%.*s\n", (int)(end - line), line);

		int pos = loc - line + indent;
		fprintf(stderr, "%*s", pos, "");
		fprintf(stderr, "^ ");
		vfprintf(stderr, fmt, ap);
		fprintf(stderr, "\n");
	} else {
		vfprintf(stderr, fmt, ap);
		fprintf(stderr, "\n");
	}

	exit(EXIT_FAILURE);
}
