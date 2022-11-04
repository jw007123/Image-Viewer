#include "Utility/Tests/HeapAllocatorTest.h"

namespace Utility::Test
{
	bool HeapAllocatorTest::Run() const
	{
		std::function<bool()> TestFuncs[Test::Num] =
		{
			std::bind(&HeapAllocatorTest::RunTestAllocate,   this),
			std::bind(&HeapAllocatorTest::RunTestFree,		 this),
			std::bind(&HeapAllocatorTest::RunTestReallocate, this)
		};

		usize testsPassed = 0;
		for (usize i = 0; i < Test::Num; ++i)
		{
			const bool testPassed = TestFuncs[i]();
			if (testPassed)
			{
				Log(LogFlag::Info, "Passed HeapAllocatorTest::%s", TestStrings[i]);
			}
			else
			{
				Log(LogFlag::Error, "Failed HeapAllocatorTest::%s", TestStrings[i]);
			}

			testsPassed += testPassed;
		}

		return (testsPassed == Test::Num);
	}


	bool HeapAllocatorTest::RunTestAllocate() const
	{
		const char* TestStrings[5] =
		{
			"Hello, I'm a",
			"string being used",
			"to test the useful",
			"HeapAllocator class",
			"as part of the current program!"
		};

		HeapAllocator heapAllocator;

		MemoryBlock heapBlocks[5];
		for (usize i = 0; i < 5; ++i)
		{
			// Copy strings onto stack mem
			heapBlocks[i] = heapAllocator.Allocate(strlen(TestStrings[i]) + 1);
			strcpy((char*)heapBlocks[i].ptr, TestStrings[i]);
		}

		for (usize i = 0; i < 5; ++i)
		{
			// Check strings still equal. strcmp != 0 is fail
			if (strcmp((char*)heapBlocks[i].ptr, TestStrings[i]))
			{
				return false;
			}
		}

		return true;
	}


	bool HeapAllocatorTest::RunTestFree() const
	{
		const char* TestStrings[3] =
		{
			"This is a string that",
			"should remain valid",
			"even after calling Allocate()"
		};

		HeapAllocator heapAllocator;

		MemoryBlock heapBlocks[6];
		for (usize i = 0; i < 6; ++i)
		{
			// Copy strings onto stack mem
			if (i % 2)
			{
				heapBlocks[i] = heapAllocator.Allocate(strlen(TestStrings[(i - 1) / 2]) + 1);
				strcpy((char*)heapBlocks[i].ptr, TestStrings[(i - 1) / 2]);
			}
			else
			{
				// We'll free these as we read the other blocks
				heapBlocks[i] = heapAllocator.Allocate(100);
			}
		}

		for (i32 i = 5; i > -1; --i)
		{
			if (i % 2)
			{
				// Check strings still equal. strcmp != 0 is fail
				if (strcmp((char*)heapBlocks[i].ptr, TestStrings[(i - 1) / 2]))
				{
					return false;
				}
			}

			heapAllocator.Free(heapBlocks[i]);
		}

		return true;
	}


	bool HeapAllocatorTest::RunTestReallocate() const
	{
		const char* TestStrings[5] =
		{
			"Hello, I'm a",
			"string being used",
			"to test the useful",
			"HeapAllocator class",
			"as part of the current program!"
		};

		HeapAllocator heapAllocator;

		MemoryBlock heapBlocks[5];
		for (usize i = 0; i < 5; ++i)
		{
			// Allocate and then reallocate. Differ between over and undersized blocks
			heapBlocks[i] = heapAllocator.Allocate((i % 2) ? 100 : 1);
			heapBlocks[i] = heapAllocator.Reallocate(heapBlocks[i], strlen(TestStrings[i]) + 1);

			// Copy strings onto stack mem
			strcpy((char*)heapBlocks[i].ptr, TestStrings[i]);
		}

		for (usize i = 0; i < 5; ++i)
		{
			// Check strings still equal. strcmp != 0 is fail
			if (strcmp((char*)heapBlocks[i].ptr, TestStrings[i]))
			{
				return false;
			}
		}

		return true;
	}
}
