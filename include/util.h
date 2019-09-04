#pragma once
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

char userInput[0x100];

void error(char *loc, char *fmt, ...);

