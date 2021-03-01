#include "types.h"
#include "tokenize.h"
#include "runner.h"

namespace Xscript
{
	Value Runner::run_expr(Node *node)
	{
		if( node == nullptr )
			return { };

		switch( node->type )
		{
			case Node::Type::Value:
				return node->tok->value;

			default:
			{
				Value lhs = run_expr(node->lhs);
				Value rhs = run_expr(node->rhs);

				switch( node->type )
				{
					case Node::Type::Add:
					{
						if( lhs.type == Value::Type::Array )
						{
							if( rhs.type == Value::Type::Array )
								lhs.list += rhs.list;
							else
								lhs.list.push_back(rhs);

							return lhs;
						}
						else if( rhs.type == Value::Type::Array )
							Error(node->tok->pos, "type mismatch");

						switch( lhs.type )
						{
							case Value::Type::Int:
								lhs.v_Int += rhs.v_Int;
								break;
						}

						break;
					}
				}

				return lhs;
			}
		}

		return { };
	}
}