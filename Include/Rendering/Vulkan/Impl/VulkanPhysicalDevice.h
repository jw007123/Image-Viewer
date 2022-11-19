#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"

#include "vma/vk_mem_alloc.h"

#include "Rendering/Vulkan/Impl/VulkanUtility.h"
#include "Rendering/Vulkan/Impl/VulkanInstance.h"

namespace Rendering
{
	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice(Utility::StackAllocator& stackAllocator_, VulkanInstance& vulkanInstance_);
		~VulkanPhysicalDevice();

		VkPhysicalDevice& GetVkPhysicalDevice();

	private:
		VkPhysicalDevice vulkPhysicalDevice;
		VulkanInstance&	 vulkanInstance;

		bool CheckRequiredDeviceProperties(Utility::StackAllocator& stackAllocator_, const VkPhysicalDevice& testVulkDevice_) const;
	};
}
