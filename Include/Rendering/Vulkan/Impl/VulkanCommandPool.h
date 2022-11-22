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

namespace Rendering
{
	class VulkanCommandPool
	{
	public:
		VulkanCommandPool(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_,
						  VulkanLogicalDevice& vulkanLogicalDevice_);
		~VulkanCommandPool();

		bool BindTo(VulkanPipeline& vulkanPipeline_);

		bool GetVkFramebuffer(VkFramebuffer& vulkFramebuffer_, const uint32_t idx_);

	private:
		VulkanLogicalDevice& vulkanLogicalDevice;

		Utility::StackAllocator& stackAllocator;
		Utility::HeapAllocator& heapAllocator;
	};
}
