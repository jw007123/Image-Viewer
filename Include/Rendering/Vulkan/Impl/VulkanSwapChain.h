#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"

#include "vma/vk_mem_alloc.h"

#include "Rendering/Vulkan/Impl/VulkanUtility.h"
#include "Rendering/Vulkan/Impl/VulkanPhysicalDevice.h"

namespace Rendering
{
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(Utility::StackAllocator& stackAllocator_,      VulkanPhysicalDevice&    vulkanPhysicalDevice_,
						VulkanSurface&			 vulkanSurface_,       VulkanQueueFamilies&     vulkanQueueFamilies_,
						VulkanLogicalDevice&     vulkanLogicalDevice_, const VkImageUsageFlags  usage_);
		~VulkanSwapChain();

	private:
		VkSwapchainKHR vulkSwapChain;

		VulkanSurface&		  vulkanSurface;
		VulkanPhysicalDevice& vulkanPhysicalDevice;
		VulkanQueueFamilies&  vulkanQueueFamilies;
		VulkanLogicalDevice&  vulkanLogicalDevice;

		void FillQueueInfo(VkSwapchainCreateInfoKHR& scCreateInfo_);
	};
}
