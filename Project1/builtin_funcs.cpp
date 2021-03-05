#include <iostream>
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

		if( name == "range" )
		{
			if( node->list.size() != 1 )
				Error(node->tok->pos, "illegal call function 'range'");

			Value r = run_expr(node->list[0]);
			if( r.type != Value::Type::Int )
				Error(node->tok->pos, "function 'range' is required integer on param[0]");

			ret.type = Value::Type::Array;
			for( int i = 0; i < r.v_Int; i++ )
			{
				Value x;
				x.v_Int = i;
				ret.list.push_back(x);
			}

			return ret;
		}

		Error(node->tok->pos, "unknown function '" + name + "'");
	}

}