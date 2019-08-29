#pragma once
#include "lexer.h"
#include "parser.h"
#include "AST.h"

// Label counter.
int labelCnt;

// Evaluate the lvalue. If ast type is AST_LVAR, calculate the local variable address and push. Otherwise output error.
void genLval(AST *ast);

// Generate assembly code from ast.
void genStack(AST *ast);
