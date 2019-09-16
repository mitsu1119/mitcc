#pragma once
#include <fcntl.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "type.h"
#include "AST.h"

// Loading codes.
int loadInput(const char *filename, const char *debugMode);

// Registor name list sorted arguments list.
static const char *regNames[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

// Generate assembly code
void codeGen();

// Generate global variable codes.
void genGvarCode();

// Generate a function code.
void genFuncCode(Func *function);

// Evaluate the lvalue or gvalue. If ast type is AST_LVAR, calculate the local variable address and push. Otherwise output error.
void genVar(AST *ast);

// Generate assembly code from ast.
void genStack(AST *ast);
