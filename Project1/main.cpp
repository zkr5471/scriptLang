#include <iostream>

#include "tokenize.h"
#include "parser.h"
#include "runner.h"
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
		"10403 + 4539 + 982828";

	try
	{
		Token *tok = tokenize(std::move(str));

		Node *nd = Parser::parse(tok);

		Value val = Runner::run_expr(nd);

		std::cout << val.v_Int << '\n';
	}
	catch( ... )
	{

	}

	FreeMemory();
}