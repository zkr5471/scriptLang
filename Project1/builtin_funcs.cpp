#include <iostream>
#include "types.h"
#include "tokenize.h"
#include "runner.h"

namespace Xscript
{
	Value run_builtin_func(Node *node)
	{
		string name = node->tok->str;
		Value ret;

		if( name == "print" )
		{
			for( auto &&i : node->list )
				std::cout << run_expr(i);

			std::cout << '\n';
			return ret;
		}

		Error(node->tok->pos, "unknown function '" + name + "'");
	}

}