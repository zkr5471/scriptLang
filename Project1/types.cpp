#include <iostream>
#include "types.h"

namespace Xscript
{
	bool Value::is_string() const
	{
		if( type != Type::Array )
			return 0;

		for( auto &&i : list )
		{
			if( i.type != Type::Char )
				return 0;
		}

		return list.size() != 0;
	}

	bool Value::eval() const
	{
		if( type == Type::Array )
			return list.size() != 0;

		return v_Int != 0 || v_Char != 0 || v_Float != 0;
	}
	
	bool Value::equals(Value const &val) const
	{
		if( type != val.type )
			return 0;

		switch( type )
		{
			case Type::Int:
				return v_Int == val.v_Int;

			case Type::Float:
				return v_Float == val.v_Float;

			case Type::Char:
				return v_Char == val.v_Char;

			case Type::Array:
				if( list.size() != val.list.size() )
					return 0;

				for( size_t i = 0; i < list.size(); i++ )
					if( list[i].equals(val.list[i]) == 0 )
						return 0;

				return 1;
		}

		return 0;
	}

	Value &Value::operator= (Value const &val)
	{
		type = val.type;
		v_Int = val.v_Int;
		v_Float = val.v_Float;
		v_Char = val.v_Char;
		list = val.list;

		return *this;
	}

	std::ostream &operator << (std::ostream &ost, Value const &val)
	{
		if( val.is_string() )
		{
			for( auto &&i : val.list )
				ost << i.v_Char;

			return ost;
		}

		switch( val.type )
		{
			case Value::Type::Int:
				ost << val.v_Int;
				break;

			case Value::Type::Float:
				ost << val.v_Float;
				break;

			case Value::Type::Char:
				ost << val.v_Char;
				break;

			case Value::Type::Array:
			{
				ost << '[';
				for( size_t i = 0; i < val.list.size(); i++ )
				{
					ost << val.list[i];
					if( i < val.list.size() - 1 ) ost << ", ";
				}
				ost << ']';
				break;
			}
		}

		return ost;
	}
}