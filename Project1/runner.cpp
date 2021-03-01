#include "types.h"
#include "tokenize.h"
#include "runner.h"

namespace Xscript
{
	extern std::vector<Value> variables;

	void _adjust_ValueType(Value &_L, Value &_R)
	{
		if( _L.type == Value::Type::Array || _R.type == Value::Type::Array )
			return;

		if( _L.type == Value::Type::Float || _R.type == Value::Type::Float )
		{
			for( auto &&i : { &_L,&_R } )
			{
				if( i->type == Value::Type::Int ) i->v_Float = i->v_Int;
				else if( i->type == Value::Type::Char ) i->v_Float = i->v_Char;
			}
			_L.type = _R.type = Value::Type::Float;
			return;
		}

		if( _L.type != _R.type ) {
			for( auto &&i : { &_L,&_R } ) {
				if( i->type == Value::Type::Char ) i->v_Int = i->v_Char;
			}
			_L.type = _R.type = Value::Type::Int;
		}
	}

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

				_adjust_ValueType(lhs, rhs);

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
							case Value::Type::Char:
								lhs.v_Char += rhs.v_Char;
								break;
							case Value::Type::Float:
								lhs.v_Float += rhs.v_Float;
								break;
						}

						break;
					}

					case Node::Type::Sub:
					{
						switch( lhs.type )
						{
							case Value::Type::Int:
								lhs.v_Int -= rhs.v_Int;
								break;
							case Value::Type::Char:
								lhs.v_Char -= rhs.v_Char;
								break;
							case Value::Type::Float:
								lhs.v_Float -= rhs.v_Float;
								break;
							case Value::Type::Array:
								Error(node->tok->pos, "type mismatch");
						}

						break;
					}
					
					case Node::Type::Mul:
					{
						switch( lhs.type )
						{
							case Value::Type::Int:
								lhs.v_Int *= rhs.v_Int;
								break;
							case Value::Type::Char:
								lhs.v_Char *= rhs.v_Char;
								break;
							case Value::Type::Float:
								lhs.v_Float *= rhs.v_Float;
								break;
							case Value::Type::Array:
							{
								if( rhs.type == Value::Type::Int )
								{
									lhs.list *= rhs.v_Int;
									break;
								}

								Error(node->tok->pos, "type mismatch");
							}
						}

						break;
					}

					case Node::Type::Div:
					{
						if( lhs.type == Value::Type::Array || rhs.type == Value::Type::Array )
							Error(node->tok->pos, "type mismatch");

						switch( lhs.type )
						{
							case Value::Type::Int:
								lhs.v_Int /= rhs.v_Int;
								break;
							case Value::Type::Char:
								lhs.v_Char /= rhs.v_Char;
								break;
							case Value::Type::Float:
								lhs.v_Float /= rhs.v_Float;
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