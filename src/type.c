#include "type.h"

// Make new type.
Type *newType(TypeKind kind) {
	Type *type = calloc(1, sizeof(Type));
	type->kind = kind;
	return type;
}

