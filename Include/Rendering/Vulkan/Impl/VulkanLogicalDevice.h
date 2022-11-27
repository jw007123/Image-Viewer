#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"

#include "vma/vk_mem_alloc.h"

#include "Rendering/Vulkan/Impl/VulkanUtility.h"
#include "Rendering/Vulkan/Impl/VulkanQueueFamilies.h"
#include "Rendering/Vulkan/Impl/VulkanPhysicalDevice.h"

namespace Rendering
{
	class VulkanLogicalDevice
	{
	public:
		VulkanLogicalDevice(Utility::StackAllocator& stackAllocator_, VulkanQueueFamilies& vulkanQueueFamilies_,
																	  VulkanPhysicalDevice& vulkanPhysicalDevice_);
		~VulkanLogicalDevice();

		VkDevice& GetVkLogicalDevice();
		VkQueue&  GetVkQueue(const VulkanQueueFamilies::IDs idx_);

		void WaitFor();

	private:
		struct Consts
		{
			static constexpr usize nRequiredLayers							   = 1;
			static constexpr const char* debugValidationLayer[nRequiredLayers] =
			{
				"VK_LAYER_KHRONOS_validation"
			};
			static constexpr usize nRequiredExtensions						     = 1;
			static constexpr const char* requiredExtensions[nRequiredExtensions] =
			{
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};
		};

		VkDevice			  vulkLogicalDevice;
		VkQueue				  vulkQueues[VulkanQueueFamilies::Num];
		VulkanQueueFamilies&  vulkanQueueFamilies;
		VulkanPhysicalDevice& vulkanPhysicalDevice;

		bool CheckRequiredExtensions(Utility::StackAllocator& stackAllocator_, const usize nRequiredExtensions_, const char* const* requiredExtensions_) const;
		bool CheckRequiredValidationLayers(Utility::StackAllocator& stackAllocator_, const usize nRequiredLayers_, const char* const* requiredLayers_) const;
	};
}
