#pragma once

#define  ERROR_ILLEGAL_PARAM  "�֐��̌Ăяo���ɓn���������Ԉ���Ă��܂��B"
#define  ERROR_STACK_OVERFROW "�֐��̌Ăяo�����[�����܂��B"

#define  ERR_SEE_OPTION  "(���̃G���[�̓I�v�V�����ɂ���Ė������ł��܂��B-help �R�}���h���m�F���Ă�������)"

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