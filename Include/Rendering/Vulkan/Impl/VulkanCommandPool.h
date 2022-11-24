#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"
#include "Utility/HeapAllocator.h"

#include "vma/vk_mem_alloc.h"

#include "Rendering/Vulkan/Impl/VulkanUtility.h"
#include "Rendering/Vulkan/Impl/VulkanShader.h"
#include "Rendering/Vulkan/Impl/VulkanLogicalDevice.h"
#include "Rendering/Vulkan/Impl/VulkanSwapChain.h"
#include "Rendering/Vulkan/Impl/VulkanPipeline.h"
#include "Rendering/Vulkan/Impl/VulkanQueueFamilies.h"

namespace Rendering
{
	class VulkanCommandPool
	{
	public:
		VulkanCommandPool(Utility::HeapAllocator& heapAllocator_,       Utility::StackAllocator& stackAllocator_,
						  VulkanLogicalDevice&    vulkanLogicalDevice_, VulkanQueueFamilies&     vulkanQueueFamiles_,
						  VulkanSwapChain&		  vulkanSwapChain_,		VulkanFramebuffer&		 vulkanFramebuffer_);
		~VulkanCommandPool();

		bool RecordToBuffer(VulkanPipeline& vulkanPipeline_, const uint32_t imageIdx_);

	private:
		VkCommandPool   vulkCommandPool;
		VkCommandBuffer vulkCommandBuffer;

		VulkanLogicalDevice& vulkanLogicalDevice;
		VulkanQueueFamilies& vulkanQueueFamilies;
		VulkanSwapChain&	 vulkanSwapChain;
		VulkanFramebuffer&   vulkanFramebuffer;

		Utility::StackAllocator& stackAllocator;
		Utility::HeapAllocator&  heapAllocator;

		bool BeginRenderPass(VulkanPipeline& vulkanPipeline_, const uint32_t imageIdx_);
	};
}
