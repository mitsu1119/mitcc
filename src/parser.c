#include "parser.h"

void program() {
	labelCnt = 0;

	while(!isEOF()) {
		declare();
	}
}

void declare() {
	Type *vartype = typep();

	Token *token = expectIdentifier();
	if(!vartype) error(token->str, "'%.*s'の型がありません。", token->len, token->str);

	// Function definition
	if(consume("(")) {
		AST *argAST = NULL;
		if(!consume(")")) {
			argAST = declare_args();
			expect(")");
		}
		
		expect("{");
		AST *ast = newAST(AST_BLOCK, NULL, NULL);
		AST *block = ast;
		while(!consume("}")) {
			block->lhs = statement();
			block->rhs = newAST(AST_BLOCK, NULL, NULL);
			block = block->rhs;
		}
		Func *func = searchFunc(token);
		if(func) {
			error(token->str, "'%.*s'が再定義されています。", token->len, token->str);
		} else {
			func = calloc(1, sizeof(Func));
			func->next = funcs;
			func->name = token->str;
			func->len = token->len;
			func->body = ast;
			func->arg = argAST;
			func->lvars = lvars;	
			lvars = NULL;
			funcs = func;
		}
	} else {
		// Global variable
		Var *gvar = calloc(1, sizeof(Var));
		gvar->next = gvars;
		gvar->name = token->str;
		gvar->len = token->len;
		gvar->type = vartype;
		setTypeSize(gvar->type);
		gvars = gvar;

		if(consume("[")) {
			toArray(gvar);
			expect("]");
		}
		expect(";");
	}
}

AST *statement() {
	AST *ast;	
	Type *lvartype = typep();
	if(lvartype) {
		Token *token = expectIdentifier();
		ast = newAST(AST_LVAR, NULL, NULL);
		Var *lvar = searchLVar(token);
		if(lvar) {
			error(token->str, "'%.*s'が複数回宣言されています。", token->len, token->str);
		}
		ast->var = makeLVar(token, lvartype);

		if(consume("[")) {
			toArray(ast->var);
			expect("]");
		} else {
			if(consume("=")) {
				ast = newAST(AST_ASSIGN, ast, assign());
			}
		}
		expect(";");
		return ast;
	} else if(consume("{")) {
		ast = newAST(AST_BLOCK, NULL, NULL);
		AST *block = ast;
		while(!consume("}")) {
			block->lhs = statement();
			block->rhs = newAST(AST_BLOCK, NULL, NULL);
			block = block->rhs;
		}
		return ast;
	} else if(consumeKind(TK_RETURN)) {
		ast = newAST(AST_RETURN, expr(), NULL);
		expect(";");
		return ast;
	} else if(consumeKind(TK_IF)) {
		expect("(");
		ast = newAST(AST_IF, NULL, NULL);
		ast->cond = expr();
		expect(")");
		ast->lhs = statement();		// Then statement.
		if(consumeKind(TK_ELSE)) ast->rhs = statement();	// Else statement.
		return ast;
	} else if(consumeKind(TK_WHILE)) {
		expect("(");
		ast = newAST(AST_WHILE, NULL, NULL);
		ast->cond = expr();
		expect(")");
		ast->lhs = statement();
	} else {
		ast = expr();
		expect(";");
	}

	return ast;
}

AST *expr() {
	return assign();
}

AST *assign() {
	AST *ast;
	ast = equality();
	if(consume("=")) ast = newAST(AST_ASSIGN, ast, assign());
	return ast;
}

AST *equality() {
	AST *ast = inequality();
	while(true) {
		if(consume("==")) ast = newAST(AST_EQ, ast, inequality());
		else return ast;
	}
}

AST *inequality() {
	AST *ast = polynomial();
	while(true) {
		if(consume("<=")) ast = newAST(AST_LESSEQ, ast, polynomial());
		else if(consume("<")) ast = newAST(AST_LESS, ast, polynomial());
		else if(consume(">=")) ast = newAST(AST_LESSEQ, polynomial(), ast);
		else if(consume(">")) ast = newAST(AST_LESS, polynomial(), ast);
		else return ast;
	}
}

