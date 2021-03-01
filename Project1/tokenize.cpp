#include <iostream>
#include <string>
#include "tokenize.h"
#include "allocator.h"

namespace Xscript
{
	namespace
	{
		string  source;
		size_t  srcpos;

		char const *punctuators[] =
		{
			"+=",
			"-=",
			"*=",
			"/=",
			">>",
			"<<",
			"==",
			"!=",
			">=",
			"<=",
			">",
			"<",
			"=",
			"+",
			"-",
			"*",
			"/",
			"(",
			")",
			"[",
			"]",
			"{",
			"}",
			",",
			".",
			";",
		};
	}

	[[noreturn]]
	void Error(size_t errpos, string msg)
	{
		std::cout << std::to_string(errpos) << ": " << msg;
		throw 0;
	}

	Token *tokenize(string &&src)
	{
		Token top;
		Token *cur = &top;

		source = std::move(src);
		srcpos = 0;

		auto peek = [] {
			return source[srcpos];
		};

		auto next = [] {
			srcpos++;
		};

		auto check = [] {
			return srcpos < source.length();
		};

		auto pass_space = [=] {
			while( check() && peek() <= ' ' )
				next();
		};

		pass_space();

		while( check() )
		{
			char c = peek();
			size_t pos = srcpos;
			string str;

			// number
			if( isdigit(c) )
			{
				while( isdigit(c = peek()) )
				{
					str += c;
					next();
				}

				cur = NewToken(Token::Type::Int, cur, pos);
				cur->value.v_Int = std::stoi(str);
				cur->str = str;

				if( peek() == '.' )
				{
					str += '.';

					next();
					while( isdigit(c = peek()) )
					{
						str += c;
						next();
					}

					cur->type = Token::Type::Float;
					cur->value.v_Float = std::stof(cur->str = str);
					cur->value.type = Value::Type::Float;
				}
			}

			// identifier
			else if( isalpha(c) || c == '_' )
			{
				while( isalnum(c = peek()) || c == '_' )
				{
					str += c;
					next();
				}

				cur = NewToken(Token::Type::Ident, cur, pos);
				cur->str = str;
			}

			// string
			else if( c == '"' )
			{
				next();
				while( (c = peek()) != '"' )
				{
					str += c;
					next();
				}

				next();
				cur = NewToken(Token::Type::String, cur, pos);
				cur->str = str;

				cur->value.type = Value::Type::Array;

				for( auto &&c : str )
				{
					Value val;
					val.type = Value::Type::Char;
					val.v_Char = c;
					cur->value.list.push_back(val);
				}
			}

			// character
			else if( c == '\'' )
			{
				next();

				cur = NewToken(Token::Type::Char, cur, pos);
				cur->str += cur->value.v_Char = peek();
				cur->value.type = Value::Type::Char;

				next();

				if( peek() != '\'' )
				{
					Error(pos, "unclosed character literal");
				}

				next();
			}

			else // punctuator
			{
				bool find = false;

				for( string tok : punctuators )
				{
					size_t len = tok.length();

					if( pos + len <= source.length() && source.substr(pos, len) == tok )
					{
						cur = NewToken(Token::Type::Reserved, cur, pos);
						cur->str = tok;

						find = true;
						srcpos += len;

						break;
					}
				}

				if( find == false )
				{
					Error(pos, "unknown token");
				}
			}

			pass_space();
		}

		NewToken(Token::Type::End, cur, srcpos);

		return top.next;
	}
}