#include <iostream>
#include "types.h"
#include "tokenize.h"
#include "runner.h"
#include "error.h"

namespace Xscript
{
	extern std::vector<Value> variables;
	extern std::vector<Node *> functions;

	namespace
	{
		std::vector<Value> func_params;

		bool *LoopBreaked = nullptr;
		bool *LoopContinued = nullptr;
		bool *func_returned = nullptr;

		Node *cur_func_node;
		Value *func_ret_val;
		
		size_t call_count;
	}

	int64_t find_func(string name)
	{
		for( size_t i = 0; i < functions.size(); i++ )
			if( functions[i]->tok->str == name )
				return i;

		return -1;
	}

	void _adjust_ValueType(Value &_L, Value &_R)
	{
		if( _L.type == Value::Type::Array || _R.type == Value::Type::Array )
			return;

		if( _L.type == Value::Type::Float || _R.type == Value::Type::Float )
		{
			for( auto &&i : { &_L,&_R } )
				if( i->type == Value::Type::Int ) i->v_Float = i->v_Int;
				else if( i->type == Value::Type::Char ) i->v_Float = i->v_Char;
			
			_L.type = _R.type = Value::Type::Float;
			return;
		}

		if( _L.type != _R.type ) {
			for( auto &&i : { &_L,&_R } )
				if( i->type == Value::Type::Char ) i->v_Int = i->v_Char;
			
			_L.type = _R.type = Value::Type::Int;
		}
	}

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

		if( name == "to_int" )
		{
			if( node->list.size() != 1 )
				Error(node->tok->pos, ERROR_ILLEGAL_PARAM);

			auto &&v = run_expr(node->list[0]);

			if( v.type == Value::Type::Char )
			{
				if( isdigit(v.v_Char) == 0 )
					return ret;

				ret.v_Int = v.v_Char - '0';
				return ret;
			}

			if( v.is_string() == 0 )
				Error(node->tok->pos, "'to_int' には文字列を渡してください。");

			string s;
			for( auto &&i : v.list )
			{
				if( isdigit(i.v_Char) == 0 )
					return ret;
				//	Error(node->tok->pos, "数値に変換できません。");

				s += i.v_Char;
			}

			ret.v_Int = std::stoi(s);
			return ret;
		}

		//if( name == "get_args" )
		//{
		//	if( cur_func_node == nullptr )
		//		Error(node->tok->pos, "'get_args'を使用できるのは関数の中のみです。");

		//	ret.type = Value::Type::Array;
		//	for( auto &&i : cur_func_node->list )
		//	{
		//		ret.list.push_back(i->tok->value);
		//	}

		//	return ret;
		//}

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
					
					if( func_returned  && *func_returned )
						break;

