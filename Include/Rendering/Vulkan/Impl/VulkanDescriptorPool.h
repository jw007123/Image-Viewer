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
#include "Rendering/Vulkan/Impl/VulkanUBO.h"
#include "Rendering/Vulkan/Impl/VulkanMeshData.h"

namespace Rendering
{
	class VulkanDescriptorPool
	{
	public:
		struct CreateInfo
		{
			uint32_t		 size;
			VkDescriptorType type;
		};

		VulkanDescriptorPool(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, VulkanLogicalDevice& vulkanLogicalDevice_);
		~VulkanDescriptorPool();

		void Create(const CreateInfo& createInfo_);
		void Allocate(const VulkanUBO& ubo_);

	private:
		bool			 created;
		VkDescriptorPool vulkDescriptorPool;

		VulkanLogicalDevice&     vulkanLogicalDevice;
		Utility::StackAllocator& stackAllocator;
		Utility::HeapAllocator&  heapAllocator;
	};
}
