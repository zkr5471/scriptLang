#pragma once

namespace Xscript
{
	template <typename T>
	std::vector<T> &operator += (std::vector<T> &_Left, std::vector<T> const &_Right)
	{
		for( auto &&x : _Right )
			_Left.push_back(x);

		return _Left;
	}

}