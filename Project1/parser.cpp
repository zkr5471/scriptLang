#include <iostream>
#include "types.h"
#include "tokenize.h"
#include "allocator.h"
#include "parser.h"

namespace Xscript
{
	std::vector<Value> variables;

	namespace Parser
	{
		static Token *g_tok, *csm_tok;

		bool check()
		{
			return g_tok->type != Token::Type::End;
		}

		void next()
		{
			g_tok = g_tok->next;
		}

		bool consume(string str)
		{
			if( g_tok->str == str )
			{
				csm_tok = g_tok;
				next();
				return true;
			}

			return false;
		}

		void expect(string str)
		{
			if( consume(str) == false )
			{
				Error(g_tok->pos, "expect '" + str + "'");
			}
		}

		int64_t find_variable(string name)
		{
			for( size_t i = 0; i < variables.size(); i++ )
			{
				if( variables[i].name == name )
					return i;
			}

			return -1;
		}

		Node *NewNode_Int(int val)
		{
			Node *nd = NewNode();
			nd->tok = NewToken();
			nd->tok->value.v_Int = val;
			return nd;
		}

		Node *MakeUniqueVariable()
		{
			string name;

			do {
				name = Utils::GetRandomStr();
			} while( find_variable(name) != -1 );

			Value var;
			var.name = name;
			
			Node *nd = NewNode(Node::Type::Variable);
			nd->varIndex = variables.size();

			variables.push_back(var);

			return nd;
		}

		Node *primary()
		{
			if( consume("(") )
			{
				Node *x = expr();
				expect(")");
				return x;
			}

			if( consume("[") )
			{
				Node *x = NewNode(Node::Type::Array);
				x->tok = csm_tok;

				if( !consume("]") )
				{
					do {
						x->list.push_back(expr());
					} while( consume(",") );

					expect("]");
				}

				return x;
			}

			switch( g_tok->type )
			{
				case Token::Type::Int:
				case Token::Type::Float:
				case Token::Type::Char:
				case Token::Type::String:
				{
					Node *nd = NewNode();
					nd->tok = g_tok;

					next();
					return nd;
				}

				case Token::Type::Ident:
				{
					Node *nd = NewNode(Node::Type::Variable);
					nd->tok = g_tok;

					next();

					if( consume("(") )
					{
						nd->type = Node::Type::Callfunc;

						if( !consume(")") )
						{
							while( check() )
							{
								nd->list.push_back(expr());

								if( consume(",") )
									continue;

								expect(")");
								break;
							}
						}

						return nd;
					}

					int64_t find = find_variable(nd->tok->str);

					if( find == -1 )
					{
						Value var;
						var.name = nd->tok->str;

						find = variables.size();
						variables.push_back(var);
					}

					nd->varIndex = find;
					return nd;
				}
			}

			Error(g_tok->pos, "syntax error");
		}

		Node *indexRef()
		{
			Node *x = primary();

			while( check() )
			{
				if( consume("[") )
				{
					x = NewNode(Node::Type::IndexRef, x, expr(), csm_tok);
					expect("]");
				}
				else
					break;
			}

			return x;
		}
		
		Node *member_access()
		{
			Node *x = indexRef();

			while( check() )
			{
				if( consume(".") )
					x = NewNode(Node::Type::MemberAccess, x, indexRef(), csm_tok);
				else
					break;
			}

			return x;
		}

		Node *unary()
		{
			if( consume("-") )
				return NewNode(Node::Type::Sub, NewNode_Int(0), member_access(), csm_tok);
			
			if( consume("!") )
				return NewNode(Node::Type::Not, member_access(), nullptr, csm_tok);

			if( consume("~") )
				return NewNode(Node::Type::BitNOT, member_access(), nullptr, csm_tok);

			return member_access();
		}

		Node *mul()
		{
			Node *x = unary();

			while( check() )
			{
				if( consume("*") )
					x = NewNode(Node::Type::Mul, x, unary(), csm_tok);
				else if( consume("/") )
					x = NewNode(Node::Type::Div, x, unary(), csm_tok);
				else if( consume("%") )
					x = NewNode(Node::Type::Mod, x, unary(), csm_tok);
				else
					break;
			}

			return x;
		}

		Node *add()
		{
			Node *x = mul();

			while( check() )
			{
				if( consume("+") )
					x = NewNode(Node::Type::Add, x, mul(), csm_tok);
				else if( consume("-") )
					x = NewNode(Node::Type::Sub, x, mul(), csm_tok);
				else
					break;
			}

			return x;
		}

