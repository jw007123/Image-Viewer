#include "Utility/MemoryBlock.h"

namespace Utility
{
	MemoryBlock::MemoryBlock()
	{
		size = 0;
		ptr  = nullptr;
	}


	MemoryBlock::MemoryBlock(const usize size_, void* const ptr_)
	{
		size = size_;
		ptr  = ptr_;
	}
}
