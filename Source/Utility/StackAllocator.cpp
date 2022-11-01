#include "Utility/StackAllocator.h"

namespace Utility
{
	StackAllocator::StackAllocator(const usize allocSize_)
	{
		if (!allocSize_)
		{
			Log(LogFlag::Error, "StackAllocator() called with allocSize_ == 0!");
			assert(0);
		}

		buffer = MemoryBlock(allocSize_, malloc(allocSize_));
		if (buffer.ptr == nullptr)
		{
			Log(LogFlag::Error, "malloc() error in StackAllocator()!");
			assert(0);
		}

		offset = 0;
	}


	StackAllocator::~StackAllocator()
	{
		// Can assume buffer != nullptr
		free(buffer.ptr);
	}


	MemoryBlock StackAllocator::Allocate(const usize size_)
	{
		// Check that we have space available
		if ((offset + size_) > buffer.size)
		{
			
			Log(LogFlag::Error, "StackAllocator::Allocate() %zu bytes requested, but only %zu available!", size_, (buffer.size - offset));
			assert(0);
		}

		// Create block and add size_ to offset
		const MemoryBlock retBlk = MemoryBlock(size_, (void*)((char*)buffer.ptr + offset));
		offset += size_;

		return retBlk;
	}


	MemoryBlock StackAllocator::Reallocate(MemoryBlock& oldBlock_, const usize newSize_)
	{
		// oldBlock_ must be last allocation. Perform check to make sure that this is the case
		const usize lookAtPos = (offset >= oldBlock_.size) ? (offset - oldBlock_.size) : -1;
		if (lookAtPos == -1)
		{
			Log(LogFlag::Error, "StackAllocator::Reallocate() called with incorrect original block!");
			assert(0);
		}

		// Perform the check
		void* lastAllocPtr = (void*)((char*)buffer.ptr + lookAtPos);
		if (lastAllocPtr != oldBlock_.ptr)
		{
			Log(LogFlag::Error, "StackAllocator::Reallocate() called with incorrect original block!");
			assert(0);
		}

		// Check we have the bytes available and resize the offset
		if ((lookAtPos + newSize_) > buffer.size)
		{
			Log(LogFlag::Error, "StackAllocator::Reallocate() %zu bytes required, but only %zu available!", newSize_, (buffer.size - lookAtPos));
			assert(0);
		}

		// Can now resize allocation
		offset = lookAtPos + newSize_;

		// Return new block and set old to null
		oldBlock_ = MemoryBlock();
		return MemoryBlock(newSize_, lastAllocPtr);
	}


	void StackAllocator::Free(MemoryBlock& block_)
	{
		// block_ must be last allocation. Perform check to make sure that this is the case
		const usize lookAtPos = (offset >= block_.size) ? (offset - block_.size) : -1;
		if (lookAtPos == -1)
		{
			Log(LogFlag::Error, "StackAllocator::Free() called with incorrect original block!");
			assert(0);
		}

		// Perform the check
		void* lastAllocPtr = (void*)((char*)buffer.ptr + lookAtPos);
		if (lastAllocPtr != block_.ptr)
		{
			Log(LogFlag::Error, "StackAllocator::Free() called with incorrect original block!");
			assert(0);
		}

		// All OK. Simply reduce offset by block_.size
		offset -= block_.size;
		block_  = MemoryBlock();
	}
}
