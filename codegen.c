#include "chibicc.h"

static int top = 0;

static char *reg(int idx)
{
	static char *r[] = {"r10", "r11", "r12", "r13", "r14", "r15"};
	if (idx < 0 || sizeof(r) / sizeof(*r) <= idx)
		error("register out of range: $d", idx);
	return (r[idx]);
}
static void gen(Node *node)
{
	if(node->kind == ND_NUM)
	{
		printf("  mov %s, %d\n", reg(top++), node->val);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

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

void codegen(Node *node)
{
	//アセンブリの前半部分
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	
	// Save callee-saved registers.
	printf("  push r12\n");
	printf("  push r13\n");
	printf("  push r14\n");
	printf("  push r15\n");
	
	//抽象構文木を下りながらコード生成
	gen(node);

	// raxにスタックトップをプッシュして返値になっているはず。
	printf("  mov rax, %s\n",reg(top - 1));
	// back callee-saved registers.
	printf("  pop r15\n");
	printf("  pop r14\n");
	printf("  pop r13\n");
	printf("  pop r12\n");
	
	printf("  ret\n");


}


