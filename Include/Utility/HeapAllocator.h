#pragma once

#include <malloc.h>
#include <assert.h>

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/MemoryBlock.h"

namespace Utility
{
	class HeapAllocator
	{
	public:
		template <typename T>
		MemoryBlock Allocate(const usize n_);
		MemoryBlock Allocate(const usize size_);

		/// Sets oldBlock_ as empty on return
		template <typename T>
		MemoryBlock Reallocate(MemoryBlock& oldBlock_, const usize newN_);
		MemoryBlock Reallocate(MemoryBlock& oldBlock_, const usize newSize_);

		/// Sets block_ as empty on return
		void Free(MemoryBlock& block_);

	private:
		static constexpr u8 alignment = sizeof(usize);
		inline usize AlignAddress(const usize address_) const;
	};
}
