#pragma once
#include "AST.h"

typedef struct AST AST;

// Type data
typedef enum {
	TY_INT,
	TY_PTR,
	TY_ARRAY
} TypeKind;

typedef struct Type Type;
struct Type {
	TypeKind kind;
	Type *ptr;
	unsigned int arraySize;
};

// Make new type.
Type *newType(TypeKind kind);

// Add type for AST.
void addType(AST *ast);

