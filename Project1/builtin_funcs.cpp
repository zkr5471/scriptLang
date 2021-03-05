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
			ret.type = Value::Type::Array;

			switch( node->list.size() )
			{
				case 1:
				{
					Value r = run_expr(node->list[0]);
					if( r.type != Value::Type::Int )
						Error(node->tok->pos, "function 'range' is required integer");

					for( int i = 0; i < r.v_Int; i++ )
					{
						Value x;
						x.v_Int = i;
						ret.list.push_back(x);
					}

					break;
				}

				case 2:
				{
					Value begin = run_expr(node->list[0]);
					Value end = run_expr(node->list[1]);

					if( begin.type != Value::Type::Int || end.type != Value::Type::Int )
						Error(node->tok->pos, "function 'range' is required integer");

					for( int i = begin.v_Int; i < end.v_Int; i++ )
					{
						Value x;
						x.v_Int = i;
						ret.list.push_back(x);
					}

					break;
				}

				default:
					Error(node->tok->pos, "illegal call function 'range'");
			}

			return ret;
		}

		Error(node->tok->pos, "unknown function '" + name + "'");
	}

}