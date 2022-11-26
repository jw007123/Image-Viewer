#include "Rendering/Vulkan/Backend.h"

namespace Rendering
{
	VBackend::VBackend(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_) :
					   vulkanGlfw(),
				       vulkanInstance(stackAllocator_),
					   vulkanPhysicalDevice(stackAllocator_, vulkanInstance),
					   vulkanQueueFamilies(stackAllocator_, vulkanPhysicalDevice),
					   vulkanLogicalDevice(stackAllocator_, vulkanQueueFamilies, vulkanPhysicalDevice),
					   vulkanVma(vulkanPhysicalDevice, vulkanLogicalDevice, vulkanInstance),
					   vulkanSurface(vulkanInstance, vulkanGlfw, vulkanQueueFamilies, vulkanPhysicalDevice, heapAllocator_),
					   vulkanSwapChain(heapAllocator_, vulkanPhysicalDevice, vulkanSurface, vulkanQueueFamilies, vulkanLogicalDevice, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
					   vulkanCommandPool(heapAllocator_, stackAllocator_, vulkanLogicalDevice, vulkanQueueFamilies, vulkanSwapChain),
					   vulkanImGui(vulkanGlfw)
	{
		// Set as invalid
		imageTargetIdx = -1;

		// Create sems
		VkSemaphoreCreateInfo semCreateInfo = {};
		semCreateInfo.sType					= VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VULK_ASSERT_SUCCESS(vkCreateSemaphore, vulkanLogicalDevice.GetVkLogicalDevice(), &semCreateInfo, nullptr, &vulkImageAvailableSem);
		VULK_ASSERT_SUCCESS(vkCreateSemaphore, vulkanLogicalDevice.GetVkLogicalDevice(), &semCreateInfo, nullptr, &vulkImageFinishedSem);

		// Create fences
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType			  = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags			  = VK_FENCE_CREATE_SIGNALED_BIT;
		VULK_ASSERT_SUCCESS(vkCreateFence, vulkanLogicalDevice.GetVkLogicalDevice(), &fenceCreateInfo, nullptr, &vulkInFlightFence);
	}


	VBackend::~VBackend()
	{
		vkDestroySemaphore(vulkanLogicalDevice.GetVkLogicalDevice(), vulkImageAvailableSem, nullptr);
		vkDestroySemaphore(vulkanLogicalDevice.GetVkLogicalDevice(), vulkImageFinishedSem, nullptr);
		vkDestroyFence(vulkanLogicalDevice.GetVkLogicalDevice(), vulkInFlightFence, nullptr);
	}


	bool VBackend::IsRunning()
	{
		return vulkanGlfw.IsWindowOpen();
	}


	void VBackend::StartFrame()
	{
		vulkanGlfw.PollEvents();

		VULK_ASSERT_SUCCESS(vkWaitForFences, vulkanLogicalDevice.GetVkLogicalDevice(), 1, &vulkInFlightFence, VK_TRUE, UINT64_MAX);
		VULK_ASSERT_SUCCESS(vkResetFences, vulkanLogicalDevice.GetVkLogicalDevice(), 1, &vulkInFlightFence);

		// NOTE: Does this make sense...?
		if (imageTargetIdx == -1)
		{
			GetImageTarget(imageTargetIdx);
		}

		vulkanCommandPool.Reset();
	}


	void VBackend::EndFrame()
	{
		VkSemaphore waitSems[]			  = { vulkImageAvailableSem };
		VkSemaphore signalSems[]		  = { vulkImageFinishedSem };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSwapchainKHR swapChains[]		  = { vulkanSwapChain.GetVkSwapchainKHR() };

		VkSubmitInfo submitInfo		    = {};
		submitInfo.sType			    = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount   = 1;
		submitInfo.pWaitSemaphores      = waitSems;
		submitInfo.pWaitDstStageMask    = waitStages;
		submitInfo.commandBufferCount   = 1;
		submitInfo.pCommandBuffers	    = &vulkanCommandPool.GetVkCommandBuffer();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores    = signalSems;

		VkQueue& graphicsQueue = vulkanLogicalDevice.GetVkQueue(VulkanQueueFamilies::Graphics);
		VkQueue& presentQueue  = vulkanLogicalDevice.GetVkQueue(VulkanQueueFamilies::Presenting);
		VULK_ASSERT_SUCCESS(vkQueueSubmit, graphicsQueue, 1, &submitInfo, vulkInFlightFence);

		VkPresentInfoKHR presentInfo   = {};
		presentInfo.sType			   = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores    = signalSems;
		presentInfo.swapchainCount	   = 1;
		presentInfo.pSwapchains        = swapChains;
		presentInfo.pImageIndices	   = &imageTargetIdx;
		presentInfo.pResults		   = nullptr;

		VULK_ASSERT_SUCCESS(vkQueuePresentKHR, presentQueue, &presentInfo);
		imageTargetIdx = -1;
	}


	bool VBackend::GetImageTarget(uint32_t& imageIdx_)
	{
		if (imageTargetIdx != -1)
		{
			imageIdx_ = imageTargetIdx;
			return true;
		}
		else
		{
			const bool res = VULK_CHECK_SUCCESS(vkAcquireNextImageKHR, vulkanLogicalDevice.GetVkLogicalDevice(), vulkanSwapChain.GetVkSwapchainKHR(),
												UINT64_MAX, vulkImageAvailableSem, VK_NULL_HANDLE, &imageIdx_);
			if (res)
			{
				imageTargetIdx = imageIdx_;
			}
			return res;
		}
	}
}
