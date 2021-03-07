#include <iostream>
#include <fstream>

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

string openfile(const char *path)
{
	std::ifstream ifs(path);
	string line, ret;

	if( ifs.fail() )
	{
		std::cout << "cannot open file.";
		throw 0;
	}

	while( std::getline(ifs, line) )
	{
		ret += line + '\n';
	}

	if( ret.empty() )
	{
		std::cout << "empty source file.";
		throw 0;
	}

	return std::move(ret);
}

namespace Xscript {
	extern std::vector<Value> variables;
}

#define  TEST_BUILD  1

int main(int argc, char **argv)
{
	#if TEST_BUILD == 0
	if( argc == 1 )
	{
		std::cout << "no input files.\n";
		return -1;
	}
	#endif

	try
	{
		string src = std::move(openfile(
			#if TEST_BUILD
			"C:/Users/mrzkr/Desktop/test.txt"
			#else
			argv[1]
			#endif
		));

		Token *tok = tokenize(std::move(src));

		Node *nd = Parser::parse(tok);

		Value val = run_stmt(nd);

	//	for( auto &&i : variables )
	//		std::cout << i.name << ": " << i << '\n';
	}
	catch( ... )
	{

	}

	FreeMemory();
}