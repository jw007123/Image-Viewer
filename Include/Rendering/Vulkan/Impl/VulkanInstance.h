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

		VkInstance& GetVkInstance();

	private:
		struct Consts
		{
			static constexpr usize nRequiredLayers							   = 1;
			static constexpr const char* debugValidationLayer[nRequiredLayers] =
			{
				"VK_LAYER_KHRONOS_validation"
			};
		};

		VkInstance vulkInstance;

		bool CheckRequiredValidationLayers(Utility::StackAllocator& stackAllocator_, const usize nRequiredLayers_, const char* const* requiredLayers_) const;
		bool CheckRequiredExtensions(Utility::StackAllocator& stackAllocator_, const usize nRequiredExtensions_, const char* const* requiredExtensions_) const;
	};
}
