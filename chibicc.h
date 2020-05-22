
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
	int len;			//token length
};


//Reports an error and exit.
void error(char *fmt, ...);

//reports an error location and exit.
void error_at(char *loc, char *fmt, ...);




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
} NodeKind;


// Node type
typedef struct Node Node;

struct Node {
	NodeKind kind;		//node kind
	Node *lhs;			//left-hand side
	Node *rhs;			//right-hand side
	int val;			//when kind is ND_NUM
};

bool consume(char *op);
void expect(char *op);
int expect_number(void);
bool at_eof(void);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
Token *tokenize(char *user_input);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);


Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();


//
//codegen.c
//

void gen(Node *node);


//grobal//input program
char *user_input;

//current token
Token *token;


