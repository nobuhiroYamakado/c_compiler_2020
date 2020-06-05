#include "chibicc.h"

static Node *expr(Token **rest, Token *tok);
static Node *equality(Token **rest, Token *tok);
static Node *relational(Token **rest, Token *tok);
static Node *add(Token **rest, Token *tok);
static Node *mul(Token **rest, Token *tok);
static Node *unary(Token **rest, Token *tok);
static Node *primary(Token **rest, Token *tok);

static Node *new_node(NodeKind kind)
{
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	return (node);
}

static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs)
{
	Node *node = new_node(kind);
	node->lhs = lhs;
	node->rhs = rhs;
	return (node);
}

static Node *new_unary(NodeKind kind, Node *expr)
{
	Node *node = new_node(kind);
	node->lhs = expr;
	return (node);
}

static Node *new_num(long val)
{
	Node *node = new_node(ND_NUM);
	node->val = val;
	return (node);
}

static long get_number(Token *tok)
{
	if(tok->kind != TK_NUM)
		error_tok(tok, "expected a number");
	return (tok->val);
}

// stmt = expr ";"
static Node *stmt(Token **rest, Token *tok)
{
	Node *node = new_unary(ND_EXPR_STMT, expr(&tok, tok));
	*rest = skip(tok, ";");
	return (node);
}

//expr = equality
static Node *expr(Token **rest, Token *tok)
{
	return (equality(rest, tok));
}

static Node *equality(Token **rest, Token *tok)
{
	Node *node = relational(&tok, tok);

	for (;;)
	{
		if (equal(tok, "=="))
		{
			Node *rhs = relational(&tok, tok->next);
			node = new_binary(ND_EQ, node, rhs);
		}
		else if (equal(tok,"!="))
		{
			Node *rhs = relational(&tok, tok->next);
			node = new_binary(ND_NE, node, rhs);
		}
		else
		{
			*rest = tok;
			return (node);
		}
	}
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
static Node *relational(Token **rest, Token *tok)
{
	Node *node = add(&tok, tok);
	for (;;)
	{
		if (equal(tok, "<"))
		{
			Node *rhs = add(&tok, tok->next);
			node = new_binary(ND_LT, node, rhs);
		}
		else if (equal(tok, "<="))
		{
			Node *rhs = add(&tok, tok->next);
			node = new_binary(ND_LE, node, rhs);
		}
		else if (equal(tok, ">"))
		{
			Node *rhs = add(&tok, tok->next);
			node = new_binary(ND_LT, rhs, node);
		}
		else if (equal(tok, ">="))
		{
			Node *rhs = add(&tok, tok->next);
			node = new_binary(ND_LE, rhs, node);
		}
		else
		{
			*rest = tok;
			return (node);
		}
	}
}

// add = mul ("+" mul | "-" mul)*
static Node *add(Token **rest, Token *tok)
{
	Node *node = mul(&tok, tok);

	for (;;)
	{
		if (equal(tok, "+"))
		{
			Node *rhs = mul(&tok, tok->next);
			node = new_binary(ND_ADD, node, rhs);
		}
		else if (equal(tok, "-"))
		{
			Node *rhs = mul(&tok, tok->next);
			node = new_binary(ND_SUB, node, rhs);
		}
		else
		{
			*rest = tok;
			return (node);
		}
	}
}

// mul = unary("*" unary | "/" unary)*
Node *mul(Token **rest, Token *tok)
{
	Node *node = unary(&tok, tok);
	for (;;)
	{
		if (equal(tok, "*"))
		{
			Node *rhs = unary(&tok, tok->next);
			node = new_binary(ND_MUL, node, rhs);
		}
		else if (equal(tok, "/"))
		{
			Node *rhs = unary(&tok, tok->next);
			node = new_binary(ND_DIV, node, rhs);
		}
		else
		{
			*rest = tok;
			return (node);
		}
	}
}

// unary = ("+" | "-") unary
static Node *unary(Token **rest, Token *tok)
{
	if (equal(tok, "+"))
	{
		return (unary(rest, tok->next));
	}
	
	if (equal(tok, "-"))
	{
		return (new_binary(ND_SUB, new_num(0), unary(rest, tok->next)));
	}

	return (primary(rest, tok));
}

//primary = "(" expr ")" | num
static Node *primary(Token **rest, Token *tok)
{

	//次のトークンが'('なら、"(" expr ")"のはず
	//if (consume("("))
	if (equal(tok, "("))
	{
		Node *node = expr(&tok, tok->next);
		//expect(")");
		*rest = skip(tok, ")");
		return (node);
	}
	
	//そうでないときは数字が渡されるはず
	Node *node = new_num(get_number(tok));
	*rest = tok->next;
	return (node);
	//return new_node_num(expect_number());
}

// program = stmt*
Node *parse(Token *tok)
{
	Node head = {};
	Node *cur = &head;
	while (tok->kind != TK_EOF)
		cur = cur->next = stmt(&tok, tok);
	return (head.next);
}
