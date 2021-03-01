#pragma once

namespace Xscript
{
	Token *NewToken(Token::Type type = Token::Type::Int);
	Token *NewToken(Token::Type type, Token *back, size_t pos);

	Node *NewNode(Node::Type type = Node::Type::Value);
	Node *NewNode(Node::Type type, Node *lhs, Node *rhs, Token *tok = nullptr);

	void FreeMemory();

}