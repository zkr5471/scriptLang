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

	template <typename T>
	std::vector<T> &operator *= (std::vector<T> &vec, size_t count)
	{
		auto temp = vec;

		for( size_t i = 1; i < count; i++ )
			temp += vec;

		return vec = temp;
	}

	namespace Utils
	{
		void RandomInit();

		int Random(int range);
		int Random(int begin, int end);

		string GetRandomStr();
		string GetRandomStr(size_t min, size_t max);
	}
}