#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"

#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Rendering/Vulkan/Impl/VulkanInstance.h"
#include "Rendering/Vulkan/Impl/VulkanGLFW.h"
#include "Rendering/Vulkan/Impl/VulkanQueueFamilies.h"

namespace Rendering
{
	class VulkanSurface
	{
	public:
		VulkanSurface(VulkanInstance& vulkanInstance_, VulkanGLFW& vulkanGLFW_,
					  VulkanQueueFamilies& vulkanQueueFamilies_, VulkanPhysicalDevice& vulkanPhysicalDevice_);
		~VulkanSurface();

		VkSurfaceKHR& GetVkSurface();

	private:
		VkSurfaceKHR vulkSurface;

		VulkanGLFW&			  vulkanGLFW;
		VulkanInstance&		  vulkanInstance;
		VulkanQueueFamilies&  vulkanQueueFamilies;
		VulkanPhysicalDevice& vulkanPhysicalDevice;
	};
}
