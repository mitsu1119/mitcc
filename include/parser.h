#pragma once
#include "AST.h"
#include "lexer.h"

AST *expr();
AST *mul();
AST *primary();

void genStack(AST *ast);

