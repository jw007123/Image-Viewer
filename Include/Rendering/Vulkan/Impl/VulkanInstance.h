#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"

#include <GLFW/glfw3.h>

#include "vma/vk_mem_alloc.h"

#include "Rendering/Vulkan/Impl/VulkanUtility.h"

namespace Rendering
{
	class VulkanInstance
	{
	public:
		VulkanInstance();
		~VulkanInstance();

		void Init(Utility::StackAllocator& stackAllocator_);

	private:
		VkInstance vulkInstance;
		bool	   initd;

		/// Returns false if some required Vk extensions are missing
		bool CheckRequiredExtensions(Utility::StackAllocator& stackAllocator_, const usize nExtensions_, const char** extensions_) const;
	};
}
