#pragma once

#define  ERROR_ILLEGAL_PARAM  "関数の呼び出しに渡す引数が間違っています。"
#define  ERROR_STACK_OVERFROW "関数の呼び出しが深すぎます。"

#define  ERR_SEE_OPTION  "(このエラーはオプションによって無効化できます。-help コマンドを確認してください)"

namespace Xscript
{
	namespace Options
	{
		extern string input_file;

		namespace Safety
		{
			extern bool stack;
		}
	}
}