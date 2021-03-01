#include "types.h"
#include "tokenize.h"
#include "allocator.h"
#include "parser.h"

namespace Xscript
{
	namespace Parser
	{
		static Token *g_tok;

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

		Node *NewNode_Int(int val)
		{
			Node *nd = NewNode();
			nd->tok = NewToken();
			nd->tok->value.v_Int = val;
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

			if( g_tok->type == Token::Type::Int || g_tok->type == Token::Type::Char || g_tok->type == Token::Type::Float )
			{
				Node *nd = NewNode();
				nd->tok = g_tok;

				next();
				return nd;
			}

			Error(g_tok->pos, "syntax error");
		}

		Node *indexRef()
		{
			return primary();
		}

		Node *unary()
		{
			if( consume("-") )
			{
				return NewNode(Node::Type::Sub, NewNode_Int(0), primary());
			}

			return primary();
		}

		Node *mul()
		{
			Node *x = unary();

			while( check() )
			{
				if( consume("*") )
					x = NewNode(Node::Type::Mul, x, unary());
				else if( consume("/") )
					x = NewNode(Node::Type::Div, x, unary());
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
					x = NewNode(Node::Type::Add, x, mul());
				else if( consume("-") )
					x = NewNode(Node::Type::Sub, x, mul());
				else
					break;
			}

			return x;
		}

		Node *expr()
		{
			return add();
		}

		Node *stmt()
		{


			
		}

		Node *parse(Token *tok)
		{
			g_tok = tok;

			return expr();
		}

	}
}