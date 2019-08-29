#include "parser.h"

void program() {
	labelCnt = 0;

	int i = 0;
	while(!isEOF()) {
		code[i++] = statement();
	}
	code[i] = NULL;
}

AST *statement() {
	AST *ast;
	if(consume("{")) {
		ast = newAST(AST_LIST, NULL, NULL);
		AST *block = ast;
		while(!consume("}")) {
			block->lhs = statement();
			block->rhs = newAST(AST_LIST, NULL, NULL);
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
			expect(")");
		}
		return ast;
	}

	ast = newNumAST(expectNumber());
	return ast;
}

// Search local variable.
LVar *searchLVar(Token *token) {
	for(LVar *var = lvars; var; var = var->next) {
		if(var->len == token->len && !strncmp(token->str, var->name, token->len)) return var;
	}
	return NULL;
}

