#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"

#include "vma/vk_mem_alloc.h"

#include "Rendering/Vulkan/Impl/VulkanUtility.h"
#include "Rendering/Vulkan/Impl/VulkanPhysicalDevice.h"

namespace Rendering
{
	class VulkanQueueFamilies
	{
	public:
		enum IDs : u8
		{
			Graphics   = 0,
			Presenting = 1,
			Num
		};

		VulkanQueueFamilies(Utility::StackAllocator& stackAllocator_, VulkanPhysicalDevice& vulkanPhysicalDevice_);
		~VulkanQueueFamilies();

		uint32_t GetQueueHandle(const IDs idx_) const;

		bool IsValid() const;

	private:
		struct Consts
		{
			static constexpr VkQueueFlagBits requiredQueueBits[IDs::Num] =
			{
				VK_QUEUE_GRAPHICS_BIT,
				VK_QUEUE_GRAPHICS_BIT
			};
		};

		uint32_t			  queueHandles[IDs::Num];
		VulkanPhysicalDevice& vulkanPhysicalDevice;
	};
}
