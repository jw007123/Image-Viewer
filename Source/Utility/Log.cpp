#include "Utility/Log.h"

namespace Utility
{
	void Log(const LogFlag logFlag_, const char* str_, ...)
	{
		constexpr usize buffSize = 1024;
		char buff[buffSize];

		va_list args;
		va_start(args, str_);

		vsnprintf(buff, buffSize, str_, args);
		printf("%s: %s\n", LogStrings[logFlag_], buff);

		va_end(args);
	}
}
