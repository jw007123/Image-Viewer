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
		VulkanInstance(Utility::StackAllocator& stackAllocator_);
		~VulkanInstance();

	private:
		struct Consts
		{
			static constexpr usize maxNameLength = 1024;
		};

		VkInstance vulkInstance;

		/// Returns false if some required Vk extensions are missing
		bool CheckRequiredExtensions(Utility::StackAllocator& stackAllocator_, const usize nRequiredExtensions_, const char** requiredExtensions_) const;
	};
}
