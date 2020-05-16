
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	TK_RESERVED, //keywords or punctuators
	TK_NUM, //integer literals
	TK_EOF, //end-of-file markers
} TokenKind;

typedef enum {
	ND_ADD, //+
	ND_SUB, //-
	ND_MUL, //*
	ND_DIV, ///
	ND_NUM, //integer
} NodeKind

// Token type
typedef struct Token Token;

struct Token {
	TokenKind kind;		//token kind
	Token *next;		//next token
	int val;			//if kind is TK_NUM, its value
	char *str;			//token string
};

// Node type
typedef struct Node Node;

struct Node {
	NodeKind kind;		//node kind
	Node *lhs;			//left-hand side
	Node *rhs;			//right-hand side
	int val;			//when kind is ND_NUM
}

//input program
char *user_input;

//current token
Token *token;

//Reports an error and exit.
void error(char *fmt, ...)
{
	va_list ap;
	va_start(ap,fmt);
	vfprintf(stderr,fmt,ap);
	fprintf(stderr, "\n");
	exit(1);
}

//reports an error location and exit.
void error_at(char *loc, char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, "");//print pos spaces.
	fprintf(stderr, "^ ");
	vfprintf(stderr,fmt,ap);
	fprintf(stderr, "\n");
	exit(1);
}
//consumes the current token if it matchs "op".
bool consume(char op)
{
	if (token->kind != TK_RESERVED || token->str[0] != op)
		return (false);
	token = token->next;
	return (true);
}

//ensure that the current token is "op".
void expect(char op)
{
	if(token->kind != TK_RESERVED || token ->str[0] != op)
		error_at(token->str,"expected '%c'",op);
	token = token->next;
}

//ensure that the current token is TK_MUM
int expect_number()
{
	if(token->kind != TK_NUM)
		error_at(token->str, "expected a number");
	int val = token->val;
	token = token->next;
	return (val);
}

bool at_eof()
{
	return token->kind == TK_EOF;
}

//create a new token and add it as the next token of "cur".
Token *new_token(TokenKind kind, Token *cur, char *str)
{
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return (tok);
}

//Tokenize "user_input" and returns new tokens.
Token *tokenize()
{
	char *p = user_input;
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p)
	{
		//skip whitespace characters.
		if (isspace(*p))
		{
			p++;
			continue;
		}

		//punctuator
		if (*p == '+' || *p == '-')
		{
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		//integer literal
		if (isdigit(*p))
		{
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error_at(p,"expected a number");
	}

	new_token(TK_EOF, cur, p);
	return (head.next);
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		error("%s: invalid number of argments", argv[0]);
		return (1);
	}

	user_input = argv[1];
	token = tokenize();

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	//the first token must be a number
	printf("  mov rax, %d\n", expect_number());

	//... followed by either "+<number>" or "-<number>"
	while (!at_eof())
	{
		if (consume('+'))
		{
			printf("  add rax, %d\n", expect_number());
			continue;
		}
		expect('-');
		printf("  sub rax, %d\n", expect_number());
	}

	printf("  ret\n");
	return (0);
}
