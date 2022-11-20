#include "Rendering/Vulkan/Impl/VulkanSurface.h"

namespace Rendering
{
	VulkanSurface::VulkanSurface(VulkanInstance&	      vulkanInstance_,		VulkanGLFW&		      vulkanGLFW_,
								 VulkanQueueFamilies&     vulkanQueueFamilies_, VulkanPhysicalDevice& vulkanPhysicalDevice_,
								 Utility::HeapAllocator&  heapAllocator_) :
								 vulkanInstance(vulkanInstance_),
								 vulkanGLFW(vulkanGLFW_),
								 vulkanQueueFamilies(vulkanQueueFamilies_),
								 vulkanPhysicalDevice(vulkanPhysicalDevice_),
								 heapAllocator(heapAllocator_)
	{
		// Order here matters. Latter func populates swapChainSupportInfo
		if (!CheckPresentingSupport() || !CheckSwapchainSupport())
		{
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


	uint32_t VulkanSurface::GetRecommendedImageCount() const
	{
		VkSurfaceCapabilitiesKHR vulkSurfaceCaps = {};
		VULK_ASSERT_SUCCESS(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, vulkanPhysicalDevice.GetVkPhysicalDevice(), vulkSurface, &vulkSurfaceCaps);

		// c.f. p88 Vulkan Tutorial
		uint32_t nRecommendedImages = vulkSurfaceCaps.minImageCount + 1;
		if (vulkSurfaceCaps.maxImageCount > 0 && (nRecommendedImages > vulkSurfaceCaps.maxImageCount))
		{
			nRecommendedImages = vulkSurfaceCaps.minImageCount;
		}

		return nRecommendedImages;
	}


	VkSurfaceTransformFlagBitsKHR VulkanSurface::GetRecommendedTransform() const
	{
		VkSurfaceCapabilitiesKHR vulkSurfaceCaps = {};
		VULK_ASSERT_SUCCESS(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, vulkanPhysicalDevice.GetVkPhysicalDevice(), vulkSurface, &vulkSurfaceCaps);

		return vulkSurfaceCaps.currentTransform;
	}


	bool VulkanSurface::RequestFormat(VkSurfaceFormatKHR& surfaceformat_, const VkFormat type_) const
	{
		// See if formats are in SCSInfo
		for (usize i = 0; i < swapChainSupportInfo.nFormats; ++i)
		{
			const VkSurfaceFormatKHR& curFormat = swapChainSupportInfo.formats[i];
			if (curFormat.format == type_)
			{
				surfaceformat_ = curFormat;
				return true;
			}
		}

		return false;
	}


	bool VulkanSurface::RequestPresentMode(VkPresentModeKHR& presentMode_, const VkPresentModeKHR type_) const
	{
		// See if modes are in SCSInfo
		for (usize i = 0; i < swapChainSupportInfo.nPresentModes; ++i)
		{
			const VkPresentModeKHR& curMode = swapChainSupportInfo.presentModes[i];
			if (curMode == type_)
			{
				presentMode_ = curMode;
				return true;
			}
		}

		return false;
	}


	bool VulkanSurface::RequestExtent(VkExtent2D& extent_) const
	{
		VkSurfaceCapabilitiesKHR vulkSurfaceCaps = {};
		VULK_ASSERT_SUCCESS(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, vulkanPhysicalDevice.GetVkPhysicalDevice(), vulkSurface, &vulkSurfaceCaps);

		// No width/height as params and declaring here as glfw output due to glfw bind to surface in CPsupport
		i16 width  = 0;
		i16 height = 0;
		glfwGetFramebufferSize(vulkanGLFW.GetGLWindow(), &width, &height);

		// Not much to do other than clamp values
		extent_.width  = std::clamp<uint32_t>(width, vulkSurfaceCaps.minImageExtent.width, vulkSurfaceCaps.maxImageExtent.width);
		extent_.height = std::clamp<uint32_t>(height, vulkSurfaceCaps.minImageExtent.height, vulkSurfaceCaps.maxImageExtent.height);

		// NOTE: This function feels wrong...
		return true;
	}


	bool VulkanSurface::CheckPresentingSupport()
	{
		// Use glfw to handle OS-code
		VULK_ASSERT_SUCCESS(glfwCreateWindowSurface, vulkanInstance.GetVkInstance(),
							vulkanGLFW.GetGLWindow(), nullptr, &vulkSurface);

		// Check that presenting queue supports this surface
		VkBool32 supported;
		VULK_ASSERT_SUCCESS(vkGetPhysicalDeviceSurfaceSupportKHR, vulkanPhysicalDevice.GetVkPhysicalDevice(),
						    vulkanQueueFamilies.GetQueueHandle(VulkanQueueFamilies::Presenting),
						    vulkSurface, &supported);

		return (supported == VK_TRUE);
	}


	bool VulkanSurface::CheckSwapchainSupport()
	{
		// Get num of supported formats
		swapChainSupportInfo.nFormats = 0;
		VULK_ASSERT_SUCCESS(vkGetPhysicalDeviceSurfaceFormatsKHR, vulkanPhysicalDevice.GetVkPhysicalDevice(), vulkSurface, &swapChainSupportInfo.nFormats, nullptr);

		// Alloc mem and populate with supported formats
		Utility::MemoryBlock supportedFormatsBlk = heapAllocator.Allocate<VkSurfaceFormatKHR>(swapChainSupportInfo.nFormats);
		swapChainSupportInfo.formats			 = (VkSurfaceFormatKHR*)supportedFormatsBlk.ptr;
		VULK_ASSERT_SUCCESS(vkGetPhysicalDeviceSurfaceFormatsKHR, vulkanPhysicalDevice.GetVkPhysicalDevice(), vulkSurface, &swapChainSupportInfo.nFormats, swapChainSupportInfo.formats);

		// Get number of supported presenting modes
		swapChainSupportInfo.nPresentModes = 0;
		VULK_ASSERT_SUCCESS(vkGetPhysicalDeviceSurfacePresentModesKHR, vulkanPhysicalDevice.GetVkPhysicalDevice(), vulkSurface, &swapChainSupportInfo.nPresentModes, nullptr);

		// Alloc mem and populate with supported presenting modes
		Utility::MemoryBlock presentModesBlk = heapAllocator.Allocate<VkPresentModeKHR>(swapChainSupportInfo.nPresentModes);
		swapChainSupportInfo.presentModes    = (VkPresentModeKHR*)presentModesBlk.ptr;
		VULK_ASSERT_SUCCESS(vkGetPhysicalDeviceSurfacePresentModesKHR, vulkanPhysicalDevice.GetVkPhysicalDevice(), vulkSurface, &swapChainSupportInfo.nPresentModes, swapChainSupportInfo.presentModes);

		return (swapChainSupportInfo.nFormats && swapChainSupportInfo.nPresentModes);
	}
}