		Node *shift()
		{
			Node *x = add();

			while( check() )
			{
				if( consume("<<") )
					x = NewNode(Node::Type::ShiftL, x, add(), csm_tok);
				else if( consume(">>") )
					x = NewNode(Node::Type::ShiftR, x, add(), csm_tok);
				else
					break;
			}

			return x;
		}

		Node *compare()
		{
			Node *x = shift();

			while( check() )
			{
				if( consume("<") )
					x = NewNode(Node::Type::Bigger, x, shift(), csm_tok);
				else if( consume(">") )
					x = NewNode(Node::Type::Bigger, shift(), x, csm_tok);
				else if( consume("<=") )
					x = NewNode(Node::Type::BiggerOrEqual, x, shift(), csm_tok);
				else if( consume(">=") )
					x = NewNode(Node::Type::BiggerOrEqual, shift(), x, csm_tok);
				else
					break;
			}

			return x;
		}

		Node *equal()
		{
			Node *x = compare();

			while( check() )
			{
				if( consume("==") )
					x = NewNode(Node::Type::Equal, x, compare(), csm_tok);
				else if( consume("!=") )
					x = NewNode(Node::Type::NotEqual, x, compare(), csm_tok);
				else
					break;
			}

			return x;
		}

		Node *BitAND()
		{
			Node *x = equal();

			while( check() )
			{
				if( consume("&") )
					x = NewNode(Node::Type::BitAND, x, equal(), csm_tok);
				else
					break;
			}

			return x;
		}

		Node *BitXOR()
		{
			Node *x = BitAND();

			while( check() )
			{
				if( consume("^") )
					x = NewNode(Node::Type::BitXOR, x, BitAND(), csm_tok);
				else
					break;
			}

			return x;
		}

		Node *BitOR()
		{
			Node *x = BitXOR();

			while( check() )
			{
				if( consume("|") )
					x = NewNode(Node::Type::BitOR, x, BitXOR(), csm_tok);
				else
					break;
			}

			return x;
		}

		Node *And()
		{
			Node *x = BitOR();

			while( check() )
			{
				if( consume("&&") )
					x = NewNode(Node::Type::And, x, BitOR(), csm_tok);
				else
					break;
			}

			return x;
		}
		
		Node *Or()
		{
			Node *x = And();

			while( check() )
			{
				if( consume("||") )
					x = NewNode(Node::Type::Or, x, And(), csm_tok);
				else
					break;
			}

			return x;
		}

		Node *assign()
		{
			Node *x = Or();

			if( consume("=") )
				x = NewNode(Node::Type::Assign, x, assign(), csm_tok);
			else if( consume("+=") )
				x = NewNode(Node::Type::Assign, x, NewNode(Node::Type::Add, x, assign()), csm_tok);
			else if( consume("-=") )
				x = NewNode(Node::Type::Assign, x, NewNode(Node::Type::Sub, x, assign()), csm_tok);
			else if( consume("*=") )
				x = NewNode(Node::Type::Assign, x, NewNode(Node::Type::Mul, x, assign()), csm_tok);
			else if( consume("/=") )
				x = NewNode(Node::Type::Assign, x, NewNode(Node::Type::Div, x, assign()), csm_tok);
			else if( consume("%=") )
				x = NewNode(Node::Type::Assign, x, NewNode(Node::Type::Mod, x, assign()), csm_tok);
			else if( consume("&=") )
				x = NewNode(Node::Type::Assign, x, NewNode(Node::Type::BitAND, x, assign()), csm_tok);
			else if( consume("^=") )
				x = NewNode(Node::Type::Assign, x, NewNode(Node::Type::BitXOR, x, assign()), csm_tok);
			else if( consume("|=") )
				x = NewNode(Node::Type::Assign, x, NewNode(Node::Type::BitOR, x, assign()), csm_tok);

			return x;
		}

		Node *expr()
		{
			return assign();
		}

