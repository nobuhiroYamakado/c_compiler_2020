#include "chibicc.h"

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


//create a new token and add it as the next token of "cur".
Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return (tok);
}
bool startswith(char *p, char *q)
{
	return (memcmp(p,q,strlen(q)) == 0);
}

//Tokenize "user_input" and returns new tokens.
Token *tokenize(char *user_input)
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
		//2digits punctuator
		if (startswith(p,"==") || startswith(p, "!=") ||
				startswith(p, "<=") || startswith(p, ">="))
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}
		//1digit punctuator
		if (*p == '+' || *p == '-' || *p == '*' || *p == '/'
				|| *p == '(' || *p == ')' || *p == '<' || *p == '>')
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

