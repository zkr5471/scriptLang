#include <iostream>
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

	Value run_stmt(Node *node)
	{
		if( node == nullptr )
			return { };

		switch( node->type )
		{
			case Node::Type::Block:
			{
				for( auto &&i : node->list )
				{
					run_stmt(i);
				}
				break;
			}

			default:
				return run_expr(node);
		}

		return { };
	}

	Value run_expr(Node *node)
	{
		if( node == nullptr )
			return { };

		switch( node->type )
		{
			case Node::Type::Value:
				return node->tok->value;

			case Node::Type::Variable:
			{
				Value var = variables[node->varIndex];
				var.var_ptr = &variables[node->varIndex];
				return var;
			}

			case Node::Type::Array:
			{
				Value val;
				val.type = Value::Type::Array;

				for( auto &&i : node->list )
					val.list.push_back(run_expr(i));

				return val;
			}

			case Node::Type::IndexRef:
			{
				Value arr = run_expr(node->lhs);
				Value index = run_expr(node->rhs);

				if( arr.type != Value::Type::Array )
					Error(node->tok->pos, "type mismatch");

				if( index.type != Value::Type::Int )
					Error(node->tok->pos, "type mismatch");

				if( index.v_Int < 0 || index.v_Int >= arr.list.size() )
					Error(node->tok->pos, "subscript out of range");

				Value ret = arr.list[index.v_Int];
				if( arr.var_ptr != nullptr )
				{
					ret.var_ptr = & arr.var_ptr->list[index.v_Int];
				}

				return ret;
			}

			case Node::Type::Not:
			{
				auto val = run_expr(node->lhs);
				val.v_Int = !val.eval();
				val.type = Value::Type::Int;
				return val;
			}
			
			case Node::Type::BitNOT:
			{
				auto val = run_expr(node->lhs);
				if( val.type != Value::Type::Int )
					Error(node->tok->pos, "type mismatch");
				val.v_Int = ~val.v_Int;
				return val;
			}

			case Node::Type::Assign:
			{
				Value lhs = run_expr(node->lhs);
				Value rhs = run_expr(node->rhs);

				if( lhs.var_ptr == nullptr )
					Error(node->tok->pos, "cannot assignment to rvalue");

				return *lhs.var_ptr = rhs;
			}

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

					case Node::Type::Mod:
					{
						if( lhs.type == Value::Type::Array || rhs.type == Value::Type::Array )
							Error(node->tok->pos, "type mismatch");

						if( lhs.type == Value::Type::Float || rhs.type == Value::Type::Float )
							Error(node->tok->pos, "type mismatch");

						switch( lhs.type )
						{
							case Value::Type::Int:
								lhs.v_Int %= rhs.v_Int;
								break;
							case Value::Type::Char:
								lhs.v_Char %= rhs.v_Char;
								break;
						}

						break;
					}

					case Node::Type::ShiftL:
					{
						if( lhs.type != Value::Type::Int || rhs.type != Value::Type::Int )
							Error(node->tok->pos, "type mismatch");

						lhs.v_Int <<= rhs.v_Int;
						break;
					}
					
					case Node::Type::ShiftR:
					{
						if( lhs.type != Value::Type::Int || rhs.type != Value::Type::Int )
							Error(node->tok->pos, "type mismatch");

						lhs.v_Int >>= rhs.v_Int;
						break;
					}

					case Node::Type::Bigger:
					{
						if( lhs.type == Value::Type::Array || rhs.type == Value::Type::Array )
							Error(node->tok->pos, "type mismatch");

						switch( lhs.type )
						{
							case Value::Type::Int:
								lhs.v_Int = lhs.v_Int > rhs.v_Int;
								break;
							case Value::Type::Char:
								lhs.v_Int = lhs.v_Char > rhs.v_Char;
								break;
							case Value::Type::Float:
								lhs.v_Int = lhs.v_Float > rhs.v_Float;
								break;
						}

						lhs.type = Value::Type::Int;
						break;
					}
					
					case Node::Type::BiggerOrEqual:
					{
						if( lhs.type == Value::Type::Array || rhs.type == Value::Type::Array )
							Error(node->tok->pos, "type mismatch");

						switch( lhs.type )
						{
							case Value::Type::Int:
								lhs.v_Int = lhs.v_Int >= rhs.v_Int;
								break;
							case Value::Type::Char:
								lhs.v_Int = lhs.v_Char >= rhs.v_Char;
								break;
							case Value::Type::Float:
								lhs.v_Int = lhs.v_Float >= rhs.v_Float;
								break;
						}

						lhs.type = Value::Type::Int;
						break;
					}
					
					case Node::Type::Equal:
					{
						if( lhs.type == Value::Type::Array || rhs.type == Value::Type::Array )
							Error(node->tok->pos, "type mismatch");

						switch( lhs.type )
						{
							case Value::Type::Int:
								lhs.v_Int = lhs.v_Int == rhs.v_Int;
								break;
							case Value::Type::Char:
								lhs.v_Int = lhs.v_Char == rhs.v_Char;
								break;
							case Value::Type::Float:
								lhs.v_Int = lhs.v_Float == rhs.v_Float;
								break;
						}

						lhs.type = Value::Type::Int;
						break;
					}
					
					case Node::Type::NotEqual:
					{
						if( lhs.type == Value::Type::Array || rhs.type == Value::Type::Array )
							Error(node->tok->pos, "type mismatch");

						switch( lhs.type )
						{
							case Value::Type::Int:
								lhs.v_Int = lhs.v_Int != rhs.v_Int;
								break;
							case Value::Type::Char:
								lhs.v_Int = lhs.v_Char != rhs.v_Char;
								break;
							case Value::Type::Float:
								lhs.v_Int = lhs.v_Float != rhs.v_Float;
								break;
						}

						lhs.type = Value::Type::Int;
						break;
					}

					case Node::Type::BitAND:
					{
						if( lhs.type != Value::Type::Int || rhs.type != Value::Type::Int )
							Error(node->tok->pos, "type mismatch");

						lhs.v_Int &= rhs.v_Int;
						break;
					}
					
					case Node::Type::BitXOR:
					{
						if( lhs.type != Value::Type::Int || rhs.type != Value::Type::Int )
							Error(node->tok->pos, "type mismatch");

						lhs.v_Int ^= rhs.v_Int;
						break;
					}
					
					case Node::Type::BitOR:
					{
						if( lhs.type != Value::Type::Int || rhs.type != Value::Type::Int )
							Error(node->tok->pos, "type mismatch");

						lhs.v_Int |= rhs.v_Int;
						break;
					}

					case Node::Type::And:
					{
						if( lhs.type == Value::Type::Array || rhs.type == Value::Type::Array )
							Error(node->tok->pos, "type mismatch");

						lhs.v_Int = lhs.eval() && rhs.eval();
						lhs.type = Value::Type::Int;
						break;
					}
					
					case Node::Type::Or:
					{
						if( lhs.type == Value::Type::Array || rhs.type == Value::Type::Array )
							Error(node->tok->pos, "type mismatch");

						lhs.v_Int = lhs.eval() || rhs.eval();
						lhs.type = Value::Type::Int;
						break;
					}




				}

				return lhs;
			}
		}

		return { };
	}
}