AST *polynomial() {
	AST *ast = term();
	AST *right;
	while(true) {
		if(consume("+")) {
			right = term();
			addType(ast);
			addType(right);
			if(!isPointerType(ast->ty) && !isPointerType(right->ty)) ast = newAST(AST_ADD, ast, right);
			else if(isPointerType(ast->ty) && !isPointerType(right->ty)) ast = newAST(AST_PTRADD, ast, right);
			else if(!isPointerType(ast->ty) && isPointerType(right->ty)) ast = newAST(AST_PTRADD, right, ast);
			else error(nowToken->str, "'%.*s'不正なオペランドです。", nowToken->len, nowToken->str);
		} else if(consume("-")) {
			right = term();
			addType(ast);
			addType(right);
			if(!isPointerType(ast->ty) && !isPointerType(right->ty)) ast = newAST(AST_SUB, ast, right);
			else if(isPointerType(ast->ty) && !isPointerType(right->ty)) ast = newAST(AST_PTRSUB, ast, right);
			else if(isPointerType(ast->ty) && isPointerType(right->ty)) ast = newAST(AST_PTRDIFF, ast, right);
			else error(nowToken->str, "'%.*s'不正なオペランドです。", nowToken->len, nowToken->str);
		} else {
			return ast;
		}
	}
}

AST *term() {
	AST *ast = sign();
	while(true) {
		if(consume("*")) ast = newAST(AST_MUL, ast, sign());
		else if(consume("/")) ast = newAST(AST_DIV, ast, sign());
		else return ast;
	}
}

AST *sign() {
	AST *ast;
	if(consume("-")) {
		ast = newAST(AST_SUB, newNumAST(0), factor());
	} else if(consume("+")) {
		ast = factor();
	} else if(consume("*")) {
		ast = newAST(AST_DEREF, sign(), NULL);
	} else if(consume("&")) {
		ast = newAST(AST_ADDR, sign(), NULL);
	} else if(consumeKind(TK_SIZEOF)) {
		ast = sign();
		addType(ast);
		ast = newNumAST(ast->ty->size);
	} else {
		ast = factor();
		if(consume("[")) {
			AST *right = expr();
			addType(ast);
			addType(right);
			if(!isPointerType(ast->ty) && !isPointerType(right->ty)) ast = newAST(AST_ADD, ast, right);
			else if(isPointerType(ast->ty) && !isPointerType(right->ty)) ast = newAST(AST_PTRADD, ast, right);
			else if(!isPointerType(ast->ty) && isPointerType(right->ty)) ast = newAST(AST_PTRADD, right, ast);
			else error(nowToken->str, "'%.*s'不正なオペランドです。", nowToken->len, nowToken->str);
			addType(ast);
			ast = newAST(AST_DEREF, ast, NULL);
			addType(ast);
			expect("]");
		}
	}
	return ast;
}

AST *factor() {
	AST *ast;
	Token *token = consumeKind(TK_STR);
	if(token) {
		ast = newAST(AST_STR, NULL, NULL);
		ast->val = token->val;
		return ast;
	}

	if(consume("(")) {
		ast = expr();
		expect(")");
		return ast;
	}

	token = consumeIdentifier();
	if(token) {
		ast = calloc(1, sizeof(AST));
		if(consume("(")) {
			ast->type = AST_CALL;
			ast->calledFunc = token;
			ast->lhs = NULL;
			if(!consume(")")) {
				ast->lhs = args();
				expect(")");
			}
		} else {
			Var *var = searchLVar(token);
			if(var) {
				ast->type = AST_LVAR;
				ast->var = var;
			} else {
				var = searchGVar(token);
				if(var) {
					ast->type = AST_GVAR;
					ast->var = var;
				} else {
					error(token->str, "変数'%.*s'が宣言されていません。", token->len, token->str);
				}
			}
		}
		return ast;
	}

	ast = newNumAST(expectNumber());
	return ast;
}

