#include "chibicc.h"

static int top = 0;

static char *reg(int idx)
{
	static char *r[] = {"r10", "r11", "r12", "r13", "r14", "r15"};
	if (idx < 0 || sizeof(r) / sizeof(*r) <= idx)
		error("register out of range: $d", idx);
	return (r[idx]);
}

// pushes the given node's address to the stack
static void gen_addr(Node *node)
{
	if (node->kind == ND_VAR)
	{
		int offset = (node->name - 'a' + 1) * 8;
		offset += 32; //for callee-saved registers
		printf("  lea %s, [rbp-%d]\n", reg(top++), offset);
		return;
	}
	error("not an lvalue/codegen.c");
}

static void load(void)
{
	printf("  mov %s, [%s]\n", reg(top - 1), reg(top - 1));
}

static void store(void)
{
	printf("  mov [%s], %s\n", reg(top - 1), reg(top - 2));
	top--;
}

//Generate code fot a given node.
static void gen_expr(Node *node)
{
	switch (node->kind)
	{
		case ND_NUM:
			printf("  mov %s, %d\n", reg(top++), node->val);
			return;
		case ND_VAR:
			gen_addr(node);
			load();
			return;
		case ND_ASSIGN:
			gen_expr(node->rhs);
			gen_addr(node->lhs);
			store();
			return;
	}

	gen_expr(node->lhs);
	gen_expr(node->rhs);

	char *rd = reg(top -2);
	char *rs = reg(top -1);
	top--;

	switch (node->kind)
	{
		case ND_ADD:
			printf("  add %s, %s\n", rd, rs);
			break;
		case ND_SUB:
			printf("  sub %s, %s\n", rd, rs);
			break;
		case ND_MUL:
			printf("  imul %s, %s\n", rd, rs);
			break;
		case ND_DIV:
			printf("  mov rax, %s\n", rd);
			printf("  cqo\n");
			printf("  idiv %s\n", rs);
			printf("  mov %s, rax\n", rd);
			break;
		case ND_EQ:
			printf("  cmp %s, %s\n", rd, rs);
			printf("  sete al\n");
			printf("  movzb %s, al\n", rd);
			break;
		case ND_NE:
			printf("  cmp %s, %s\n", rd, rs);
			printf("  setne al\n");
			printf("  movzb %s, al\n", rd);
			break;
		case ND_LT:
			printf("  cmp %s, %s\n", rd, rs);
			printf("  setl al\n");
			printf("  movzb %s, al\n", rd);
			break;
		case ND_LE:
			printf("  cmp %s, %s\n", rd, rs);
			printf("  setle al\n");
			printf("  movzb %s, al\n", rd);
			break;
		default:
			error("invalid expression");
	}
}

static void gen_stmt(Node *node)
{
	switch (node->kind)
	{
		case ND_RETURN:
			gen_expr(node->lhs);
			printf("  mov rax, %s\n", reg(--top));
			printf("  jmp .L.return\n");
			return;
		case ND_EXPR_STMT:
			gen_expr(node->lhs);
			top--;
			return;
		default:
			error("invalid statement");
	}
}

void codegen(Node *node)
{
	//アセンブリの前半部分
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	
	// Prologue, r12-15 are callee-saved registers.
	
	printf("  push rbp\n");
	printf("  mov rbp, rsp\n");
	printf("  sub rsp, 240\n");
	printf("  mov [rbp-8], r12\n");
	printf("  mov [rbp-16], r13\n");
	printf("  mov [rbp-24], r14\n");
	printf("  mov [rbp-32], r15\n");

	for (Node *n = node; n; n = n->next)
	{
		gen_stmt(n);
		assert(top == 0);
	}

	// from ND_RETURN
	printf(".L.return:\n");
	// back callee-saved registers.
	printf("  mov r12, [rbp-8]\n");
	printf("  mov r13, [rbp-16]\n");
	printf("  mov r14, [rbp-24]\n");
	printf("  mov r15, [rbp-32]\n");
	printf("  mov rsp, rbp\n");
	printf("  pop rbp\n");
	printf("  ret\n");


}