					if( LoopBreaked && *LoopBreaked )
						break;
				}
				break;
			}

			case Node::Type::Break:
			{
				if( LoopBreaked == nullptr )
					Error(node->tok->pos, "cannot use 'break' here");

				*LoopBreaked = 1;
				break;
			}

			case Node::Type::Continue:
			{
				if( LoopContinued == nullptr )
					Error(node->tok->pos, "cannot use 'continue' here");

				*LoopBreaked = *LoopContinued = 1;
				break;
			}

			case Node::Type::If:
			{
				if( run_expr(node->lhs).eval() )
					run_stmt(node->rhs);
				else if( node->list.size() )
					run_stmt(node->list[0]);

				break;
			}

			case Node::Type::For:
			{
				bool breaked = 0, continued = 0;;
				bool *oldptr = LoopBreaked;
				bool *oldptrc = LoopContinued;
				LoopBreaked = &breaked;
				LoopContinued = &continued;

				run_expr(node->list[0]);
				while( 1 )
				{
					if( node->list[1] && run_expr(node->list[1]).eval() == false )
						break;

					continued = breaked = 0;
					run_stmt(node->lhs);

					if( func_returned && *func_returned )
						break;

					if( !continued && breaked )
						break;

					run_expr(node->list[2]);
				}

				LoopBreaked = oldptr;
				LoopContinued = oldptrc;
				break;
			}

			case Node::Type::Switch:
			{
				bool breaked = 0;
				bool *oldptr = LoopBreaked;
				LoopBreaked = &breaked;

				Value cond = run_expr(node->lhs);

				size_t index = 0;

				for( index = 0; index < node->list.size(); index++ )
				{
					if( run_expr(node->list[index]->lhs).eval() )
						break;
				}
				
				for( ; index < node->list.size(); index++ )
				{
					run_stmt(node->list[index]->rhs);

					if( func_returned || breaked )
						break;
				}

				LoopBreaked = oldptr;
				break;
			}

			case Node::Type::Return:
			{
				if( func_ret_val == nullptr )
					Error(node->tok->pos, "cannot use 'return' here");

				*func_ret_val = run_expr(node->lhs);
				*func_returned = true;
				break;
			}

			case Node::Type::Function:
				break;

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

			case Node::Type::Param:
			{
		//		printf("func_params.size() = %d, varIndex = %d\n", func_params.size(), node->varIndex);
		//		return { };
				
				auto &x = func_params[func_params.size() - 1 - node->varIndex];
				x.var_ptr = &x;
				return x;
			}

			case Node::Type::Callfunc:
			{
				// find user-defined function
				auto find = find_func(node->tok->str);
				Value func_ret;

				if( OPTIONS::ignore_stack == 0 )
				{
					call_count++;

					if( call_count >= 0x100 )
					{
						Error(node->tok->pos, "関数の呼び出しが深すぎます。" ERR_SEE_OPTION);
					}
				}

				// if found
				if( find != -1 )
				{
					auto func_nd = functions[find];
					auto &params = node->list;

					if( params.size() != func_nd->list.size() )
						Error(node->tok->pos, "'" + node->tok->str + "' の呼び出しに渡される引数が正しくありません");

					// save pointers
					auto func_ret_ptr = func_ret_val;
					auto func_ret_flag = func_returned;

					func_ret_val = &func_ret;

					bool returned = 0;
					func_returned = &returned;

					for( int64_t i = params.size() - 1; i >= 0; i-- )
						func_params.push_back(run_expr(node->list[i]));

					run_stmt(func_nd->lhs);

					for( auto &&i : params )
						func_params.pop_back();

					// restore pointers
					func_ret_val = func_ret_ptr;
					func_returned = func_ret_flag;

				//	return func_ret;
				}
				else
				{
					// not found the user-defined function, try to execute built-in function
					func_ret = run_builtin_func(node);
				}

				if( OPTIONS::ignore_stack == 0 )
				{
					call_count--;
				}

				return func_ret;
			}


			case Node::Type::MemberAccess:
			{
				Value obj = run_expr(node->lhs);

				if( node->rhs->type != Node::Type::Callfunc && node->rhs->type != Node::Type::Variable )
					Error(node->tok->pos, "syntax error");

				string name = node->rhs->tok->str;
				bool is_func = node->rhs->type == Node::Type::Callfunc;

				if( name == "length" && !is_func )
				{
					if( obj.type != Value::Type::Array )
						Error(node->tok->pos, "not have the 'length' property");

					Value ret;
					ret.v_Int = obj.list.size();

					return ret;
				}

				Error(node->tok->pos, "invalid member access");
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
								lhs.v_Int = lhs.v_Int < rhs.v_Int;
								break;
							case Value::Type::Char:
								lhs.v_Int = lhs.v_Char < rhs.v_Char;
								break;
							case Value::Type::Float:
								lhs.v_Int = lhs.v_Float < rhs.v_Float;
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
								lhs.v_Int = lhs.v_Int <= rhs.v_Int;
								break;
							case Value::Type::Char:
								lhs.v_Int = lhs.v_Char <= rhs.v_Char;
								break;
							case Value::Type::Float:
								lhs.v_Int = lhs.v_Float <= rhs.v_Float;
								break;
						}

						lhs.type = Value::Type::Int;
						break;
					}
					
					case Node::Type::Equal:
					{
						if( (lhs.type == Value::Type::Array) != (rhs.type == Value::Type::Array) )
							Error(node->tok->pos, "type mismatch");

						lhs.v_Int = lhs.equals(rhs);
						lhs.type = Value::Type::Int;
						break;
					}
					
					case Node::Type::NotEqual:
					{
						if( (lhs.type == Value::Type::Array) != (rhs.type == Value::Type::Array) )
							Error(node->tok->pos, "type mismatch");

						lhs.v_Int = lhs.equals(rhs) == 0;
						lhs.type = Value::Type::Int;
						break;
					}

					case Node::Type::BitAND:
					{
						if( lhs.type != Value::Type::Int || rhs.type != Value::Type::Int )
							Error(node->tok->pos, "type mismatch");

						lhs.v_Int &= rhs.v_Int;
						lhs.type = Value::Type::Int;
						break;
					}
					
					case Node::Type::BitXOR:
					{
						if( lhs.type != Value::Type::Int || rhs.type != Value::Type::Int )
							Error(node->tok->pos, "type mismatch");

						lhs.v_Int ^= rhs.v_Int;
						lhs.type = Value::Type::Int;
						break;
					}
					
					case Node::Type::BitOR:
					{
						if( lhs.type != Value::Type::Int || rhs.type != Value::Type::Int )
							Error(node->tok->pos, "type mismatch");

						lhs.v_Int |= rhs.v_Int;
						lhs.type = Value::Type::Int;
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