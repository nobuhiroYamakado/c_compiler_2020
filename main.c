#include "chibicc.h"
int main(int argc, char **argv)
{
	if (argc != 2)
	{
		error("%s: invalid number of argments", argv[0]);
		return (1);
	}

	Token *tok = tokenize(argv[1]);
	Node *node = parse(tok);
	codegen(node);
	return (0);
}
