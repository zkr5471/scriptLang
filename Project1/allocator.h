#pragma once

namespace Xscript
{
	Token *NewToken(Token::Type type = Token::Type::Int);
	Token *NewToken(Token::Type type, Token *back, size_t pos);


	void FreeMemory();

}