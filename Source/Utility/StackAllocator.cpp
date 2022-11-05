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

		// Setup buffer
		buffer = MemoryBlock(allocSize_, malloc(allocSize_));
		
		// Determine aligned buffer and offset by amount
		const usize alignedBuffer = AlignAddress((usize)buffer.ptr);
		offset					  = (alignedBuffer - (usize)buffer.ptr);
	}


	StackAllocator::~StackAllocator()
	{
		// Can assume buffer != nullptr
		free(buffer.ptr);
	}


	template <typename T>
	MemoryBlock StackAllocator::Allocate(const usize n_)
	{
		return Allocate(sizeof(T) * n_);
	}


	MemoryBlock StackAllocator::Allocate(const usize size_)
	{
		// Check that we have space available
		if ((offset + (alignment + size_)) > buffer.size)
		{
			Log(LogFlag::Error, "StackAllocator::Allocate() %zu bytes requested, but only %zu available!", (alignment + size_), (buffer.size - offset));
			assert(0);
		}

		// Align the buffer
		const usize currentBuffer	  = (usize)buffer.ptr + offset;
		const usize alignedBuffer	  = AlignAddress(currentBuffer);
		offset						 += (alignedBuffer - currentBuffer);

		// Create block and add size_ to offset
		const MemoryBlock retBlk = MemoryBlock(size_, (void*)((char*)buffer.ptr + offset));
		offset					+= size_;

		// Store alignment 
		*((char*)buffer.ptr + offset) = (alignedBuffer - currentBuffer);
		offset						 += 1;

		return retBlk;
	}


	template <typename T>
	MemoryBlock StackAllocator::Reallocate(MemoryBlock& oldBlock_, const usize newN_)
	{
		return Reallocate(oldBlock_, sizeof(T) * newN_);
	}


	MemoryBlock StackAllocator::Reallocate(MemoryBlock& oldBlock_, const usize newSize_)
	{
		// Set oldBlock empty and return the new one
		Free(oldBlock_);
		return Allocate(newSize_);
	}


	void StackAllocator::Free(MemoryBlock& block_)
	{
		// Recover alignment size
		offset				  -= 1;
		const u8 alignmentSize = *((char*)buffer.ptr + offset);

		// Reduce offset by block size and alignment size
		offset -= (block_.size + alignmentSize);

		block_ = MemoryBlock();
	}


	inline usize StackAllocator::AlignAddress(const usize address_) const
	{
		// Compiler will optimise to the usual
		const usize pad = (alignment - (address_ % alignment)) % alignment;
		return address_ + pad;
	}
}
