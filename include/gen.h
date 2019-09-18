#pragma once
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include "lexer.h"
#include "parser.h"
#include "type.h"
#include "AST.h"

// Loading codes.
int loadInput(const char *filename, const char *debugMode);

// Generate assembly code
void codeGen();

// Generate global variable codes.
void genGvarCode();

// Generate string literals.
void genStringLiteralCodes();

// Generate a function code.
void genFuncCode(Func *function);

// Evaluate the lvalue or gvalue. If ast type is AST_LVAR, calculate the local variable address and push. Otherwise output error.
void genVar(AST *ast);

// Generate assembly code from ast.
void genStack(AST *ast);
