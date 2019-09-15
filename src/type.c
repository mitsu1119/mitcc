#include "type.h"

// Make new type.
Type *newType(TypeKind kind) {
	Type *type = calloc(1, sizeof(Type));
	type->kind = kind;
	return type;
}

// Add type for ASt.
void addType(AST *ast) {
	if(!ast || ast->ty) return;

	addType(ast->lhs);
	addType(ast->rhs);

	switch(ast->type) {
	case AST_LVAR:
		ast->ty = ast->lvar->type;
		return;
	case AST_ADDR:
		ast->ty = newType(TY_PTR);
		ast->ty->ptr = ast->lhs->ty;
		return;
	case AST_DEREF:
		if(!ast->lhs->ty->ptr) error(nowToken->str, "不正なポインタのデリファレンスです。");
		ast->ty = ast->lhs->ty->ptr;
		return;
	default:
		ast->ty = newType(TY_INT);
		return;
	}
}

