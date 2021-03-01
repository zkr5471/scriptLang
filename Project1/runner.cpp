#include "types.h"
#include "tokenize.h"
#include "runner.h"

namespace Xscript
{
	namespace Runner
	{


		Value run_expr(Node *node)
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

						}
					}

					return lhs;
				}
			}

			return { };
		}
	}
}