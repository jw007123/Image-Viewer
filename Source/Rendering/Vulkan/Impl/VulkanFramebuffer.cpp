#include "Rendering/Vulkan/Impl/VulkanFramebuffer.h"

namespace Rendering
{
	VulkanFramebuffer::VulkanFramebuffer(Utility::HeapAllocator& heapAllocator_,       Utility::StackAllocator& stackAllocator_,
										 VulkanLogicalDevice&    vulkanLogicalDevice_, VulkanSwapChain&			vulkanSwapChain_) :
										 vulkanLogicalDevice(vulkanLogicalDevice_),
									 	 vulkanSwapChain(vulkanSwapChain_),
										 heapAllocator(heapAllocator_),
										 stackAllocator(stackAllocator_)
	{

	}


	VulkanFramebuffer::~VulkanFramebuffer()
	{
		if (framebufferBlk.size)
		{
			const uint32_t nBuffers = framebufferBlk.size / sizeof(VkFramebuffer);
			for (uint32_t i = 0; i < nBuffers; ++i)
			{
				VkFramebuffer& fbuffer = *((VkFramebuffer*)framebufferBlk.ptr + i);
				vkDestroyFramebuffer(vulkanLogicalDevice.GetVkLogicalDevice(), fbuffer, nullptr);
			}

			heapAllocator.Free(framebufferBlk);
		}
	}


	bool VulkanFramebuffer::BindTo(VulkanPipeline& vulkanPipeline_)
	{
		// Determine no. of images in swapchain
		VkImageView testView;
		uint32_t nImages = 0;
		while (vulkanSwapChain.GetVkImageView(testView, nImages))
		{
			nImages++;
		}

		// Allocate mem if haven't already done so. Because SC is passed in CTOR, don't need to resize
		if (!framebufferBlk.size)
		{
			framebufferBlk = heapAllocator.Allocate<VkFramebuffer>(nImages);
		}

		// Create buffer for each image
		for (uint32_t i = 0; i < nImages; ++i)
		{
			VkImageView fbufferImageView;
			if (!vulkanSwapChain.GetVkImageView(fbufferImageView, i))
			{
				return false;
			}

			VkRenderPass fbufferRenderPass;
			if (!vulkanPipeline_.GetVkRenderPass(fbufferRenderPass))
			{
				return false;
			}

			VkFramebufferCreateInfo fbufferInfo = {};
			fbufferInfo.sType				    = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			fbufferInfo.attachmentCount			= 1;
			fbufferInfo.pAttachments		    = &fbufferImageView;
			fbufferInfo.renderPass				= fbufferRenderPass;
			fbufferInfo.width					= vulkanSwapChain.GetVkExtent().width;
			fbufferInfo.height					= vulkanSwapChain.GetVkExtent().height;
			fbufferInfo.layers					= 1;

			// Create each buffer
			VkFramebuffer& fbuffer = *((VkFramebuffer*)framebufferBlk.ptr + i);
			if (!VULK_CHECK_SUCCESS(vkCreateFramebuffer, vulkanLogicalDevice.GetVkLogicalDevice(), &fbufferInfo, nullptr, &fbuffer))
			{
				return false;
			}
		}

		return true;
	}


	bool VulkanFramebuffer::GetVkFramebuffer(VkFramebuffer& vulkFramebuffer_, const uint32_t idx_)
	{
		const uint32_t nBuffers = framebufferBlk.size / sizeof(VkFramebuffer);
		if (idx_ > nBuffers)
		{
			return false;
		}

		vulkFramebuffer_ =  *((VkFramebuffer*)framebufferBlk.ptr + idx_);
		return true;
	}
}
