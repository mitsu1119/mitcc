#include "parser.h"

void program() {
	labelCnt = 0;

	while(!isEOF()) {
		declare_func();
	}
}

void declare_func() {
	Token *token = expectIdentifier();
	expect("(");
	
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
		error(0, "'%*s'が再定義されています。\n", token->len, token->str);
	} else {
		func = calloc(1, sizeof(Func));
		func->next = funcs;
		func->name = token->str;
		func->len = token->len;
		func->body = ast;
		func->arg = argAST;
		funcs = func;
	}
}

AST *statement() {
	AST *ast;
	if(consume("{")) {
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
	AST *ast = equality();
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
	while(true) {
		if(consume("+")) ast = newAST(AST_ADD, ast, term());
		else if(consume("-")) ast = newAST(AST_SUB, ast, term());
		else return ast;
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
	} else {
		consume("+");
		ast = factor();
	}
	return ast;
}

AST *factor() {
	AST *ast;
	if(consume("(")) {
		ast = expr();
		expect(")");
		return ast;
	}

	Token *token = consumeIdentifier();
	if(token) {
		ast = calloc(1, sizeof(AST));
		if(!consume("(")) {
			ast->type = AST_LVAR;
			LVar *lvar = searchLVar(token);
			if(lvar) {
				ast->offset = lvar->offset;
			} else {
				lvar = calloc(1, sizeof(LVar));
				lvar->next = lvars;
				lvar->name = token->str;
				lvar->len = token->len;	
				if(lvars) lvar->offset = lvars->offset + 8;
				else lvar->offset = 8;
				ast->offset = lvar->offset;
				lvars = lvar;
			}
		} else {
			ast->type = AST_CALL;
			ast->calledFunc = token;
			ast->lhs = NULL;
			if(!consume(")")) {
				ast->lhs = args();
				expect(")");
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
	Token *arg = consumeIdentifier();
	AST *ast = newAST(AST_LVAR, NULL, NULL);
	LVar *lvar = searchLVar(arg);
	if(lvar) {
		ast->offset = lvar->offset;
	} else {
		lvar = calloc(1, sizeof(LVar));
		lvar->next = lvars;
		lvar->name = arg->str;
		lvar->len = arg->len;
		if(lvars) lvar->offset = lvars->offset + 8;
		else lvar->offset = 8;
		ast->offset = lvar->offset;
		lvars = lvar;
	}
	AST *ret = newAST(AST_ARGDECS, ast, declare_argsp());
	return ret;
}

AST *declare_argsp() {
	AST *ret = newAST(AST_ARGDECS, NULL, NULL);
	if(!consume(",")) {
		return ret;
	}
	AST *ast = newAST(AST_LVAR, NULL, NULL);
	Token *arg = expectIdentifier();
	LVar *lvar = searchLVar(arg);
	if(lvar) {
		ast->offset = lvar->offset;
	} else {
		lvar = calloc(1, sizeof(LVar));
		lvar->next = lvars;
		lvar->name = arg->str;
		lvar->len = arg->len;
		if(lvars) lvar->offset = lvars->offset + 8;
		else lvar->offset = 8;
		ast->offset = lvar->offset;
		lvars = lvar;
	}
	ret->lhs = ast;
	ret->rhs = declare_argsp();
	return ret;
}

// Search local variable.
LVar *searchLVar(Token *token) {
	for(LVar *var = lvars; var; var = var->next) {
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

