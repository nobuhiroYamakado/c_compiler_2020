#include "chibicc.h"
int main(int argc, char **argv)
{
	if (argc != 2)
	{
		error("%s: invalid number of argments", argv[0]);
		return (1);
	}

	//user_input = argv[1];
	//token = tokenize(user_input);
	Token *tok = tokenize(argv[1]);
	//Node *node = expr();
	Node *node = parse(tok);
	
	codegen(node);
	return (0);
}
