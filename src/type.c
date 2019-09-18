#include "type.h"

// Make new type.
Type *newType(TypeKind kind) {
	Type *type = calloc(1, sizeof(Type));
	type->kind = kind;
	type->arraySize = 0;
	setTypeSize(type);
	return type;
}

// Pointer type.
bool isPointerType(Type *type) {
	if(type->ptr) return true;
	switch(type->kind) {
	case TY_INT:
	case TY_CHAR:
		return false;
	default:
		return true;
	}
}

// Set type size.
void setTypeSize(Type *type) {	
	switch(type->kind) {
	case TY_INT:
		type->size = 4;
		break;
	case TY_CHAR:
		type->size = 1;
		break;
	case TY_PTR:
		type->size = 8;
		break;
	case TY_ARRAY:
		setTypeSize(type->ptr);
		type->size = type->ptr->size * type->arraySize;
		break;
	}
}

// Add type for ASt.
void addType(AST *ast) {
	if(!ast || ast->ty) return;

	addType(ast->lhs);
	addType(ast->rhs);

	switch(ast->type) {
	case AST_PTRADD:
	case AST_PTRSUB:
		ast->ty = ast->lhs->ty;
		return;
	case AST_ADDR:
		ast->ty = newType(TY_PTR);
		ast->ty->ptr = ast->lhs->ty;
		return;
	case AST_DEREF:
		if(!ast->lhs->ty->ptr) error(nowToken->str, "不正なポインタのデリファレンスです。");
		ast->ty = ast->lhs->ty->ptr;
		return;
	case AST_GVAR:
	case AST_LVAR:
		ast->ty = ast->var->type;
		return;
	default:
		ast->ty = newType(TY_INT);
		return;
	}
}

