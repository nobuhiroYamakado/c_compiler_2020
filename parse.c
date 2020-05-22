#include "chibicc.h"


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


