#include <iostream>
#include "types.h"

namespace Xscript
{
	bool Value::is_string() const
	{
		for( auto &&i : list )
		{
			if( i.type != Type::Char )
				return 0;
		}

		return list.size() != 0;
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