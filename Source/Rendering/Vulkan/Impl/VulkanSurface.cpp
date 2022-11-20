#include "Rendering/Vulkan/Impl/VulkanSurface.h"

namespace Rendering
{
	VulkanSurface::VulkanSurface(VulkanInstance& vulkanInstance_, VulkanGLFW& vulkanGLFW_,
								 VulkanQueueFamilies& vulkanQueueFamilies_, VulkanPhysicalDevice& vulkanPhysicalDevice_) :
								 vulkanInstance(vulkanInstance_),
								 vulkanGLFW(vulkanGLFW_),
								 vulkanQueueFamilies(vulkanQueueFamilies_),
								 vulkanPhysicalDevice(vulkanPhysicalDevice_)
	{
		// Use glfw to handle OS-code
		VULK_ASSERT_SUCCESS(glfwCreateWindowSurface, vulkanInstance.GetVkInstance(),
													 vulkanGLFW.GetGLWindow(),
													 nullptr, &vulkSurface);

		// Check that presenting queue supports this surface
		VkBool32 supported;
		VULK_ASSERT_SUCCESS(vkGetPhysicalDeviceSurfaceSupportKHR, vulkanPhysicalDevice.GetVkPhysicalDevice(),
																  vulkanQueueFamilies.GetQueueHandle(VulkanQueueFamilies::Presenting),
																  vulkSurface, &supported);

		if (supported != VK_TRUE)
		{
			// Pointless without support
			assert(0);
		}
	}


	VulkanSurface::~VulkanSurface()
	{
		vkDestroySurfaceKHR(vulkanInstance.GetVkInstance(), vulkSurface, nullptr);
	}


	VkSurfaceKHR& VulkanSurface::GetVkSurface()
	{
		return vulkSurface;
	}
}
