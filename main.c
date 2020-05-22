#include "chibicc.h"
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
