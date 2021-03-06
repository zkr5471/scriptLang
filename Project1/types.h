#pragma once

#include <string>
#include <vector>

using std::string;
#include "utils.h"

#define  ALART  { size_t L=__LINE__; \
  char const* s=__FILE__; \
	for(int64_t i=strlen(s);i>=0;i--) { \
    if(s[i]=='\\'){s=s+i+1;break;} \
 }fprintf(stderr, " # %s:%zd\n", s, L); \
}

#define  DEBUG_MSG(msg)  fprintf(stderr,"%s:%s\n\t%s\n",__FILE__,__func__,msg)

namespace Xscript
{
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

		bool is_string() const;
		bool eval() const;
		bool equals(Value const &val) const;
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
			Punctuator,
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
			Mod,
			ShiftL,
			ShiftR,
			Bigger,
			BiggerOrEqual,
			Equal,
			NotEqual,
			BitAND,
			BitNOT,
			BitXOR,
			BitOR,
			Assign,
			
			Not,
			And,
			Or,

			Block,
			If,
			For,
//			Foreach,
	//		While,
			Break,
			Continue,

			Switch,
			Case,

	//		RunUserFunction,
			Function,
			Param,
			Return,

			Array,
			Value,
			Variable,
			Callfunc,
			IndexRef,

			MemberAccess
		};

		Type type = Node::Type::Value;
		Node *lhs = nullptr;
		Node *rhs = nullptr;
		Token *tok = nullptr;

		size_t varIndex = 0;

		std::vector<Node *> list;
	//	std::vector<Value> params; // function
	};

	std::ostream &operator << (std::ostream &ost, Value const &val);
}