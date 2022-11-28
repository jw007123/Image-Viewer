#include "Rendering/Vulkan/Impl/VulkanCommandPool.h"

namespace Rendering
{
	VulkanCommandPool::VulkanCommandPool(Utility::HeapAllocator& heapAllocator_,       Utility::StackAllocator& stackAllocator_,
										 VulkanLogicalDevice&    vulkanLogicalDevice_, VulkanQueueFamilies&     vulkanQueueFamiles_,
										 VulkanSwapChain&		 vulkanSwapChain_) :
										 vulkanLogicalDevice(vulkanLogicalDevice_),
										 vulkanQueueFamilies(vulkanQueueFamiles_),
										 vulkanSwapChain(vulkanSwapChain_),
										 heapAllocator(heapAllocator_),
										 stackAllocator(stackAllocator_)
	{
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType					 = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags					 = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex		 = vulkanQueueFamilies.GetQueueHandle(VulkanQueueFamilies::Graphics);

		VULK_ASSERT_SUCCESS(vkCreateCommandPool, vulkanLogicalDevice.GetVkLogicalDevice(), &poolInfo, nullptr, &vulkCommandPool);

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType						  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool				  = vulkCommandPool;
		allocInfo.level						  = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount		  = 1;

		VULK_ASSERT_SUCCESS(vkAllocateCommandBuffers, vulkanLogicalDevice.GetVkLogicalDevice(), &allocInfo, &vulkCommandBuffer);
	}


	VulkanCommandPool::~VulkanCommandPool()
	{
		vkDestroyCommandPool(vulkanLogicalDevice.GetVkLogicalDevice(), vulkCommandPool, nullptr);
	}


	bool VulkanCommandPool::Reset()
	{
		return VULK_CHECK_SUCCESS(vkResetCommandBuffer, vulkCommandBuffer, 0);
	}


	bool VulkanCommandPool::RecordToBuffer(VulkanPipeline& vulkanPipeline_, VulkanFramebuffer& vulkanFramebuffer_,
										   const DrawInfo& drawInfo_,		const uint32_t	   imageIdx_)
	{
		const bool begun = BeginRenderPass(vulkanPipeline_, vulkanFramebuffer_, imageIdx_);
		if (!begun)
		{
			return false;
		}

		VkPipeline vulkPipeline;
		if (!vulkanPipeline_.GetVkPipeline(vulkPipeline))
		{
			return false;
		}

		vkCmdBindPipeline(vulkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkPipeline);
		vkCmdBindVertexBuffers(vulkCommandBuffer, 0, drawInfo_.nVertBuffers, drawInfo_.vertBuffers, drawInfo_.vertOffsets);
		for (usize i = 0; i < drawInfo_.nVertBuffers; ++i)
		{
			vkCmdDraw(vulkCommandBuffer, drawInfo_.verticesCnt[i], 1, 0, 0);
		}
		vkCmdEndRenderPass(vulkCommandBuffer);

		return VULK_CHECK_SUCCESS(vkEndCommandBuffer, vulkCommandBuffer);
	}


	VkCommandBuffer& VulkanCommandPool::GetVkCommandBuffer()
	{
		return vulkCommandBuffer;
	}


	bool VulkanCommandPool::BeginRenderPass(VulkanPipeline& vulkanPipeline_, VulkanFramebuffer& vulkanFramebuffer_, const uint32_t imageIdx_)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType					   = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags					   = 0;
		beginInfo.pInheritanceInfo		   = nullptr;

		if (!VULK_CHECK_SUCCESS(vkBeginCommandBuffer, vulkCommandBuffer, &beginInfo))
		{
			return false;
		}

		VkRenderPass vulkRenderPass;
		if (!vulkanPipeline_.GetVkRenderPass(vulkRenderPass))
		{
			return false;
		}

		VkFramebuffer vulkFramebuffer;
		if (!vulkanFramebuffer_.GetVkFramebuffer(vulkFramebuffer, imageIdx_))
		{
			return false;
		}

		VkClearValue clearColour = {};
		clearColour.color		 = { 0.0f, 0.0f, 0.0f, 1.0f };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType				 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass			 = vulkRenderPass;
		renderPassInfo.framebuffer			 = vulkFramebuffer;
		renderPassInfo.renderArea.offset.x	 = 0;
		renderPassInfo.renderArea.offset.y	 = 0;
		renderPassInfo.renderArea.extent	 = vulkanSwapChain.GetVkExtent();
		renderPassInfo.clearValueCount		 = 1;
		renderPassInfo.pClearValues			 = &clearColour;
	
		vkCmdBeginRenderPass(vulkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		return true;
	}
}