AST *args() {
	AST *ast = newAST(AST_ARGS, expr(), NULL);
	ast->rhs = argsp();
	return ast;
}

AST *argsp() {
	AST *ast = newAST(AST_ARGS, NULL, NULL);
	if(!consume(",")) {
		return ast;
	} 
	ast->lhs = expr();
	ast->rhs = argsp();

	return ast;
}

AST *declare_args() {
	Type *lvartype = typep();

	Token *arg = expectIdentifier();
	if(!lvartype) error(arg->str, "'%.*s'に型がありません。", arg->len, arg->str);

	AST *ast = newAST(AST_LVAR, NULL, NULL);
	Var *lvar = searchLVar(arg);
	if(lvar) {
		ast->var = lvar;
	} else {
		ast->var = makeLVar(arg, lvartype);
	}
	AST *ret = newAST(AST_ARGDECS, ast, declare_argsp());
	return ret;
}

AST *declare_argsp() {
	AST *ret = newAST(AST_ARGDECS, NULL, NULL);
	if(!consume(",")) {
		return ret;
	}

	Type *lvartype = typep();

	Token *arg = expectIdentifier();
	if(!lvartype) error(arg->str, "'%.*s'に型がありません。", arg->len, arg->str);

	Var *lvar = searchLVar(arg);
	AST *ast = newAST(AST_LVAR, NULL, NULL);
	if(lvar) {
		ast->var = lvar;
	} else {
		ast->var = makeLVar(arg, lvartype);
	}
	ret->lhs = ast;
	ret->rhs = declare_argsp();
	return ret;
}

Type *typep() {	
	TypeKind kind;
	
	if(consume("int")) kind = TY_INT;
	else if(consume("char")) kind = TY_CHAR;
	else return NULL;

	Type *vartype = calloc(1, sizeof(Type));
	Type *typeBuf = vartype;
	while(consume("*")) {
		typeBuf->kind = TY_PTR;
		setTypeSize(typeBuf);
		typeBuf->ptr = calloc(1, sizeof(Type));
		typeBuf = typeBuf->ptr;
	}
	typeBuf->kind = kind;
	setTypeSize(typeBuf);
	setTypeSize(vartype);

	return vartype;
}

// Search local variable.
Var *searchLVar(Token *token) {
	for(Var *var = lvars; var; var = var->next) {
		if(var->len == token->len && !strncmp(token->str, var->name, token->len)) return var;
	}
	return NULL;
}

// Make local variable and add lvars.
Var *makeLVar(Token *token, Type *type) {
	Var *lvar = calloc(1, sizeof(Var));
	lvar->next = lvars;
	lvar->name = token->str;
	lvar->len = token->len;
	lvar->type = type;
	if(lvars) {
		lvar->offset = lvars->offset + 8;
	} else {
		lvar->offset = 8;
	}
	lvars = lvar;
	return lvar;
}

// Change lvar to array type.
void toArray(Var *var) {
	Type *buf = newType(TY_PTR);
	buf->ptr = var->type;
	var->type = buf;
	var->type->arraySize = expectNumber();
	var->type->kind = TY_ARRAY;
	var->offset += var->type->ptr->size * (var->type->arraySize - 1);
	setTypeSize(var->type);
}

// Search global variable.
Var *searchGVar(Token *token) {
	for(Var *var = gvars; var; var = var->next) {
		if(var->len == token->len && !strncmp(token->str, var->name, token->len)) return var;
	}
	return NULL;
}

// Search function.
Func *searchFunc(Token *token) {
	for(Func *func = funcs; func; func = func->next) {
		if(func->len == token->len && !strncmp(token->str, func->name, token->len)) return func;
	}
	return NULL;
}

