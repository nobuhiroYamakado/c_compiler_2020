#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// tokenize.c
//

typedef enum {
	TK_RESERVED, //keywords or punctuators
	TK_NUM, //integer literals
	TK_EOF, //end-of-file markers
} TokenKind;

// Token type
typedef struct Token Token;

struct Token {
	TokenKind kind;		//token kind
	Token *next;		//next token
	int val;			//if kind is TK_NUM, its value
	char *str;			//token string
	char *loc;			//token location
	int len;			//token length
};


//Reports an error and exit.
void error(char *fmt, ...);

void error_tok(Token *tok, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
Token *tokenize(char *input);


//
// parse.c
//

typedef enum {
	ND_ADD, //+
	ND_SUB, //-
	ND_MUL, //*
	ND_DIV, ///
	
	ND_EQ,	//==
	ND_NE,	//!=
	ND_LT,	//<
	ND_LE,	//<=
	
	ND_RT,	//>
	ND_RE,	//>=

	ND_NUM, //integer

	ND_EXPR_STMT, //Expression statment
} NodeKind;


// Node type
typedef struct Node Node;

struct Node {
	NodeKind kind;		//node kind
	Node *next;			//next node
	Node *lhs;			//left-hand side
	Node *rhs;			//right-hand side
	int val;			//when kind is ND_NUM
};

Node *parse(Token *tok);


//
//codegen.c
//

//void gen(Node *node);
void codegen(Node *node);
