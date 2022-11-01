#pragma once

#include "Utility/Literals.h"

namespace Utility
{
	struct MemoryBlock
	{
		MemoryBlock();
		MemoryBlock(const usize size_, void* const ptr_);

		usize size;
		void* ptr;
	};
}
