#pragma once
#include "AST.h"

typedef struct AST AST;

// Type data
typedef enum {
	TY_INT,
	TY_CHAR,
	TY_PTR,
	TY_ARRAY
} TypeKind;

typedef struct Type Type;
struct Type {
	TypeKind kind;
	Type *ptr;
	int size;
	unsigned int arraySize;
};

// Make new type.
Type *newType(TypeKind kind);

// Pointer type.
bool isPointerType(Type *type);

// Set type size.
void setTypeSize(Type *type);

// Add type for AST.
void addType(AST *ast);