		Node *stmt()
		{
			if( consume("{") )
			{
				Node *x = NewNode(Node::Type::Block);
				x->tok = csm_tok;
				bool closed = 0;

				while( check() )
				{
					if( consume("}") )
					{
						closed = 1;
						break;
					}

					x->list.push_back(stmt());
				}

				if( closed == 0 )
					Error(x->tok->pos, "unclosed block statement");

				return x;
			}

			if( consume("if") )
			{
				expect("(");
				Node *x = NewNode(Node::Type::If, expr(), nullptr, csm_tok);
				
				expect(")");
				x->rhs = stmt();

				if( consume("else") )
				{
					x->list.push_back(stmt());
				}

				return x;
			}

			if( consume("while") )
			{
				expect("(");
				Node *x = NewNode(Node::Type::While);
				x->tok = csm_tok;
				x->lhs = expr();

				expect(")");
				x->rhs = stmt();

				return x;
			}

			if( consume("do") )
			{
				Node *nd = NewNode(Node::Type::While);
				
				Node *s = NewNode(Node::Type::Block);
				s->list.push_back(stmt());
				
				expect("while");
				expect("(");
				s->list.push_back(NewNode(Node::Type::If, NewNode(Node::Type::Equal, expr(), NewNode_Int(0)), NewNode(Node::Type::Break)));
				expect(")");
				expect(";");

				nd->lhs = NewNode_Int(1);
				nd->rhs = s;

				return nd;
			}

			if( consume("for") )
			{
				expect("(");
				Node *x = NewNode(Node::Type::For);
				x->tok = csm_tok;

				for( int i = 0; i < 2; i++ ) {
					if( !consume(";") ) {
						x->list.push_back(expr());
						expect(";");
					}
					else
						x->list.push_back(nullptr);
				}

				if( !consume(")") ) {
					x->list.push_back(expr());
					expect(")");
				}
				else
					x->list.push_back(nullptr);

				x->lhs = stmt();
				return x;
			}

			if( consume("foreach") )
			{
				Token *tok = csm_tok;

				expect("(");
				
				Node *iterator = primary();
				if( iterator->type != Node::Type::Variable)
					Error(tok->pos, "iterator is must be a variable");

				expect("in");
				Node *arr = expr();

				expect(")");

				Node *counter = MakeUniqueVariable();

				std::vector<Node *> for_nodes;

				// making for-loop nodes
				{
					// first, set zero to counter
					for_nodes.push_back(NewNode(Node::Type::Assign, counter, NewNode_Int(0)));

					// second, smaller than length of array
					Node *Length_nd = NewNode(Node::Type::Variable);
					Length_nd->tok = NewToken(Token::Type::Ident);
					Length_nd->tok->str = "length";
						// copy token pointer for "not have length" error
						Length_nd = NewNode(Node::Type::MemberAccess, arr, Length_nd);
						Length_nd->tok = tok; // getting "node->tok->pos" when say error in member access with "length"
					for_nodes.push_back(NewNode(Node::Type::Bigger, counter, Length_nd));

					// last, inclement counter
					for_nodes.push_back(NewNode(Node::Type::Assign, counter, NewNode(Node::Type::Add, counter, NewNode_Int(1))));
				}

				Node *for_nd = NewNode(Node::Type::For);
				for_nd->list = std::move(for_nodes);

				// making statement
				Node *st = NewNode(Node::Type::Block);
				
				st->list.push_back(NewNode(Node::Type::Assign, iterator, NewNode(Node::Type::IndexRef, arr, counter)));
				st->list.push_back(stmt());

				for_nd->lhs = st;

				return for_nd;
			}

			if( consume("switch") )
			{
				size_t pos = csm_tok->pos;

				expect("(");

				Node *var = MakeUniqueVariable();
				Node *cond = NewNode(Node::Type::Assign, var, expr());

				expect(")");

				expect("{");

				if( consume("}") )
				{
					Error(pos, "This switch statement is empty");
				}

				std::vector<Node *> cases;

				do
				{
					Node *case_expr = nullptr;
					Node *case_block = NewNode(Node::Type::Block);

					if( !consume("default") )
					{
						expect("case");
						case_expr = NewNode(Node::Type::Equal, var, expr());
					}
					else
					{
						case_expr = NewNode_Int(1);
					}

					expect(":");

					while( g_tok->str != "case" && g_tok->str != "default" && g_tok->str != "}" )
					{
						case_block->list.push_back(stmt());
					}

					cases.push_back(NewNode(Node::Type::Case, case_expr, case_block));
				} while( !consume("}") );

				Node *nd = NewNode(Node::Type::Switch, cond, nullptr);
				nd->list = std::move(cases);
				return nd;
			}

			if( consume("break") )
			{
				expect(";");
				Node *x = NewNode(Node::Type::Break);
				x->tok = csm_tok;
				return x;
			}

			if( consume("continue") )
			{
				expect(";");
				Node *x = NewNode(Node::Type::Continue);
				x->tok = csm_tok;
				return x;
			}

			Node *x = expr();
			expect(";");
			return x;
		}

		Node *parse(Token *tok)
		{
			g_tok = tok;

			Node *nd = NewNode(Node::Type::Block);

			while( check() )
			{
				nd->list.push_back(stmt());
			}

			return nd;
		}

	}
}