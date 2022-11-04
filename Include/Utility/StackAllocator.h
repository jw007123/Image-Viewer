#pragma once

#include <malloc.h>
#include <assert.h>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/MemoryBlock.h"

namespace Utility
{
	class StackAllocator
	{
	public:
		/// Bytes available set to allocSize_
		StackAllocator(const usize allocSize_);
		~StackAllocator();

		MemoryBlock Allocate(const usize size_);

		/// Sets oldBlock_ as empty on return
		MemoryBlock Reallocate(MemoryBlock& oldBlock_, const usize newSize_);

		/// Sets block_ as empty on return
		void Free(MemoryBlock& block_);

	private:
		MemoryBlock buffer;
		usize		offset;

		static constexpr u8 alignment = sizeof(usize);
		inline usize AlignAddress(const usize address_) const;
	};
}
