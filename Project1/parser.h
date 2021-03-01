#pragma once

namespace Xscript
{
	namespace Parser
	{
		bool check();
		void next();

		bool consume(string str);
		void expect(string str);

		Node *NewNode_Int(int val);

		Node *primary();
		Node *indexRef();
		Node *unary();
		Node *mul();
		Node *add();
		Node *expr();
		Node *stmt();

		Node *parse(Token *tok);
	}
}