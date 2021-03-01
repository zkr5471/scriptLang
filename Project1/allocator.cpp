#include <string>
#include <vector>
#include "tokenize.h"
#include "allocator.h"

namespace Xscript
{
	namespace _allocated
	{
		static std::vector<Token *> tokens;
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

	void FreeMemory()
	{
		for( auto &&ptr : tokens )
		{
			delete ptr;
		}

		tokens.clear();
	}

}