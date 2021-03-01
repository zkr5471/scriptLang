#pragma once

#include "types.h"

namespace Xscript
{
	[[noreturn]]
	void Error(size_t errpos, string msg);

	Token *tokenize(string &&src);


}