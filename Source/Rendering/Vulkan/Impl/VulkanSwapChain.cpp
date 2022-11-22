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

		// Must be called in this order as ...Views assumes existence of block
		SetupVulkImages();
		SetupVulkImageViews();
	}


	VulkanSwapChain::~VulkanSwapChain()
	{
		// Destroy images before swapchain
		const uint32_t nImages = vulkImageBlk.size / sizeof(VkImage);
		for (uint32_t i = 0; i < nImages; ++i)
		{
			VkImageView& vulkImageView = *(VkImageView*)((VkImageView*)vulkImageViewBlk.ptr + i);
			vkDestroyImageView(vulkanLogicalDevice.GetVkLogicalDevice(), vulkImageView, nullptr);
		}

		// Destroy swapchain
		vkDestroySwapchainKHR(vulkanLogicalDevice.GetVkLogicalDevice(), vulkSwapChain, nullptr);

		// Free allocated blocks
		heapAllocator.Free(vulkImageBlk);
		heapAllocator.Free(vulkImageViewBlk);
	}


	bool VulkanSwapChain::GetVkImage(VkImage& vulkImage_, const uint32_t idx_)
	{
		const uint32_t nImages = vulkImageBlk.size / sizeof(VkImage);
		if (idx_ > nImages)
		{
			return false;
		}

		// See, C is good
		vulkImage_ = *((VkImage*)vulkImageBlk.ptr + idx_);
		return true;
	}


	bool VulkanSwapChain::GetVkImageView(VkImageView& vulkImageView_, const uint32_t idx_)
	{
		const uint32_t nViews = vulkImageViewBlk.size / sizeof(VkImageView);
		if (idx_ > nViews)
		{
			return false;
		}

		// See, C is good
		vulkImageView_ = *((VkImageView*)vulkImageViewBlk.ptr + idx_);
		return true;
	}


	VkExtent2D& VulkanSwapChain::GetVkExtent()
	{
		return vulkExtent;
	}


	VkFormat& VulkanSwapChain::GetVkFormat()
	{
		return vulkFormat;
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


	void VulkanSwapChain::SetupVulkImages()
	{
		// Cache images in block
		uint32_t nImages = 0;
		VULK_ASSERT_SUCCESS(vkGetSwapchainImagesKHR, vulkanLogicalDevice.GetVkLogicalDevice(), vulkSwapChain, &nImages, nullptr);

		vulkImageBlk		= heapAllocator.Allocate<VkImage>(nImages);
		VkImage* vulkImages = (VkImage*)vulkImageBlk.ptr;
		VULK_ASSERT_SUCCESS(vkGetSwapchainImagesKHR, vulkanLogicalDevice.GetVkLogicalDevice(), vulkSwapChain, &nImages, vulkImages);
	}


	void VulkanSwapChain::SetupVulkImageViews()
	{
		// Get image count and alloc mem
		const uint32_t nImages		= vulkImageBlk.size / sizeof(VkImage);
		vulkImageViewBlk			= heapAllocator.Allocate<VkImageView>(nImages);
		VkImageView* vulkImageViews = (VkImageView*)vulkImageViewBlk.ptr;

		// Create views
		for (uint32_t i = 0; i < nImages; ++i)
		{
			VkImageViewCreateInfo createInfo		   = {};
			createInfo.sType						   = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image						   = *(VkImage*)((VkImage*)vulkImageBlk.ptr + i);
			createInfo.viewType						   = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format						   = vulkFormat;
			createInfo.components.r					   = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g					   = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b					   = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a					   = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask	   = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel   = 0;
			createInfo.subresourceRange.levelCount	   = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount	   = 1;

			VULK_ASSERT_SUCCESS(vkCreateImageView, vulkanLogicalDevice.GetVkLogicalDevice(), &createInfo, nullptr, &vulkImageViews[i]);
		}
	}
}
