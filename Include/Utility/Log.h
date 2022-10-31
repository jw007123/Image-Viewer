#pragma once

#include "Utility/Literals.h"

#include <stdio.h>
#include <stdarg.h>

namespace Utility
{
	static enum LogFlag : u8
	{
		Info  = 0,
		Warn  = 1,
		Error = 2,
		Num
	};

	static constexpr const char* LogStrings[LogFlag::Num] =
	{
		"LOG INFO",
		"LOG WARN",
		"LOG ERROR"
	};

	static void Log(const LogFlag logFlag_, const char* str_, ...);
}
