#include "Rendering/Vulkan/Impl/VulkanSwapChain.h"

namespace Rendering
{
	VulkanSwapChain::VulkanSwapChain(Utility::HeapAllocator& heapAllocator_,       VulkanPhysicalDevice&    vulkanPhysicalDevice_,
									 VulkanSurface&			 vulkanSurface_,       VulkanQueueFamilies&     vulkanQueueFamilies_,
									 VulkanLogicalDevice&    vulkanLogicalDevice_, const VkImageUsageFlags  usage_) :
									 vulkanPhysicalDevice(vulkanPhysicalDevice_),
									 vulkanSurface(vulkanSurface_),
									 vulkanQueueFamilies(vulkanQueueFamilies_),
									 vulkanLogicalDevice(vulkanLogicalDevice_),
									 heapAllocator(heapAllocator_)
	{
		// Create chain with requested format and present mode. Use surface size as extent
		VkSurfaceFormatKHR scFormat      = {};
		VkPresentModeKHR   scPresentMode = {};
		VkExtent2D		   scExtent		 = {};
		if (!vulkanSurface.RequestFormat(scFormat, VK_FORMAT_B8G8R8A8_SRGB)				  ||
			!vulkanSurface.RequestPresentMode(scPresentMode, VK_PRESENT_MODE_MAILBOX_KHR) ||
			!vulkanSurface.RequestExtent(scExtent))
		{
			assert(0);
		}

		// Create chain. usage_ probably going to be VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
		VkSwapchainCreateInfoKHR scCreateInfo = {};
		scCreateInfo.sType					  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		scCreateInfo.surface				  = vulkanSurface.GetVkSurface();
		scCreateInfo.minImageCount			  = vulkanSurface.GetRecommendedImageCount();
		scCreateInfo.imageFormat			  = scFormat.format;
		scCreateInfo.imageColorSpace		  = scFormat.colorSpace;
		scCreateInfo.imageExtent			  = scExtent;
		scCreateInfo.imageArrayLayers         = 1;
		scCreateInfo.imageUsage			      = usage_;
		scCreateInfo.preTransform			  = vulkanSurface.GetRecommendedTransform();
		scCreateInfo.compositeAlpha			  = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		scCreateInfo.presentMode			  = scPresentMode;
		scCreateInfo.clipped				  = VK_TRUE;
		scCreateInfo.oldSwapchain			  = VK_NULL_HANDLE;

		// Determine chain ownership
		FillQueueInfo(scCreateInfo);

		VULK_ASSERT_SUCCESS(vkCreateSwapchainKHR, vulkanLogicalDevice.GetVkLogicalDevice(), &scCreateInfo, nullptr, &vulkSwapChain);

		// Fill in rest of class data from chain
		vulkFormat = scFormat.format;
		vulkExtent = scExtent;

		// Cache images in block
		uint32_t nImages = 0;
		VULK_ASSERT_SUCCESS(vkGetSwapchainImagesKHR, vulkanLogicalDevice.GetVkLogicalDevice(), vulkSwapChain, &nImages, nullptr);

		vulkImageBlk		= heapAllocator.Allocate<VkImage>(nImages);
		VkImage* vulkImages = (VkImage*)vulkImageBlk.ptr;
		VULK_ASSERT_SUCCESS(vkGetSwapchainImagesKHR, vulkanLogicalDevice.GetVkLogicalDevice(), vulkSwapChain, &nImages, vulkImages);
	}


	VulkanSwapChain::~VulkanSwapChain()
	{
		vkDestroySwapchainKHR(vulkanLogicalDevice.GetVkLogicalDevice(), vulkSwapChain, nullptr);
	}


	VkImage& VulkanSwapChain::GetVkImage(const uint32_t idx_)
	{
		const uint32_t nImages = vulkImageBlk.size / sizeof(VkImage);
		if (idx_ > nImages)
		{
			assert(0);
		}

		// See, C is good
		return *(VkImage*)((VkImage*)vulkImageBlk.ptr + idx_);
	}


	void VulkanSwapChain::FillQueueInfo(VkSwapchainCreateInfoKHR& scCreateInfo_)
	{
		uint32_t idxs[2];
		idxs[0] = vulkanQueueFamilies.GetQueueHandle(VulkanQueueFamilies::Graphics);
		idxs[1] = vulkanQueueFamilies.GetQueueHandle(VulkanQueueFamilies::Presenting);

		if (idxs[0] != idxs[1])
		{
			scCreateInfo_.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
			scCreateInfo_.queueFamilyIndexCount = 2;
			scCreateInfo_.pQueueFamilyIndices   = idxs;
		}
		else
		{
			scCreateInfo_.imageSharingMode		= VK_SHARING_MODE_EXCLUSIVE;
			scCreateInfo_.queueFamilyIndexCount = 0;
			scCreateInfo_.pQueueFamilyIndices   = nullptr;
		}
	}
}
