#include "Utility/Tests/StackAllocatorTest.h"

namespace Utility::Test
{
	bool StackAllocatorTest::Run() const
	{
		std::function<bool()> TestFuncs[Test::Num] =
		{
			std::bind(&StackAllocatorTest::RunTestAllocate,   this),
			std::bind(&StackAllocatorTest::RunTestFree,		  this),
			std::bind(&StackAllocatorTest::RunTestReallocate, this)
		};

		usize testsPassed = 0;
		for (usize i = 0; i < Test::Num; ++i)
		{
			const bool testPassed = TestFuncs[i]();
			if (testPassed)
			{
				Log(LogFlag::Info, "Passed StackAllocatorTest::%s", TestStrings[i]);
			}
			else
			{
				Log(LogFlag::Error, "Failed StackAllocatorTest::%s", TestStrings[i]);
			}

			testsPassed += testPassed;
		}

		return (testsPassed == Test::Num);
	}


	bool StackAllocatorTest::RunTestAllocate() const
	{
		const char* TestStrings[5] =
		{
			"Hello, I'm a",
			"string being used",
			"to test the useful",
			"StackAllocator class",
			"as part of the current program!"
		};

		StackAllocator stackAllocator(1000);

		MemoryBlock stackBlocks[5];
		for (usize i = 0; i < 5; ++i)
		{
			// Copy strings onto stack mem
			stackBlocks[i] = stackAllocator.Allocate(strlen(TestStrings[i]) + 1);
			strcpy((char*)stackBlocks[i].ptr, TestStrings[i]);
		}

		for (usize i = 0; i < 5; ++i)
		{
			// Check strings still equal. strcmp != 0 is fail
			if (strcmp((char*)stackBlocks[i].ptr, TestStrings[i]))
			{
				return false;
			}
		}

		return true;
	}


	bool StackAllocatorTest::RunTestFree() const
	{
		const char* TestStrings[3] =
		{
			"This is a string that",
			"should remain valid",
			"even after calling Allocate()"
		};

		StackAllocator stackAllocator(1000);

		MemoryBlock stackBlocks[6];
		for (usize i = 0; i < 6; ++i)
		{
			// Copy strings onto stack mem
			if (i % 2)
			{
				stackBlocks[i] = stackAllocator.Allocate(strlen(TestStrings[(i - 1) / 2]) + 1);
				strcpy((char*)stackBlocks[i].ptr, TestStrings[(i - 1) / 2]);
			}
			else
			{
				// We'll free these as we read the other blocks
				stackBlocks[i] = stackAllocator.Allocate(100);
			}
		}

		for (i32 i = 5; i > -1; --i)
		{
			if (i % 2)
			{
				// Check strings still equal. strcmp != 0 is fail
				if (strcmp((char*)stackBlocks[i].ptr, TestStrings[(i - 1) / 2]))
				{
					return false;
				}
			}

			stackAllocator.Free(stackBlocks[i]);
		}

		return true;
	}


	bool StackAllocatorTest::RunTestReallocate() const
	{
		const char* TestStrings[5] =
		{
			"Hello, I'm a",
			"string being used",
			"to test the useful",
			"StackAllocator class",
			"as part of the current program!"
		};

		StackAllocator stackAllocator(1000);

		MemoryBlock stackBlocks[5];
		for (usize i = 0; i < 5; ++i)
		{
			// Allocate and then reallocate. Differ between over and undersized blocks
			stackBlocks[i] = stackAllocator.Allocate((i % 2) ? 100 : 1);
			stackBlocks[i] = stackAllocator.Reallocate(stackBlocks[i], strlen(TestStrings[i]) + 1);

			// Copy strings onto stack mem
			strcpy((char*)stackBlocks[i].ptr, TestStrings[i]);
		}

		for (usize i = 0; i < 5; ++i)
		{
			// Check strings still equal. strcmp != 0 is fail
			if (strcmp((char*)stackBlocks[i].ptr, TestStrings[i]))
			{
				return false;
			}
		}

		return true;
	}
}
