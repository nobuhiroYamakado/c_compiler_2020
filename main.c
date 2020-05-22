#include "chibicc.h"
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
bool consume(char *op)
{
	if (token->kind != TK_RESERVED ||
			strlen(op) != token->len ||
			memcmp(token->str, op, token->len))
		return (false);
	token = token->next;
	return (true);
}

//ensure that the current token is "op".
void expect(char *op)
{
	if(token->kind != TK_RESERVED ||
			strlen(op) != token->len ||
			memcmp(token->str, op, token->len))
		error_at(token->str,"expected '%s'",op);
	token = token->next;
}

//ensure that the current token is TK_MUM
int expect_number()
{
	if(token->kind != TK_NUM)
		error_at(token->str, "expected a number//expect_number");
	int val = token->val;
	token = token->next;
	return (val);
}

bool at_eof()
{
	return token->kind == TK_EOF;
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

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val)
{
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

//再帰的に使い合うので宣言しておく

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Node *expr()
{
	return (equality());
}

Node *equality()
{
	Node *node = relational();

	for (;;)
	{
		if (consume("=="))
			node = new_node(ND_EQ, node, relational());
		else if (consume("!="))
			node = new_node(ND_NE, node, relational());
		else
			return (node);
	}
}

Node *relational()
{
	Node *node = add();
	for (;;)
	{
		if (consume("<"))
			node = new_node(ND_LT, node, add());
		else if (consume("<="))
			node = new_node(ND_LE, node, add());
		else if (consume(">"))
			node = new_node(ND_LT, add(), node);
		else if (consume(">="))
			node = new_node(ND_LE, add(), node);
		else
			return (node);
	}
}

Node *add()
{
	Node *node = mul();

	for (;;)
	{
		if (consume("+"))
			node = new_node(ND_ADD, node, mul());
		else if (consume("-"))
			node = new_node(ND_SUB, node, mul());
		else
			return (node);
	}
}

Node *mul()
{
	Node *node = unary();
	for (;;)
	{
		if (consume("*"))
			node = new_node(ND_MUL, node, unary());
		else if (consume("/"))
			node = new_node(ND_DIV, node, unary());
		else
			return (node);
	}
}

Node *unary()
{
	if (consume("+"))
		return (primary());
	if (consume("-"))
		return (new_node(ND_SUB, new_node_num(0), primary()));
	return (primary());
}

Node *primary()
{
	//次のトークンが'('なら、"(" expr ")"のはず
	if (consume("("))
	{
		Node *node = expr();
		expect(")");
		return node;
	}
	
	//そうでないときは数字が渡されるはず
	return new_node_num(expect_number());
}

void gen(Node *node)
{
	if(node->kind == ND_NUM)
	{
		printf("  push %d\n", node->val);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("  pop rdi\n");
	printf("  pop rax\n");

	switch (node->kind)
	{
		case ND_ADD:
			printf("  add rax, rdi\n");
			break;
		case ND_SUB:
			printf("  sub rax, rdi\n");
			break;
		case ND_MUL:
			printf("  imul rax, rdi\n");
			break;
		case ND_DIV:
			printf("  cqo\n");
			printf("  idiv rdi\n");
			break;
		case ND_EQ:
			printf("  cmp rax, rdi\n");
			printf("  sete al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_NE:
			printf("  cmp rax, rdi\n");
			printf("  setne al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_LT:
			printf("  cmp rax, rdi\n");
			printf("  setl al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_LE:
			printf("  cmp rax, rdi\n");
			printf("  setle al\n");
			printf("  movzb rax, al\n");
			break;
	}
	printf("  push rax\n");
}
int main(int argc, char **argv)
{
	if (argc != 2)
	{
		error("%s: invalid number of argments", argv[0]);
		return (1);
	}

	user_input = argv[1];
	token = tokenize(user_input);
	Node *node = expr();
	
	//アセンブリの前半部分
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	
	//抽象構文木を下りながらコード生成
	gen(node);

	//スタックトップに敷き全体の値が残っているはずなので、
	//それをRAXにおｒ−どして関数からの返り値とする。
	printf("  pop rax\n");
	printf("  ret\n");
	return (0);
}
