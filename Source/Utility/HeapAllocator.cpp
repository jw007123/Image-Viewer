#include "Utility/HeapAllocator.h"

namespace Utility
{
	template <typename T>
	MemoryBlock HeapAllocator::Allocate(const usize n_)
	{
		return Allocate(sizeof(T) * n_);
	}


	MemoryBlock HeapAllocator::Allocate(const usize size_)
	{
		if (size_ == 0)
		{
			Log(Error, "HeapAllocator::Allocate() given size = 0!");
			assert(0);
		}

		// Get buffer from malloc, with extra space in case of moving ptr to alignment
		MemoryBlock retBlk = MemoryBlock(size_, malloc(size_ + alignment));

		// Align ptr
		const usize currentBuffer = (usize)retBlk.ptr;
		const usize alignedBuffer = AlignAddress(currentBuffer);
		const u8 alignmentSize	  = alignedBuffer - currentBuffer;

		// Store the alignment size in bytes at end of full block
		*((char*)retBlk.ptr + size_ + alignmentSize) = alignmentSize;

		// Move returned ptr along to align
		retBlk.ptr = ((char*)retBlk.ptr + alignmentSize);
		return retBlk;
	}


	template <typename T>
	MemoryBlock HeapAllocator::Reallocate(MemoryBlock& oldBlock_, const usize newN_)
	{
		return Reallocate(oldBlock_, sizeof(T) * newN_);
	}


	MemoryBlock HeapAllocator::Reallocate(MemoryBlock& oldBlock_, const usize newSize_)
	{
		// Set oldBlock empty and return the new one
		Free(oldBlock_);
		return Allocate(newSize_);
	}


	void HeapAllocator::Free(MemoryBlock& block_)
	{
		// Recover alignment size
		const u8 alignmentSize = *((char*)block_.ptr + block_.size);

		// Recover original ptr and free
		void* ogPtr = ((char*)block_.ptr - alignmentSize);
		free(ogPtr);

		// Empty block
		block_ = MemoryBlock();
	}


	inline usize HeapAllocator::AlignAddress(const usize address_) const
	{
		// Compiler will optimise to the usual
		const usize pad = (alignment - (address_ % alignment)) % alignment;
		return address_ + pad;
	}
}
