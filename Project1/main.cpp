#include <iostream>

#include "tokenize.h"
#include "parser.h"
#include "allocator.h"

using namespace Xscript;

void print_token(Token *tok)
{
	while( tok->type != Token::Type::End )
	{
		std::cout << tok->str << ' ';
		tok = tok->next;
	}
}

int main()
{
	string str =
		"1 + 2 * 3";

	try
	{
		Token *tok = tokenize(std::move(str));

		Node *nd = Parser::parse(tok);

	}
	catch( ... )
	{

	}

	FreeMemory();
}