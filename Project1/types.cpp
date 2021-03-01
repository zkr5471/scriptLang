#include "types.h"

namespace Xscript
{
	Value &Value::operator= (Value const &val)
	{
		type = val.type;
		v_Int = val.v_Int;
		v_Float = val.v_Float;
		v_Char = val.v_Char;
		list = val.list;

		return *this;
	}
}