#include <string>
#include <vector>
#include "tokenize.h"
#include "allocator.h"

namespace Xscript
{
	namespace _allocated
	{
		static std::vector<Token *> tokens;
		static std::vector<Node *> nodes;
	}

	using namespace _allocated;

	Token *NewToken(Token::Type type)
	{
		Token *tok = new Token;
		tok->type = type;
		
		tokens.push_back(tok);
		return tok;
	}

	Token *NewToken(Token::Type type, Token *back, size_t pos)
	{
		Token *tok = NewToken(type);
		tok->pos = pos;
		
		if( back )
			back->next = tok;

		return tok;
	}
	
	Node *NewNode(Node::Type type)
	{
		Node *nd = new Node;
		nd->type = type;

		nodes.push_back(nd);
		return nd;
	}

	Node *NewNode(Node::Type type, Node *lhs, Node *rhs, Token *tok)
	{
		Node *nd = NewNode(type);
		nd->lhs = lhs;
		nd->rhs = rhs;
		nd->tok = tok;
		return nd;
	}

	void FreeMemory()
	{
		for( auto &&ptr : tokens )
		{
			delete ptr;
		}

		tokens.clear();
	}

}