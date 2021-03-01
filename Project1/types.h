#pragma once

#include <string>
#include <vector>
#include "utils.h"

#define  ALART  { size_t L=__LINE__; \
  char const* s=__FILE__; \
	for(int64_t i=strlen(s);i>=0;i--) { \
    if(s[i]=='\\'){s=s+i+1;break;} \
 }fprintf(stderr, " # %s:%zd\n", s, L); \
}


namespace Xscript
{
	using std::string;

	struct Value
	{
		enum class Type
		{
			Int,
			Float,
			Char,
			Array
		};

		Type type = Type::Int;
		string name;

		int v_Int = 0;
		float v_Float = 0;
		char v_Char = 0;
		std::vector<Value> list;

		Value *var_ptr = nullptr;

		Value &operator = (Value const &value);
	};

	struct Token
	{
		enum class Type
		{
			Int,
			Float,
			Char,
			String,
			Ident,
			Reserved,
			End
		};

		Type  type = Type::Int;
		string  str;
		Value value;
		Token *next = nullptr;

		size_t pos = 0;

	};

	struct Node
	{
		enum class Type
		{
			Add,
			Sub,
			Mul,
			Div,

			Value
		};

		Type type;
		Node *lhs = nullptr;
		Node *rhs = nullptr;
		Token *tok = nullptr;

		size_t varIndex = 0;

	};
}