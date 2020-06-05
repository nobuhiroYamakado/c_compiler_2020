#include "chibicc.h"

// Input string
static char *current_input;

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
static void verror_at(char *loc, char *fmt, va_list ap)
{

	int pos = loc - current_input;
	fprintf(stderr, "%s\n", current_input);
	fprintf(stderr, "%*s", pos, "");//print pos spaces.
	fprintf(stderr, "^ ");
	vfprintf(stderr,fmt,ap);
	fprintf(stderr, "\n");
	exit(1);

}

static void error_at(char *loc, char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	verror_at(loc, fmt, ap);
}

void error_tok(Token *tok, char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	verror_at(tok->loc, fmt, ap);
}

bool equal(Token *tok, char *op)
{
	return (strlen(op) == tok->len &&
			!strncmp(tok->loc, op, tok->len));
}

Token *skip(Token *tok, char *op)
{
	if(!equal(tok, op))
		error_tok(tok, "expected '%s'",op);
	return (tok->next);
}

//create a new token and add it as the next token of "cur".
Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->loc = str;
	tok->len = len;
	cur->next = tok;
	return (tok);
}

bool startswith(char *p, char *q)
{
	return (memcmp(p,q,strlen(q)) == 0);
}

//Tokenize "user_input" and returns new tokens.
Token *tokenize(char *p)
{
	current_input = p;
	
	Token head = {};
	Token *cur = &head;

	while (*p)
	{
		//skip whitespace characters.
		if (isspace(*p))
		{
			p++;
			continue;
		}

		//2digits punctuator
		if (startswith(p,"==") || startswith(p, "!=") ||
				startswith(p, "<=") || startswith(p, ">="))
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}
		
		//1digit punctuator
		if (ispunct(*p))
		{
			cur = new_token(TK_RESERVED, cur, p++,1);
			continue;
		}

		//integer literal
		if (isdigit(*p))
		{
			cur = new_token(TK_NUM, cur, p, 0);
			char *q = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p - q;
			continue;
		}

		error_at(p,"invalid token//tokenize");
	}

	new_token(TK_EOF, cur, p, 0);
	return (head.next);
}

