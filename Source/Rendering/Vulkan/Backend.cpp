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
					   vulkanCommandPool
					   {
					       {heapAllocator_, stackAllocator_, vulkanLogicalDevice, vulkanQueueFamilies, vulkanSwapChain},
					       {heapAllocator_, stackAllocator_, vulkanLogicalDevice, vulkanQueueFamilies, vulkanSwapChain}
					   },
					   vulkanImGui(vulkanGlfw)
	{
		// First vkFence call is invalid. Setting to 0 to avoid OoB access
		imageTargetIdx  = 0;
		currentFrameIdx = 0;
		swapChainOoD    = false;
		isMinimised		= false;

		// Create sems
		for (usize i = 0; i < Consts::maxFramesInFlight; ++i)
		{
			VkSemaphoreCreateInfo semCreateInfo = {};
			semCreateInfo.sType					= VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VULK_ASSERT_SUCCESS(vkCreateSemaphore, vulkanLogicalDevice.GetVkLogicalDevice(), &semCreateInfo, nullptr, &vulkImageAvailableSem[i]);
			VULK_ASSERT_SUCCESS(vkCreateSemaphore, vulkanLogicalDevice.GetVkLogicalDevice(), &semCreateInfo, nullptr, &vulkImageFinishedSem[i]);
		}
		
		// Create fences
		for (usize i = 0; i < Consts::maxFramesInFlight; ++i)
		{
			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType			  = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags			  = VK_FENCE_CREATE_SIGNALED_BIT;
			VULK_ASSERT_SUCCESS(vkCreateFence, vulkanLogicalDevice.GetVkLogicalDevice(), &fenceCreateInfo, nullptr, &vulkInFlightFence[i]);
		}
	}


	VBackend::~VBackend()
	{
		vulkanLogicalDevice.WaitFor();

		for (usize i = 0; i < Consts::maxFramesInFlight; ++i)
		{
			vkDestroySemaphore(vulkanLogicalDevice.GetVkLogicalDevice(), vulkImageAvailableSem[i], nullptr);
			vkDestroySemaphore(vulkanLogicalDevice.GetVkLogicalDevice(), vulkImageFinishedSem[i], nullptr);
			vkDestroyFence(vulkanLogicalDevice.GetVkLogicalDevice(), vulkInFlightFence[i], nullptr);
		}
	}


	bool VBackend::IsRunning()
	{
		return vulkanGlfw.IsWindowOpen();
	}


	void VBackend::StartFrame()
	{
		vulkanGlfw.PollEvents();
		VULK_ASSERT_SUCCESS(vkWaitForFences, vulkanLogicalDevice.GetVkLogicalDevice(), 1, &vulkInFlightFence[currentFrameIdx], VK_TRUE, UINT64_MAX);

		// If window is minimised, do nothing
		i16 height;
		i16 width;
		glfwGetFramebufferSize(vulkanGlfw.GetGLWindow(), &height, &width);
		isMinimised = ((height * width) == 0);
		if (isMinimised)
		{
			return;
		}

		const VkResult imgResult = vkAcquireNextImageKHR(vulkanLogicalDevice.GetVkLogicalDevice(), vulkanSwapChain.GetVkSwapchainKHR(),
														 UINT64_MAX, vulkImageAvailableSem[currentFrameIdx], VK_NULL_HANDLE, &imageTargetIdx);

		// If image is out of date, recreate the full chain
		if (imgResult == VK_ERROR_OUT_OF_DATE_KHR)
		{
			// Signal other classes to recreate their comps
			swapChainOoD = true;
			return;
		}

		VULK_ASSERT_SUCCESS(vkResetFences, vulkanLogicalDevice.GetVkLogicalDevice(), 1, &vulkInFlightFence[currentFrameIdx]);
		vulkanCommandPool[currentFrameIdx].Reset();
	}


	void VBackend::EndFrame()
	{
		if (swapChainOoD)
		{
			// Must be done
			swapChainOoD = false;
			return;
		}
		else if (isMinimised)
		{
			return;
		}

		VkSemaphore waitSems[]			  = { vulkImageAvailableSem[currentFrameIdx] };
		VkSemaphore signalSems[]		  = { vulkImageFinishedSem[currentFrameIdx] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSwapchainKHR swapChains[]		  = { vulkanSwapChain.GetVkSwapchainKHR() };

		VkSubmitInfo submitInfo		    = {};
		submitInfo.sType			    = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount   = 1;
		submitInfo.pWaitSemaphores      = waitSems;
		submitInfo.pWaitDstStageMask    = waitStages;
		submitInfo.commandBufferCount   = 1;
		submitInfo.pCommandBuffers	    = &vulkanCommandPool[currentFrameIdx].GetVkCommandBuffer();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores    = signalSems;

		VkQueue& graphicsQueue = vulkanLogicalDevice.GetVkQueue(VulkanQueueFamilies::Graphics);
		VkQueue& presentQueue  = vulkanLogicalDevice.GetVkQueue(VulkanQueueFamilies::Presenting);
		VULK_ASSERT_SUCCESS(vkQueueSubmit, graphicsQueue, 1, &submitInfo, vulkInFlightFence[currentFrameIdx]);

		VkPresentInfoKHR presentInfo   = {};
		presentInfo.sType			   = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores    = signalSems;
		presentInfo.swapchainCount	   = 1;
		presentInfo.pSwapchains        = swapChains;
		presentInfo.pImageIndices	   = &imageTargetIdx;
		presentInfo.pResults		   = nullptr;

		// If != VK_TRUE, window may be hidden
		const VkResult queueResult = vkQueuePresentKHR(presentQueue, &presentInfo);
		if (queueResult == VK_ERROR_OUT_OF_DATE_KHR || queueResult == VK_SUBOPTIMAL_KHR)
		{
			swapChainOoD = true;
		}

		// Increment frame idx
		currentFrameIdx = (currentFrameIdx + 1) % Consts::maxFramesInFlight;
	}
}
