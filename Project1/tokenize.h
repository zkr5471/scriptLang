#pragma once

#include "types.h"

namespace Xscript
{
	[[noreturn]]
	void Error(size_t errpos, string msg);

	void Warning(size_t pos, string msg);

	Token *tokenize(string &&src);


}