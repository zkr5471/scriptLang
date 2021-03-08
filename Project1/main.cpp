#include <iostream>
#include <fstream>

#include "tokenize.h"
#include "parser.h"
#include "runner.h"
#include "allocator.h"
#include "error.h"

using namespace Xscript;

void print_token(Token *tok)
{
	while( tok->type != Token::Type::End )
	{
		std::cout << tok->str << ' ';
		tok = tok->next;
	}
}

string openfile(string path)
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

bool OPTIONS::ignore_stack;

string operator "" _s(const char *s, size_t len)
{
	return string(s, len);
}

int main(int argc, char **argv)
{
	string input_file;

	try
	{
		for( int i = 1; i < argc; )
		{
			if( argv[i] == "-ignore"_s )
			{
				i++;
				if( i >= argc )
				{
					std::cout << "invalid option";
					throw 0;
				}

				if( argv[i] == "stack"_s )
				{
					OPTIONS::ignore_stack = 1;
					i++;
				}
			}

			else if( input_file.empty() )
			{
				input_file = argv[i];
				i++;
			}
		}

	#if TEST_BUILD == 0
		if( input_file.empty() )
		{
			std::cout << "no input files.\n";
			throw 0;
		}
	#endif

		string src = std::move(openfile(
		#if TEST_BUILD
			"C:/Users/mrzkr/Desktop/test.txt"
		#else
			input_file
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