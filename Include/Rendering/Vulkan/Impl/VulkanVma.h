#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"

#include <GLFW/glfw3.h>

#include "vma/vk_mem_alloc.h"

#include "Rendering/Vulkan/Impl/VulkanPhysicalDevice.h"
#include "Rendering/Vulkan/Impl/VulkanLogicalDevice.h"
#include "Rendering/Vulkan/Impl/VulkanInstance.h"

namespace Rendering
{
	class VulkanVma
	{
	public:
		VulkanVma(VulkanPhysicalDevice& vulkanPhysicalDevice_, VulkanLogicalDevice& vulkanLogicalDevice_,
															   VulkanInstance& vulkanInstance_);
		~VulkanVma();

	private:
		VmaAllocator vmemAllocator;

		VulkanPhysicalDevice& vulkanPhysicalDevice;
		VulkanLogicalDevice&  vulkanLogicalDevice;
		VulkanInstance&		  vulkanInstance;
	};
}
