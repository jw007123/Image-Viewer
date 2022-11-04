#pragma once

#include <functional>

#include "Utility/StackAllocator.h"

namespace Utility::Test
{
	class StackAllocatorTest
	{
	public:
		bool Run() const;

	private:
		enum Test : u8
		{
			Allocate   = 0,
			Free	   = 1,
			Reallocate = 2,
			Num
		};

		static constexpr const char* TestStrings[Test::Num] =
		{
			"Allocate",
			"Free",
			"Reallocate"
		};

		bool RunTestAllocate()	 const;
		bool RunTestFree()		 const;
		bool RunTestReallocate() const;
	};
}
