#pragma once
#include "AST.h"
#include "lexer.h"

AST *expr();
AST *equality();
AST *inequality();
AST *polynomial();
AST *term();
AST *sign();
AST *factor();

void genStack(AST *ast);

