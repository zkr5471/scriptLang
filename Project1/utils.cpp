#include <ctime>
#include "tokenize.h"

namespace Xscript
{
	namespace Utils
	{
		void RandomInit()
		{
			srand((unsigned)time(NULL));
		}

		int Random(int range)
		{
			return rand() % range;
		}

		int Random(int begin, int end)
		{
			if( begin > end )
				std::swap(begin, end);

			if( begin == end )
				return begin;

			return begin + (rand() % (end - begin));
		}

		string GetRandomStr()
		{
			return GetRandomStr(15, 30);
		}

		string GetRandomStr(size_t min, size_t max)
		{
			int len = Random(min, max);
			string ret;

			for( int i = 0; i < len; i++ )
			{
				if( Random(10) == 0 )
					ret += '_';
				else
				{
					switch( Random(3) )
					{
						case 0:
							ret += (char)Random('a', 'z');
							break;

						case 1:
							ret += (char)Random('A', 'Z');
							break;

						case 2:
							ret += (char)Random('0', '9');
							break;
					}
				}
			}

			return ret;
		}
	}
}