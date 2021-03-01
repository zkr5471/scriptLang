#include <iostream>

#include "tokenize.h"
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
		"1234 12 Hello 'A' 3.14  02";

	try
	{
		Token *tok = tokenize(std::move(str));

		

	}
	catch( ... )
	{

	}

	FreeMemory();
}