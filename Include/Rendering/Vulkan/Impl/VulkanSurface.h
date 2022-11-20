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
		VulkanSurface(VulkanInstance&	       vulkanInstance_,		 VulkanGLFW&		   vulkanGLFW_,
					  VulkanQueueFamilies&     vulkanQueueFamilies_, VulkanPhysicalDevice& vulkanPhysicalDevice_,
					  Utility::HeapAllocator&  heapAllocator_);
		~VulkanSurface();

		VkSurfaceKHR&				  GetVkSurface();
		uint32_t					  GetRecommendedImageCount() const;
		VkSurfaceTransformFlagBitsKHR GetRecommendedTransform() const;
 
		bool RequestFormat(VkSurfaceFormatKHR& surfaceformat_, const VkFormat type_) const;
		bool RequestPresentMode(VkPresentModeKHR& presentMode_, const VkPresentModeKHR type_) const;
		bool RequestExtent(VkExtent2D& extent_) const;

	private:
		struct SwapChainSupportInfo
		{
			uint32_t nFormats;
			uint32_t nPresentModes;

			VkSurfaceFormatKHR* formats;
			VkPresentModeKHR*   presentModes;
		} swapChainSupportInfo;

		VkSurfaceKHR vulkSurface;

		VulkanGLFW&			    vulkanGLFW;
		VulkanInstance&		    vulkanInstance;
		VulkanQueueFamilies&    vulkanQueueFamilies;
		VulkanPhysicalDevice&   vulkanPhysicalDevice;
		Utility::HeapAllocator& heapAllocator;

		bool CheckPresentingSupport();
		bool CheckSwapchainSupport();
	};
}
